/**
 * Copyright 2014 Vestel.  All rights reserved.
 *
 * @file
 *
 * @brief This file contains the implementation of the class
 * OperaMuminVideoBroadcastBackend
 */
#include "config/include_opapp_support.h"

#include "config/using_sdk422.h"
#include "config/using_sdk423.h"

#include "OperaMuminVideoBroadcastBackend.hpp"
#include "OperaMuminHelper.hpp"

#include "OperaMuminAVComponent.hpp"
#include "OperaMuminAVComponentHelper.hpp"
#include "OperaMuminObjectFactory.hpp"
#include "vewd_common/bpi/bpi_graphics.hpp"
#include "vewd_integration/oipf/OperaOipfChannel.hpp"
#include "vewd_integration/oipf/OperaOipfChannelConfig.hpp"
#include "vewd_integration/oipf/parental_control_manager/OperaOipfParentalRating.hpp"
#include "vewd_integration/oipf/parental_control_manager/OperaOipfParentalRatingCollection.hpp"
#include "vewd_integration/oipf/OperaOipfProgramme.hpp"
#include "profile_helper/ProfileHelper.h"
#include "language/LanguageCode.hpp"

#include "uva_client.h"
#include "uva_event.h"
#include "uva_channel.h"

#include "frost/frost_debug.h"
#include "frost/frost_time.h"
#include "nebula/adaptation_layer/nebula_eit_query.h"
#include "nebula/adaptation_layer/nebula_temi_timeline.h"

#include "utilities_debug/trace.h"
#include "utilities_debug/cabot_assert.h"
#include "utilities_debug/unused.h"

#ifdef INCLUDE_OPAPP_SUPPORT
#include "vewd_integration/opapp/HbbTvPackageUrl.hpp"
#include "nebula/opapp/nebula_opapp_event_publisher_provider.hpp"
#endif

#include <cstring>
#include <inttypes.h>

TRACE_IMPLEMENT("mumin/vb_backend");

namespace
{
    /**
     * @brief Convert the quiet_value used for a service selection 
     *        to the correspondent NEBULA value
     */
    NEBULA_ChannelChangeQuietValue convertQuietValuetoNebulaType(int quiet_value)
    {
        NEBULA_ChannelChangeQuietValue set_channel_quiet_value;
        switch (quiet_value)
        {
            case nebula_channel_change_no_info_displayed:
                set_channel_quiet_value = nebula_channel_change_no_info_displayed;
                break;
            case nebula_quiet_channel_change:
                set_channel_quiet_value = nebula_quiet_channel_change;
                break;
            default: 
                set_channel_quiet_value = nebula_channel_change_normal;
                break;
        }
        return set_channel_quiet_value;
    }

    int convertNebulaQuietValue(NEBULA_ChannelChangeQuietValue nebula_quiet_value)
    {
        // See ETSI TS 102 796 V1.5.1 section A.2.4.3.1
        int quiet_value = 0;
        switch (nebula_quiet_value)
        {
            case nebula_channel_change_no_info_displayed:
                quiet_value = 1;
                break;
            case nebula_quiet_channel_change:
                quiet_value = 2;
                break;
            default: 
                break;
        }
        return quiet_value;
    }

    std::string getCcidFromChannel(const std::shared_ptr<UVAChannel> channel)
    {
        std::string ccid;
        if (channel)
        {
            if (UVA_OK != channel->getCcid(ccid))
            {
                ccid.clear();
            }
        }
        return ccid;
    }

    /**
     * @brief Convert a UTF-8 encoded DSD into raw descriptor bytes.
     * 
     * @return true if the conversion was successful, false if the UTF-8 DSD
     *         does not encode valid ISO Latin-1 characters.
     */
    bool decodeDsd(std::string const & utf8_dsd, std::string & raw_dsd)
    {
        // Use UString as an intermediary rather than manually decoding the
        // string using low-level UTF-8 functions.
        bool success = true;
        UString const tmp_utf8_dsd(utf8_dsd.data(), utf8_dsd.length());
        
        // Each character is specified as a single byte ISO Latin-1 code.
        // Therefore, the byte length should be the same as the character length.
        std::string tmp_raw_dsd(tmp_utf8_dsd.length(), '\0');
        std::string::size_type i;
        CABOT_ASSERT(tmp_utf8_dsd.length() == tmp_raw_dsd.length());
        for (i = 0; success && i < tmp_raw_dsd.length(); ++i)
        {
            int const c = tmp_utf8_dsd.charAt(i);
            success = (c == (c & 0xff)); // Check for non Latin-1 characters.
            if (success)
            {
                tmp_raw_dsd[i] = static_cast<unsigned char>(c);
            }
        }
        if (success)
        {
            raw_dsd = tmp_raw_dsd;
        }
        return success;
    }

    const uva_uint64 pcr_ticks_per_sec = 90000;

    static OperaMuminVideoBroadcastBackend* test_backend = nullptr;

    void temiTest(void* param)
    {
        const FROST_DebugParamStringInt* p = static_cast<FROST_DebugParamStringInt*>(param);
        if (!p || !p->string_part)
        {
            return;
        }

        if (test_backend)
        {
            const int command = static_cast<const int>(p->int_part);
            const std::string& selector(static_cast<const char*>(p->string_part));

            // cmd: enable, disable, get position, get details
            switch (command)
            {
            case 1:
                test_backend->enableTimeline(selector);
                break;
            case 2:
                test_backend->disableTimeline(selector);
                break;
            case 3:
            {
                UVATime uva_time = test_backend->getPosition(selector);
                (void)uva_time;
                break;
            }
            case 4:
                test_backend->getTimelineDetails(selector);
                break;
            }
        }
    }

    bool registerTestTool()
    {
        static frost_bool registered = FROST_RegisterCommand("TEMI",
                "<selector_string> command: 1-enable; 2-disable; 3-get position; 4-get details",
                temiTest, FROST_debug_param_string_int);
        return registered != frost_false;
    }
} // namespace

OperaMuminVideoBroadcastBackend::OperaMuminVideoBroadcastBackend(UVAClient* client,
        OperaMuminObjectFactory& object_factory,
        AnyVideoBroadcastObjectTypeConverter& vbo_type_converter)
    : UVAVideoBroadcastBackend(),
      m_backend_mode(MODE_VIDEO_BROADCAST),
      m_client(client),
      m_vb_object(nullptr),
      m_component_helper(0),
      m_object_factory(object_factory),
      m_vbo_type_converter(vbo_type_converter),
      m_pending_quiet_value(nebula_channel_change_normal),
      m_window_id(0)
{
    TRACE_INFO(("OperaMuminVideoBroadcastBackend() [%p]\n", this));

    (void)registerTestTool();
    if (test_backend == nullptr)
    {
        test_backend = this;
    }

    m_opapp_running = false;
}

OperaMuminVideoBroadcastBackend::~OperaMuminVideoBroadcastBackend()
{
    TRACE_INFO(("~OperaMuminVideoBroadcastBackend() [%p] [%" PRIuPTR "]\n", this, m_window_id));

    deregisterAsOpAppListener();

    if (m_vb_object)
    {
        if (m_backend_mode == BackendMode::MODE_VIDEO_BROADCAST)
        {
            m_object_factory.discardOipfVideoBroadcastObject(m_vb_object, this);
        }
        else if (m_backend_mode == BackendMode::MODE_SUPERVISOR)
        {
            m_object_factory.discardBroadcastSupervisor(m_vb_object, this);
        }
    }

    if (m_component_helper)
    {
        delete m_component_helper;
        m_component_helper = nullptr;
    }

    if (test_backend == this)
    {
        test_backend = nullptr;
    }
}

UVA_STATUS OperaMuminVideoBroadcastBackend::setWindowId(uintptr_t window_id)
{
    m_window_id = window_id;
    TRACE_INFO(("setWindowId() [%p] [%" PRIuPTR "]\n", this, m_window_id));
    return UVA_OK;
}

UVA_STATUS OperaMuminVideoBroadcastBackend::initBackend(const char* origin)
{
    TRACE_INFO(("initBackend():origin:[%s]\n", origin));
    
    UVA_STATUS result = UVA_OK;

#if defined(INCLUDE_OPAPP_SUPPORT)
    const bool existing_url_an_opapp = HbbTvPackageUrl(m_origin.c_str()).valid();
#endif

    m_origin = origin;

#if defined(INCLUDE_OPAPP_SUPPORT)
    if (BackendMode::MODE_VIDEO_BROADCAST == m_backend_mode)
    {
        if (m_vb_object && !existing_url_an_opapp)
        {
            const bool new_url_an_opapp = HbbTvPackageUrl(m_origin.c_str()).valid();
            if (new_url_an_opapp)
            {
                m_vbo_type_converter.convertToAnOpAppVideoBroadcastObject(*m_vb_object);
                registerAsOpAppListener();
            }
        }
    }
#endif

    return result;
}

UVA_STATUS
OperaMuminVideoBroadcastBackend::setBackendMode(BackendMode mode)
{
    bool result = false;
    
    TRACE_INFO(("%s [%" PRIuPTR "] (mode = %d)\n", __func__, m_window_id, mode));

    if (m_vb_object)
    {
        if (m_backend_mode == mode)
        {
            result = true;
        }
        else
        {
            SHOULD_NOT_BE_HERE();

            TRACE_ERROR(("Changing BackendModes is not supported\n"));
        }
    }
    else
    {
        m_backend_mode = mode;

        result = m_pending_channel_mutex.initialise();

        if (m_backend_mode == BackendMode::MODE_VIDEO_BROADCAST)
        {
            TRACE_INFO(("%s(MODE_VIDEO_BROADCAST)\n", __func__));

#ifdef INCLUDE_OPAPP_SUPPORT
            const HbbTvPackageUrl url(m_origin.c_str());
            result = result && m_object_factory.createOipfVideoBroadcastObject(&m_vb_object,
                                                                               this,
                                                                               url.valid());
            if (result && url.valid())
            {
                registerAsOpAppListener();
            }
#else
            result = result && m_object_factory.createOipfVideoBroadcastObject(&m_vb_object,
                                                                               this);
#endif
        }
        else if (m_backend_mode == BackendMode::MODE_SUPERVISOR)
        {
            TRACE_INFO(("%s(MODE_SUPERVISOR)\n", __func__));

            result = result && m_object_factory.createBroadcastSupervisor(&m_vb_object, this);
        }
        else
        {
            SHOULD_NOT_BE_HERE();

            TRACE_ERROR(("Unknown BackendMode\n"));

            result = false;
        }

        if (result)
        {
            if (m_component_helper)
            {
                delete m_component_helper;
                m_component_helper = nullptr;
            }

            m_component_helper = new OperaMuminAVComponentHelper(*m_vb_object, m_client, false);

            if (m_component_helper)
            {
                m_component_helper->updateComponents();
            }
        }
    }

    TRACE_INFO(("%s(): returning %s\n", __func__, result ? "UVA_OK" : "UVA_ERROR"));

    return result ? UVA_OK : UVA_ERROR;
}

UVA_STATUS OperaMuminVideoBroadcastBackend::suspend()
{
    TRACE_INFO(("suspend() \n"));
    if (m_vb_object)
    {
        m_vb_object->release();
    }
    return UVA_OK;
}

UVA_STATUS OperaMuminVideoBroadcastBackend::setVideoWindowAndClip(const Rect& window_rect, const Rect& clip_rect)
{
    //TODO: clip_rect should be handled.
    TRACE_INFO(("setVideoWindow() [%" PRIuPTR "] window (x: %d, y: %d, w: %d, h: %d)\n",
                        m_window_id,
                        window_rect.x, window_rect.y, window_rect.w, window_rect.h));

    if (window_rect.w == 0 || window_rect.h == 0)
    {
        TRACE_WARN(("Ignoring window with zero width or height\n"));
        return UVA_ERROR;
    }

    return applyVideoWindowSettings(window_rect, clip_rect);
}

UVA_STATUS OperaMuminVideoBroadcastBackend::setVisibility(bool visible)
{
    TRACE_ONLY_PARAM(visible);
    TRACE_INFO(("setVisibility(): visible: %d, m_vb_object: %p\n", visible, m_vb_object));

    return UVA_OK;
}

UVA_STATUS OperaMuminVideoBroadcastBackend::getPlayState(UVAVideoBroadcastBackend::PlayState &play_state)
{
    TRACE_VERBOSE(("getPlayState() \n"));
    play_state = UVAVideoBroadcastBackend::PLAYSTATE_UNREALIZED;
    if (m_vb_object)
    {
        const nebula::PlayState state = m_vb_object->playState();
        switch (state)
        {
        case nebula::PlayState::unrealized:
            play_state = UVAVideoBroadcastBackend::PLAYSTATE_UNREALIZED;
            break;
        case nebula::PlayState::connecting:
            play_state = UVAVideoBroadcastBackend::PLAYSTATE_CONNECTING;
            break;
        case nebula::PlayState::presenting:
            play_state = UVAVideoBroadcastBackend::PLAYSTATE_PRESENTING;
            break;
        case nebula::PlayState::stopped:
            play_state = UVAVideoBroadcastBackend::PLAYSTATE_STOPPED;
            break;
        default:
            play_state = UVAVideoBroadcastBackend::PLAYSTATE_UNREALIZED;
            break;
        }
        TRACE_INFO(("getPlayState(): play_state: %d\n", play_state));

        return UVA_OK;
    }
    TRACE_ERROR(("getPlayState(): Cannot get play state\n"));
    return UVA_ERROR;
}

#if defined USING_SDK422 || defined USING_SDK423
UVA_STATUS OperaMuminVideoBroadcastBackend::bindToCurrentChannel(
                        UVAValueStatus* bound_channel_value_status,
                        std::shared_ptr<UVAChannel>& bound_channel_value)
{
    TRACE_INFO(("bindToCurrentChannel() \n"));

    return internalBindToCurrentChannel(bound_channel_value_status, bound_channel_value);
}
#else
UVA_STATUS OperaMuminVideoBroadcastBackend::bindToCurrentChannel()
{
    TRACE_INFO(("bindToCurrentChannel() \n"));

    UVAValueStatus bound_channel_value_status;
    std::shared_ptr<UVAChannel> bound_channel_value;
    return internalBindToCurrentChannel(&bound_channel_value_status, bound_channel_value);
}
#endif

UVA_STATUS OperaMuminVideoBroadcastBackend::internalBindToCurrentChannel(
                        UVAValueStatus* bound_channel_value_status,
                        std::shared_ptr<UVAChannel>& bound_channel_value)
{
    UVA_STATUS retval = UVA_ERROR;

    *bound_channel_value_status = UVA_UNDEFINED_VALUE;

    if (m_vb_object)
    {
        NEBULA_ChannelChangeMethod channel_change_method = NEBULA_BindToCurrent;
        UString ccid = "";

        if (! m_vb_object->selectChannel(channel_change_method, ccid,
                                        nebula_channel_change_normal))
        {
            TRACE_ERROR(("In %s(): selectChannel failed\n", __func__));
            retval = UVA_ERROR;
        }
        else
        {
            FrostMutexLock lock(m_pending_channel_mutex);
            UVAValueStatus value_status = UVA_NULL_VALUE;
            const UVA_STATUS current_channel_status = getCurrentChannel(&value_status,
                                                                        m_viewer_channel);
            if (current_channel_status == UVA_OK)
            {
                *bound_channel_value_status = value_status;
                if (value_status == UVA_VALUE)
                {
                    bound_channel_value = m_viewer_channel;
                }
                else if (value_status == UVA_NULL_VALUE)
                {
                    bound_channel_value = nullptr;
                }
            }

            if ((current_channel_status != UVA_OK) || (value_status != UVA_VALUE))
            {
                TRACE_ERROR(("getCurrentChannel() failed to get viewer channel\n"));
            }

            retval = UVA_OK;
        }
    }
    return retval;
}

UVA_STATUS OperaMuminVideoBroadcastBackend::setChannel(
        UVAValueStatus channel_value_status, const std::shared_ptr<UVAChannel>& channel,
        UVAValueStatus trickplay_value_status, bool trickplay,
        UVAValueStatus content_access_descriptor_url_value_status,
        const std::string& content_access_descriptor_url,
        UVAValueStatus offset_value_status, int offset,
        UVAValueStatus quiet_value_status, int quiet_value)
{
    CABOT_UNUSED_PARAM(channel_value_status);
    CABOT_UNUSED_PARAM(trickplay_value_status);
    CABOT_UNUSED_PARAM(trickplay);
    CABOT_UNUSED_PARAM(content_access_descriptor_url_value_status);
    CABOT_UNUSED_PARAM(content_access_descriptor_url);
    CABOT_UNUSED_PARAM(offset_value_status);
    CABOT_UNUSED_PARAM(offset);
    UVA_STATUS result = UVA_ERROR;

    TRACE_INFO(("%s()\n", __func__));

    if (m_vb_object)
    {
        NEBULA_ChannelChangeMethod channel_change_method = NEBULA_SelectSpecific;

        std::string uva_ccid;
        if (channel)
        {
            channel->getCcid(uva_ccid);
        }
        UString ccid = uva_ccid.c_str();

        // Note that trickplay, contentAccessDescriptorURL, has_offset are
        // ignored since they are not required by HbbTV.
        NEBULA_ChannelChangeQuietValue set_channel_quiet_value = nebula_channel_change_normal;
        if (quiet_value_status == UVA_VALUE)
        {
            set_channel_quiet_value = convertQuietValuetoNebulaType(quiet_value);
        }
        TRACE_INFO(("%s(): ccid: %s, quiet: %d\n",
                    __func__, ccid.charArray(), set_channel_quiet_value));

        {
            FrostMutexLock lock (m_pending_channel_mutex);
            if (set_channel_quiet_value == nebula_channel_change_normal)
            {
                m_pending_ccid.clear();
            }
            else
            {
                m_pending_ccid = ccid;
            }
            m_pending_quiet_value = set_channel_quiet_value;
        }
        result = m_vb_object->selectChannel(channel_change_method,
                            ccid, set_channel_quiet_value) ? UVA_OK : UVA_ERROR;
    }
    else
    {
        TRACE_ERROR(("%s(): Failed\n", __func__));
    }

    return result;
}

UVA_STATUS
OperaMuminVideoBroadcastBackend::getChannelConfig(UVAValueStatus* return_value_status,
                                                  std::shared_ptr<UVAChannelConfig>& return_value)
{
    if (m_vb_object)
    {
        *return_value_status = UVA_VALUE;
        return_value = std::shared_ptr<UVAChannelConfig>(new OperaOipfChannelConfig());
        return UVA_OK;
    }
    *return_value_status = UVA_UNDEFINED_VALUE;
    return UVA_ERROR;
}

UVA_STATUS OperaMuminVideoBroadcastBackend::previousChannel()
{
    TRACE_INFO(("previousChannel() \n"));
    if (m_vb_object)
    {
        NEBULA_ChannelChangeMethod channel_change_method = NEBULA_SelectPrevious;
        UString ccid = "";

        m_vb_object->selectChannel(channel_change_method, ccid, nebula_channel_change_normal);
        return UVA_OK;
    }
    TRACE_ERROR(("previousChannel() Failed\n"));
    return UVA_ERROR;
}

UVA_STATUS OperaMuminVideoBroadcastBackend::nextChannel()
{
    TRACE_INFO(("nextChannel() \n"));
    if (m_vb_object)
    {
        NEBULA_ChannelChangeMethod channel_change_method = NEBULA_SelectNext;
        UString ccid = "";

        m_vb_object->selectChannel(channel_change_method, ccid, nebula_channel_change_normal);
        return UVA_OK;
    }
    TRACE_ERROR(("nextChannel(): Failed\n"));
    return UVA_ERROR;
}

UVA_STATUS OperaMuminVideoBroadcastBackend::setFullScreen(bool fullscreen)
{
    TRACE_INFO(("VB setFullScreen(): fullscreen: %d\n", fullscreen));
    if (m_vb_object)
    {
        //video should be switched to fullscreen mode.
        m_vb_object->setFullScreen(fullscreen);
        return UVA_OK;
    }
    TRACE_ERROR(("setFullScreen(): Failed\n"));
    return UVA_ERROR;
}

UVA_STATUS OperaMuminVideoBroadcastBackend::setVolume(int volume)
{
    TRACE_INFO(("setVolume(): volume: %d\n", volume));

    return NEBULA_SetVolumeLevel(volume, frost_true) == frost_true ? UVA_OK : UVA_ERROR;
}

UVA_STATUS OperaMuminVideoBroadcastBackend::stop()
{
    TRACE_INFO(("stop() \n"));
    if (m_vb_object)
    {
        if (m_vb_object->stop())
        {
            return UVA_OK;
        }
    }
    return UVA_ERROR;
}

UVA_STATUS OperaMuminVideoBroadcastBackend::selectComponent(UVABackend::AVComponent* component)
{
    // Documented as not supported
    TRACE_INFO(("selectComponent() component: %p\n", component));
    UVA_STATUS ret = UVA_ERROR;
    if (m_component_helper)
    {
        ret = m_component_helper->selectComponent(component) ? UVA_OK : UVA_ERROR;
    }
    return ret;
}

UVA_STATUS OperaMuminVideoBroadcastBackend::unselectComponent(UVABackend::AVComponent* component)
{
    TRACE_INFO(("unselectComponent() component: %p\n", component));
    UVA_STATUS ret = UVA_ERROR;
    if (m_component_helper)
    {
        ret = m_component_helper->unselectComponent(component) ? UVA_OK : UVA_ERROR;
    }
    return ret;
}

UVA_STATUS OperaMuminVideoBroadcastBackend::getPlaybackOffset(double &offset)
{
    TRACE_INFO(("getPlaybackOffset() \n"));
    offset = m_vb_object->getPlaybackOffset();
    return UVA_OK;
}

UVA_STATUS OperaMuminVideoBroadcastBackend::getMaxOffset(double &offset)
{
    TRACE_INFO(("getMaxOffset() \n"));
    offset = m_vb_object->getMaxOffset();
    return UVA_OK;
}

UVA_STATUS OperaMuminVideoBroadcastBackend::getRecordingState(RecordingState &state)
{
    TRACE_INFO(("getRecordingState()\n"));
    state = (RecordingState)m_vb_object->getRecordingState();
    return UVA_OK;
}

UVA_STATUS OperaMuminVideoBroadcastBackend::getPlaybackSpeed(double &speed)
{
    TRACE_INFO(("getPlaybackSpeed() \n"));
    speed = m_vb_object->getPlaySpeed();
    return UVA_OK;
}

UVA_STATUS OperaMuminVideoBroadcastBackend::getPlaybackSpeeds(double*& speeds, size_t& num_speeds)
{
    TRACE_INFO(("getPlaybackSpeeds() \n"));

    num_speeds = 13;
    speeds = new double[num_speeds];
    speeds[0] = -32.0;
    speeds[1] = -16.0;
    speeds[2] = -8.0;
    speeds[3] = -4.0;
    speeds[4] = -2.0;
    speeds[5] = -1.0;
    speeds[6] = 0.0;
    speeds[7] = 1.0;
    speeds[8] = 2.0;
    speeds[9] = 4.0;
    speeds[10] = 8.0;
    speeds[11] = 16.0;
    speeds[12] = 32.0;

    return UVA_OK;
}

UVA_STATUS OperaMuminVideoBroadcastBackend::recordNow(double duration, char* &identifier)
{
    TRACE_INFO(("recordNow() \n"));
    char* id = 0;
    UVA_STATUS result = m_vb_object->recordNow((int)duration, &id) ? UVA_OK: UVA_ERROR;
    // Recording_id is not known yet, it will come within RecordingEvent (generateRecordingEvent)
    if (id)
    {
        const unsigned int id_len = strlen(id);
        identifier = new char[1 + id_len];
        if (identifier)
        {
            strcpy(identifier, id);
        }
        free(id);
    }
    return result;
}

UVA_STATUS OperaMuminVideoBroadcastBackend::stopRecording()
{
    TRACE_INFO(("stopRecording() \n"));
    return m_vb_object->stopRecording() ? UVA_OK: UVA_ERROR;
}

UVA_STATUS OperaMuminVideoBroadcastBackend::pause()
{
    TRACE_INFO(("pause() \n"));
    return m_vb_object->pause() ? UVA_OK: UVA_ERROR;
}

UVA_STATUS OperaMuminVideoBroadcastBackend::resume()
{
    TRACE_INFO(("resume()\n"));
    return m_vb_object->resume() ? UVA_OK: UVA_ERROR;
}

UVA_STATUS OperaMuminVideoBroadcastBackend::setPlaybackSpeed(double speed)
{
    TRACE_INFO(("setPlaybackSpeed() \n"));
    return m_vb_object->setSpeed(static_cast<int>(speed)) ? UVA_OK: UVA_ERROR;
}

UVA_STATUS OperaMuminVideoBroadcastBackend::setPosition(double offset, PositionReference reference)
{
    TRACE_INFO(("setPosition() \n"));
    return m_vb_object->seek(static_cast<int>(offset), reference) ? UVA_OK: UVA_ERROR;
}

#ifdef USING_SDK423
UVA_STATUS
OperaMuminVideoBroadcastBackend::getTimeShiftMode(int& mode)
{
    // Not currently supported.
    // Stubbed since an implementation is required for SDK 4.23 to avoid
    // a FATAL error when accessed via debugger (see LIMA2-128).
    // Older SDKs return 'undefined' when an implementation is not provided.
    mode = 0; // 'Timeshift is turned off.'
    return UVA_OK;
}

UVA_STATUS
OperaMuminVideoBroadcastBackend::getCurrentTimeShiftMode(int& mode)
{
    // Not currently supported.
    // Stubbed since an implementation is required for SDK 4.23 to avoid
    // a FATAL error when accessed via debugger (see LIMA2-128).
    // Older SDKs return 'undefined' when an implementation is not provided.
    mode = 0; // 'No timeshift.'
    return UVA_OK;
}
#endif

UVA_STATUS
OperaMuminVideoBroadcastBackend::enableTimeline(const std::string& timeline_selector)
{
    TRACE_INFO(("enableTimeline() %s\n", timeline_selector.c_str()));
    UVA_STATUS res = selectTimeline(timeline_selector, true);

    // The following part is added to fix failures for Ligada tests org.hbbtv_APP2AV0130,
    // org.hbbtv_SYNCAPI2001, org.hbbtv_MDEVSYNC101. The tests were failing because a call
    // to selectTimeline() fails and JScript side generates MediaSynchroniser error 15
    // because of this. If the first call to selectTimeline() fails, we will try a couple of
    // times to conclude that we cannot select the specified timeline.
    int count = 0;
    while(res != UVA_OK)
    {
        TRACE_INFO(("enableTimeline() %s, res: %d, count: %d\n", timeline_selector.c_str(), res, count));
        FROST_Sleep(500);
        res = selectTimeline(timeline_selector, true);
        if (count++ >= 6)
        {
            break;
        }
    }
    return res;
}

UVA_STATUS
OperaMuminVideoBroadcastBackend::disableTimeline(const std::string& timeline_selector)
{
    TRACE_INFO(("disableTimeline() %s\n", timeline_selector.c_str()));
    return selectTimeline(timeline_selector, false);
}

UVA_STATUS
OperaMuminVideoBroadcastBackend::selectTimeline(const std::string& timeline_selector,
                                                bool enable)
{
    std::uint8_t component_tag = 0;
    std::uint8_t timeline_id = 0;

    UVA_STATUS status = UVA_ERROR;
    switch (NEBULA_extractTimelineType(timeline_selector.c_str(), &component_tag, &timeline_id))
    {
        case NEBULA_PtsTimelineType:
            status = UVA_OK;
            break;

        case NEBULA_TemiTimelineType:
            // Call (indirectly) eclipse function to monitor TEMI.
            if (NEBULA_SelectTemiTimeline(component_tag, timeline_id,
                                          enable ? frost_true : frost_false))
            {
                status = UVA_OK;
            }
            else
            {
                TRACE_ERROR(("Failed to %s TEMI timeline for component tag %u timeline id %u\n",
                                 enable ? "enable" : "disable",
                                 component_tag, timeline_id));
            }
            break;

        default:
            TRACE_ERROR(("Failed to extract PTS or TEMI timeline from timeline selector\n"));
            break;
    }
    return status;
}

UVATime
OperaMuminVideoBroadcastBackend::getPosition(const std::string& timeline_selector)
{
    TRACE_INFO(("getPosition() %s\n", timeline_selector.c_str()));

    std::uint8_t component_tag = 0;
    std::uint8_t timeline_id = 0;
    switch (NEBULA_extractTimelineType(timeline_selector.c_str(), &component_tag, &timeline_id))
    {
        case NEBULA_PtsTimelineType:
        {
            NEBULA_TemiTimestamp result {};
            if (NEBULA_GetPtsTimestamp(&result) != frost_true)
            {
                TRACE_ERROR(("Failed to get PTS timeline\n"));
                break;
            }

            TRACE_VERBOSE(("PTS Timestamp: PTS %" PRIu64 "\n", result.current_pts));
            const double position = static_cast<double>(result.current_pts) / static_cast<double>(pcr_ticks_per_sec);
            const double rate = 1.0;

            TRACE_INFO(("PTS Timestamp: position %f, rate %f\n", position, rate));
            return UVATime(position, rate);
        }

        case NEBULA_TemiTimelineType:
        {
            // Call (indirectly) eclipse function to get timestamp details.
            NEBULA_TemiTimestamp result {};
            if (NEBULA_GetTemiTimestamp(component_tag, timeline_id, &result) != frost_true)
            {
                TRACE_ERROR(("Failed to get TEMI timestamp for component tag %u timeline id %u\n",
                                 component_tag, timeline_id));
                break;
            }

            // Sanity check
            if (0 == result.timescale)
            {
                TRACE_WARN(("Ignored TEMI timestamp for component tag %u timeline id %u:"
                            "zero timescale\n",
                                component_tag, timeline_id));
                break;
            }

            // Convert NEBULA_TemiTimestamp to position.
            // Rec. ITU-T H.222.0 (03/2017) section U.3.7
            // (current_PTS - timestamp_PTS)/90000 + media_timestamp/timescale
            double offset = 0.0;
            if (result.current_pts < result.timeline_pts)
            {
                offset -= static_cast<double>(result.timeline_pts - result.current_pts)
                                                / pcr_ticks_per_sec;
            }
            else
            {
                offset = static_cast<double>(result.current_pts - result.timeline_pts)
                                                / pcr_ticks_per_sec;
            }
            const double position = offset + static_cast<double>(result.media_timestamp)
                                                                    / result.timescale;

            const double original_time_sec = static_cast<double>(result.current_pts) / pcr_ticks_per_sec;

            TRACE_VERBOSE(("TEMI Timestamp: component tag %u, timeline id %u, timescale %u"
                           ", media_timestamp %" PRIu64 ", PTS %" PRIu64
                           ", current PTS %" PRIu64 ", system usec %" PRIi64 "\n",
                               component_tag, timeline_id, result.timescale,
                               result.media_timestamp, result.timeline_pts, 
                               result.current_pts, result.system_usec));

            TRACE_INFO(("TEMI Timestamp: component tag %u, timeline id %u"
                        ", position %f, system usec %" PRIi64 
                        " (PTS offset %f)\n",
                            component_tag, timeline_id, position, result.system_usec, offset));

            const double rate = 1.0;
            UVATime time(position, rate);
            //time.SetTimestamp(result.system_usec);
            time.SetOriginalTimeSec(original_time_sec);
            return time;
        }

        default:
            TRACE_WARN(("getPosition() Unrecognised selector %s\n", timeline_selector.c_str()));
            break;
    }

    // Failed to obtain position
    const UVATime failure_time(-1.0, -1.0);
    return failure_time;
}

UVATimelineDetails
OperaMuminVideoBroadcastBackend::getTimelineDetails(const std::string& timeline_selector)
{
    TRACE_INFO(("getTimelineDetails() %s\n", timeline_selector.c_str()));
    UVATimelineDetails details;
    NEBULA_TemiTimestamp result {};

    std::uint8_t component_tag = 0;
    std::uint8_t timeline_id = 0;
    switch (NEBULA_extractTimelineType(timeline_selector.c_str(), &component_tag, &timeline_id))
    {
        case NEBULA_PtsTimelineType:
            details.has_units_per_second = true;
            details.units_per_second = pcr_ticks_per_sec;
            details.has_units_per_tick = true;
            details.units_per_tick = 1;
            break;

        case NEBULA_TemiTimelineType:
            // Call (indirectly) eclipse function to get timeline details TEMI.
            if (NEBULA_GetTemiTimestamp(component_tag, timeline_id, &result) == frost_true)
            {
                TRACE_VERBOSE(("TEMI Timeline: component tag %u, timeline id %u, timescale %u\n",
                               component_tag, timeline_id, result.timescale));
                details.has_units_per_second = true;
                details.units_per_second = result.timescale;
                details.has_units_per_tick = true;
                details.units_per_tick = 1;
            }
            else
            {
                TRACE_ERROR(("Failed to get TEMI timeline for component tag %u timeline id %u\n",
                             component_tag, timeline_id));
            }
            break;

        default:
            TRACE_ERROR(("Failed to extract PTS or TEMI timeline from timeline selector\n"));
            break;
    }

    return details;
}

bool
OperaMuminVideoBroadcastBackend::generateContentIdChanged() const
{
    TRACE_INFO(("generateContentIdChanged()\n"));

    return m_client &&
           (UVA_OK == m_client->handleEvent(new ContentIdChangedEvent));
}

bool
OperaMuminVideoBroadcastBackend::generateMrsUrlChanged() const
{
    TRACE_INFO(("generateMrsUrlChanged()\n"));

    return m_client &&
           (UVA_OK == m_client->handleEvent(new MrsUrlChangedEvent));
}

UVA_STATUS
OperaMuminVideoBroadcastBackend::getContentId(std::string& content_id,
                                    UVAContentIdStatus& content_id_status)
{
    TRACE_INFO(("getContentId()\n"));

    content_id_status = CONTENT_ID_STATUS_UNDEFINED;
    if (!m_vb_object)
    {
        return UVA_NOT_INITIALIZED;
    }

    cabot::String content;
    bool partial = true;
    if (!m_vb_object->getContentId(content, partial))
    {
        return UVA_ERROR;
    }
    content_id_status = partial ? CONTENT_ID_STATUS_PARTIAL : CONTENT_ID_STATUS_FINAL;
    content_id.assign(content.charArray(), content.length());

    return UVA_OK;
}

UVA_STATUS OperaMuminVideoBroadcastBackend::getMrsUrl(std::string& mrs_url)
{
    TRACE_INFO(("getMrsUrl()\n"));

    if (!m_vb_object)
    {
        return UVA_NOT_INITIALIZED;
    }

    cabot::String vb_mrs_url;
    if (!m_vb_object->getMrsUrl(vb_mrs_url))
    {
        return UVA_ERROR;
    }
    mrs_url.assign(vb_mrs_url.charArray(), vb_mrs_url.length());

    return UVA_OK;
}

UVA_STATUS OperaMuminVideoBroadcastBackend::stopTimeShift()
{
    TRACE_INFO(("stopTimeShift() \n"));
    return m_vb_object->stopTimeshift() ? UVA_OK: UVA_ERROR;
}

UVA_STATUS
OperaMuminVideoBroadcastBackend::getCurrentChannel(UVAValueStatus* value_status,
        std::shared_ptr<UVAChannel>& current_channel_value)
{
    bool bound_to_channel = false;
    UVA_STATUS result = UVA_ERROR;

    if (m_backend_mode == BackendMode::MODE_VIDEO_BROADCAST)
    {
        if (m_vb_object)
        {
            // See ETSI TS 102 796 V1.5.1 (2018-09) A.2.4.5
            // currentChannel property value should only be returned if vbo is bound
            // in connecting, presenting or stopped states, otherwise return null.
            const nebula::PlayState play_state = m_vb_object->playState();
            bound_to_channel = 
                play_state == nebula::PlayState::connecting ||
                play_state == nebula::PlayState::presenting ||
                play_state == nebula::PlayState::stopped;
        }
    }
    else if (m_backend_mode == BackendMode::MODE_SUPERVISOR)
    {
        // Basic BroadcastSupervisor support - Workaround to allow the underlying call to 
        // getChannel() to work for a BroadcastSupervisor. Handles the case where the video may
        // be broadcast by the terminal and not a video/broadcast object within an OpApp or 
        // HbbTV App.
        bound_to_channel = true;
    }

    UString ccid;
    
    if (bound_to_channel)
    {
        // Ensure we return null if there is no current channel
        // (e.g. no services are installed when BroadcastSupervisor is used).
        bound_to_channel = m_vb_object->getCurrentChannel(ccid);
    }
    if (bound_to_channel)
    {
        NEBULA_Channel channel_info;

        if (m_vb_object->getChannelInfoByID(ccid, channel_info))
        {
            current_channel_value.reset(new OperaOipfChannel(channel_info));
            TRACE_INFO(("Current channel reported to backend '%s', name '%s'\n", ccid.c_str(), channel_info.name));
            *value_status = UVA_VALUE;
            result = UVA_OK;
        }
        else
        {
            TRACE_ERROR(("getCurrentChannel(): Failed\n"));
            *value_status = UVA_NULL_VALUE;
            result =  UVA_ERROR;
        }
    }
    else
    {
        TRACE_INFO(("Not bound, currentChannel returns null\n"));
        *value_status = UVA_NULL_VALUE;
        result = UVA_OK;
    }
    return result;
}

#ifdef USING_SDK423
UVA_STATUS
OperaMuminVideoBroadcastBackend::getCurrentServiceInstance(
    UVAValueStatus* current_service_instance_value_status,
    std::shared_ptr<UVAChannel>& current_service_instance_value)
{
    TRACE_INFO(("%s()\n", __func__));
    
    // Stubbed since an implementation is required for SDK 4.23 to avoid
    // a FATAL error when accessed via debugger (see LIMA2-128).
    // TODO: To be implemented by STEALTH-1794.
    CABOT_UNUSED_PARAM(current_service_instance_value);
    // Should return UVA_NOT_SUPPORTED for now but this leads to an error as mentioned above.
    UVA_STATUS result = UVA_OK;
    // Cannot set to UVA_UNDEFINED_VALUE as it leads to an error as mentioned above.
    *current_service_instance_value_status = UVA_NULL_VALUE;
    return result;
}
#endif

bool
OperaMuminVideoBroadcastBackend::generateChannelChangeError(UString& ccid,
                                        NEBULA_ChannelChangeResult error)
{
    std::shared_ptr<UVAChannel> channel;

    TRACE_INFO(("generateChannelChangeError(): ccid: %s\n", ccid.charArray()));
    if (m_vb_object )
    {
        NEBULA_Channel channel_info;
        if (m_vb_object->getChannelInfoByID(ccid, channel_info))
        {
            channel.reset(new OperaOipfChannel(channel_info));
        }
        else
        {
            TRACE_ERROR(("Could not get channel info.\n"));
        }

        TRACE_INFO(("generateChannelChangeError(): ccid: %s, error_state: %d\n",
                ccid.charArray(), error));

        m_client->handleEvent(
                new ChannelChangeErrorEvent(channel, (UVAVideoBroadcastBackend::ChannelError)error));
    }

    FrostMutexLock lock (m_pending_channel_mutex);
    if (!m_pending_ccid.empty() &&
         m_pending_ccid == ccid)
    {
            m_pending_ccid.clear();
            m_pending_quiet_value = nebula_channel_change_normal;
    }

    return true;
}

bool
OperaMuminVideoBroadcastBackend::generateChannelChangeSucceeded(UString& ccid)
{
    TRACE_INFO(("generateChannelChangeSucceeded(): ccid: %s\n", ccid.charArray()));

    if(m_component_helper)
    {
        m_component_helper->updateComponents();
    }

    if (m_vb_object)
    {
        if (ccid.length() == 0)
        {
            TRACE_INFO(("generateChannelChangeSucceeded(%d): channel: NULL\n",__LINE__));
            std::shared_ptr<UVAChannel> channel;
#ifndef INCLUDE_OPAPP_SUPPORT
            m_client->handleEvent(new ChannelChangeSucceededEvent(channel));
#else
            m_client->handleEvent(new ChannelChangeSucceededEvent(
                            UVA_VALUE, channel,
                            UVA_VALUE, channel,
                            UVA_VALUE, convertNebulaQuietValue(nebula_channel_change_normal)));
#endif      
        }
        else
        {
            NEBULA_Channel channel_info;
            if (m_vb_object->getChannelInfoByID(ccid, channel_info))
            {
                TRACE_INFO(("generateChannelChangeSucceeded(): ccid: %s\n", ccid.charArray()));
                std::shared_ptr<UVAChannel> channel(new OperaOipfChannel(channel_info));
#ifndef INCLUDE_OPAPP_SUPPORT
                m_client->handleEvent(new ChannelChangeSucceededEvent(channel));
#else
                FrostMutexLock lock (m_pending_channel_mutex);
                NEBULA_ChannelChangeQuietValue quiet_response = nebula_channel_change_normal;
                if (m_pending_ccid == ccid)
                {
                    quiet_response = m_pending_quiet_value;
                    m_pending_ccid.clear();
                    m_pending_quiet_value = nebula_channel_change_normal;
                }
                else if (!m_pending_ccid.empty())
                {
                    TRACE_WARN(("generateChannelChangeSucceeded() expected %s, actual %s\n",
                                       m_pending_ccid.c_str(), ccid.c_str()));
                }

                if (quiet_response != nebula_quiet_channel_change)
                {
                    m_viewer_channel = channel;
                }

                TRACE_INFO(("generateChannelChangeSucceeded(): '%s', viewer '%s', quiet %d\n",
                                    ccid.c_str(), 
                                    getCcidFromChannel(m_viewer_channel).c_str(), quiet_response));

                m_client->handleEvent(new ChannelChangeSucceededEvent(
                                UVA_VALUE, channel, 
                                UVA_VALUE, m_viewer_channel,
                                UVA_VALUE, convertNebulaQuietValue(quiet_response)));
#endif
            }
            else
            {
                TRACE_ERROR(("Could not get channel info.\n"));
            }
        }
    }

    return true;
}

bool
OperaMuminVideoBroadcastBackend::generatePlayStateChanged(nebula::PlayState state,
                                        NEBULA_ChannelChangeResult error)
{
    if (! m_client)
    {
        return false;
    }

    TRACE_INFO(("generatePlayStateChanged(): state: '%s', error %d\n", 
                nebula::toString(state).c_str(), error));

    UVAVideoBroadcastBackend::PlayState play_state = UVAVideoBroadcastBackend::PLAYSTATE_UNREALIZED;
    UVAValueStatus error_state_status = UVA_UNDEFINED_VALUE;

    if (state == nebula::PlayState::stopped)
    {
        play_state = UVAVideoBroadcastBackend::PLAYSTATE_STOPPED;
    }
    else if (state == nebula::PlayState::connecting)
    {
        play_state = UVAVideoBroadcastBackend::PLAYSTATE_CONNECTING;
        if (m_component_helper)
        {
            m_component_helper->registerComponentCallbacks();
        }
    }
    else if (state == nebula::PlayState::presenting)
    {
        play_state = UVAVideoBroadcastBackend::PLAYSTATE_PRESENTING;
    }
    else if (state == nebula::PlayState::unrealized)
    {
        play_state = UVAVideoBroadcastBackend::PLAYSTATE_UNREALIZED;
        if (m_component_helper)
        {
            m_component_helper->deregisterComponentCallbacks();
        }
    }
    else
    {
        return true;
    }

    if ((error >= NEBULA_ChannelNotSupportedByTuner) && (error <= NEBULA_UnknownError))
    {
        error_state_status = UVA_VALUE;
    }

    bool success = true;
    if (m_client->handleEvent(new BroadcastPlaystateChangeEvent
            (play_state, error_state_status, (UVAVideoBroadcastBackend::ChannelError)error)) != UVA_OK)
    {
        success = false;
    }

    return success;
}

bool
OperaMuminVideoBroadcastBackend::generateProgrammesChanged()
{
    if(m_component_helper)
    {
        m_component_helper->updateComponents();
    }
    if (! m_client)
    {
        return false;
    }
    if (UVA_OK != m_client->handleEvent(new ProgrammesChangedEvent))
    {
        return false;
    }

    return true;
}

bool
OperaMuminVideoBroadcastBackend::generateStreamEvent(
                           void* listener,
                           const cabot::String& url,
                           const cabot::String& name,
                           const cabot::String& data,
                           const UString& text,
                           StreamEventStatus status)
{
    CABOT_UNUSED_PARAM(listener);
    CABOT_UNUSED_PARAM(text);
    TRACE_ONLY_PARAM(url);
    TRACE_ONLY_PARAM(name);
    TRACE_ONLY_PARAM(data);
    TRACE_ONLY_PARAM(status);
    TRACE_INFO(("generateStreamEvent() URL(%s) event(%s) data(%s) status(%d)\n",
                url.charArray(), name.charArray(), data.charArray(), status));

    //TODO: How do we set stream events with SDK 4?
    TRACE_ERROR(("generateStreamEvent, could not find requested event callback !\n"));

    return false;
}

bool
OperaMuminVideoBroadcastBackend::generateFullScreenChanged()
{
    if (! m_client)
    {
        return false;
    }
    // The comments in mumin_Event.h says: You should not be mistaken to use this event in response to
    // MuminMediaBackend::setVideoWindow() call. This event should be fired when
    // native video size changes or becomes known!
#if 0
    if (UVA_OK != m_client->handleEvent(new VideoResizedEvent(browser_max_video_width, browser_max_video_height, 1.0f)))
    {
        return false;
    }
#endif
    return true;
}

void
OperaMuminVideoBroadcastBackend::generateComponentActivenessChangedEvent(const NEBULA_AVComponent *component_data, bool active)
{
    if(m_component_helper)
    {
        TRACE_INFO(("generateComponentActivenessChangedEvent - active=%d\n", 
                    static_cast<int>(active)));

        if (component_data)
        {
            m_component_helper->updateComponentActiveness(component_data,active);
        }
        else
        {
            m_component_helper->updateComponents();
        }
    }
}

bool
OperaMuminVideoBroadcastBackend::generateRecordingEvent(int rec_state, int rec_error, UString rec_id)
{
    if (m_client)
    {
        UVAValueStatus error_value_status = (rec_error == -1) ? UVA_UNDEFINED_VALUE : UVA_VALUE;
        if(UVA_OK == m_client->handleEvent(
                                new RecordingEvent((RecordingState)rec_state, 
                                                            error_value_status, 
                                                            (RecordingEvent::e_error)rec_error, 
                                                            rec_id.isEmpty() ? UVA_NULL_VALUE : UVA_VALUE,
                                                            (char*)rec_id.charArray())))
        {
            return true;
        }
    }
    return false;
}

bool
OperaMuminVideoBroadcastBackend::generatePlaySpeedChangedEvent(int speed)
{
    TRACE_INFO(("generatePlaySpeedChangedEvent (speed = %d)\n", speed));

    if (m_client)
    {
        if (m_client->handleEvent(new PlaySpeedChangedEvent(speed)) == UVA_OK)
        {
            return true;
        }
    }
    return false;
}

void
OperaMuminVideoBroadcastBackend::generateComponentsChangedEvent()
{
    if (m_component_helper)
    {
        m_component_helper->updateComponents();
    }
}

bool
OperaMuminVideoBroadcastBackend::generateDrmRightsErrorEvent(
        int error_state, const std::string& content_id,
        const std::string& drm_system_id, const std::string& rights_issuer_url)
{
    TRACE_INFO(("generateDrmRightsErrorEvent "
                    "err: %d, content_id: '%s', drm_sys_id: '%s', right_url: '%s'\n",
                    error_state, content_id.c_str(), drm_system_id.c_str(), rights_issuer_url.c_str()));
    if (m_client)
    {
        const UVAValueStatus content_id_status =
                content_id.empty() ? UVA_NULL_VALUE : UVA_VALUE;
        const UVAValueStatus rights_issuer_url_status =
                rights_issuer_url.empty() ? UVA_NULL_VALUE : UVA_VALUE;

        if (m_client->handleEvent(new DRMRightsErrorEvent
            (error_state,
             content_id_status, content_id_status == UVA_VALUE ?
                     content_id.c_str() : nullptr,
             drm_system_id.c_str(),
             rights_issuer_url_status, rights_issuer_url_status == UVA_VALUE ?
                     rights_issuer_url.c_str() : nullptr)) == UVA_OK)
        {
            return true;
        }
    }
    return false;
}

bool
OperaMuminVideoBroadcastBackend::generateParentalRatingChangeEvent(
        const std::string& pr_name, const std::string& pr_scheme,
        int pr_value, const std::string& pr_region,
        const std::string& content_id, const std::string& drm_system_id,
        bool blocked)
{
    TRACE_INFO(("generateParentalRatingChangeEvent "
                 "name: '%s', scheme: '%s', value: %d, region '%s', "
                 "content_id '%s', drm_sys_id: '%s', blocking: %sblocked\n",
                    pr_name.c_str(), pr_scheme.c_str(), pr_value, pr_region.c_str(),
                    content_id.c_str(), drm_system_id.c_str(), blocked ? "" : "not "));
    bool result = false;

    // Create a parental rating collection holding a single rating
    std::shared_ptr<UVAParentalRating>
        rating(new OperaOipfParentalRating(pr_name, pr_scheme, pr_region, pr_value));

    // Wrap rating in a Parental Rating Collection
    std::shared_ptr<OperaOipfParentalRatingCollection>
        ratings(new OperaOipfParentalRatingCollection(rating));

    if (m_client && ratings.get() != nullptr && rating.get() != nullptr)
    {
        const UVAValueStatus content_id_status =
                content_id.empty() ? UVA_NULL_VALUE : UVA_VALUE;
        const UVAValueStatus drm_system_id_status =
                drm_system_id.empty() ? UVA_NULL_VALUE : UVA_VALUE;

        if (m_client->handleEvent(
                new ParentalRatingChangeEvent
                        (content_id_status,
                         content_id_status == UVA_VALUE ? content_id.c_str() : nullptr,
                         ratings,
                         drm_system_id_status,
                         drm_system_id_status == UVA_VALUE ? drm_system_id.c_str() : nullptr,
                         blocked)) == UVA_OK)
        {
            result = true;
        }
    }
    return result;
}

bool
OperaMuminVideoBroadcastBackend::generateParentalRatingErrorEvent(
        const std::string& pr_name, const std::string& pr_scheme,
        int pr_value, const std::string& pr_region,
        const std::string& content_id, const std::string& drm_system_id)
{
    TRACE_INFO(("generateParentalRatingErrorEvent "
                "name: '%s', scheme: '%s', value: %d, region '%s', "
                "content_id '%s', drm_sys_id: '%s'\n",
                   pr_name.c_str(), pr_scheme.c_str(), pr_value, pr_region.c_str(),
                   content_id.c_str(), drm_system_id.c_str()));
    bool result = false;

    // Create a parental rating collection holding a single 'unsupported' rating
    std::shared_ptr<UVAParentalRating>
        rating(new OperaOipfParentalRating(pr_region, pr_value));

    // Wrap rating in a Parental Rating Collection
    std::shared_ptr<OperaOipfParentalRatingCollection>
        ratings(new OperaOipfParentalRatingCollection(rating));

    if (m_client && ratings.get() != nullptr && rating.get() != nullptr)
    {
        const UVAValueStatus content_id_status =
                content_id.empty() ? UVA_NULL_VALUE : UVA_VALUE;
        const UVAValueStatus drm_system_id_status =
                drm_system_id.empty() ? UVA_NULL_VALUE : UVA_VALUE;

        if (m_client->handleEvent(
                new ParentalRatingErrorEvent
                        (content_id_status,
                         content_id_status == UVA_VALUE ? content_id.c_str() : nullptr,
                         ratings,
                         drm_system_id_status,
                         drm_system_id_status == UVA_VALUE ? drm_system_id.c_str() : nullptr)) == UVA_OK)
        {
            result = true;
        }
    }
    return result;
}

bool
OperaMuminVideoBroadcastBackend::generateEvent(
        int handle,
        const AnyVbObjectEventGenerator::Event event)
{
    TRACE_INFO(("generateEvent() %08x\n", handle));

    bool ok = false;
    switch (event)
    {
    case AnyVbObjectEventGenerator::event_content_id:
        ok = generateContentIdChanged();
        break;

    case AnyVbObjectEventGenerator::event_mrs_url:
        ok = generateMrsUrlChanged();
        break;

    default:
        break;
    }
    return ok;
}

bool
OperaMuminVideoBroadcastBackend::generatePlayPositionChangedEvent(int position)
{
    TRACE_INFO(("generatePlayPositionChangedEvent (position = %d)\n", position));

    if (m_client)
    {
        if (m_client->handleEvent(
                new BroadcastPlayPositionChangedEvent(UVA_VALUE, position)) == UVA_OK)
        {
            return true;
        }
    }
    return false;
}

UVA_STATUS
OperaMuminVideoBroadcastBackend::getProgrammes(std::shared_ptr<UVAProgrammeCollection>& programmes_value)
{
    TRACE_VERBOSE(("%s(): Entering\n", __func__));

    if (!m_vb_object)
    {
        TRACE_INFO(("%s(): no vb object\n", __func__));
        return UVA_ERROR;
    }
    TRACE_VERBOSE(("%s(): getting num of programmes\n", __func__));
    int num_programs = m_vb_object->numPrograms();
    TRACE_VERBOSE(("%s(): num_programs: %d\n", __func__, num_programs));

    if (num_programs < 0)
    {
        return UVA_ERROR;
    }

    if (num_programs == 0)
    {
        return UVA_OK;
    }

    for (int i = 0; i < num_programs; ++i)
    {
        UString program_id;
        bool program_info_ok = false;

        program_info_ok = m_vb_object->getProgramIDByIndex(i, program_id);
        if (!program_info_ok)
        {
            continue;
        }

        NEBULA_Program program;
        program_info_ok = m_vb_object->getProgramInfoByID(program_id,
                                                          program);

        if (!program_info_ok)
        {
            continue;
        }

        programmes_value->pushBack(std::shared_ptr<UVAProgramme>(new OperaOipfProgramme(program)));
    }

    return UVA_OK;
}

UVA_STATUS
OperaMuminVideoBroadcastBackend::createChannelObject(int idType,
                                                const std::string & dsd,
                                                int sid,
                                                UVAValueStatus* return_value_status,
                                                std::shared_ptr<UVAChannel>& return_value)
{
    CABOT_UNUSED_PARAM(idType);
    TRACE_INFO(("createChannelObject3() dsd: %s, sid: %d\n", dsd.c_str(), sid));

    bool success = false;
    UString ccid;

    // The DSD is assumed to be UTF-8 (though it is not documented in the SDK),
    // so it must be converted to a raw descriptor before it can be used.
    std::string raw_dsd;
    bool const dsd_decoded = decodeDsd(dsd, raw_dsd);
    
    std::shared_ptr<UVAChannel> channel;
    if (dsd_decoded &&
        m_vb_object->createChannelByDescriptor(
                        reinterpret_cast<unsigned char const *>(raw_dsd.data()),
                        raw_dsd.size(),
                        sid, ccid))
    {
        NEBULA_Channel channel_info;
        if (m_vb_object->getChannelInfoByID(ccid, channel_info))
        {
            channel.reset(new OperaOipfChannel(channel_info));
            success = true;
        }
        else
        {
            TRACE_ERROR(("Could not get channel info.\n"));
        }
    }
    else
    {
        TRACE_ERROR(("Could not create channel.\n"));
    }

    *return_value_status = success ? UVA_VALUE : UVA_NULL_VALUE;
    return_value = channel;
    return UVA_OK;
}

namespace
{

bool
mapIdTypeToNetworkType(int id_type, NEBULA_NetworkType & network_type)
{
    bool success = true;
    switch(id_type)
    {
    case UVAChannel::ID_ANALOG:
        network_type = NEBULA_NetworkTypeAnalog;
        break;
    case UVAChannel::ID_IPTV_URI:
        network_type = NEBULA_NetworkTypeIp;
        break;
    case UVAChannel::ID_DVB_C:
        network_type = NEBULA_NetworkTypeDvbc;
        break;
    case UVAChannel::ID_DVB_S:
        network_type = NEBULA_NetworkTypeDvbs1;
        break;
    case UVAChannel::ID_DVB_S2:
        // Note: This covers both DVB-S and DVB-S2.
        network_type = NEBULA_NetworkTypeDvbs;
        break;
    case UVAChannel::ID_DVB_T:
        network_type = NEBULA_NetworkTypeDvbt1;
        break;
    case UVAChannel::ID_DVB_T2:
        // Note: This covers both DVB-T and DVB-T2.
        network_type = NEBULA_NetworkTypeDvbt;
        break;
    default:
        success = false;
        break;
    }
    return success;
}

} // namespace

UVA_STATUS
OperaMuminVideoBroadcastBackend::createChannelObject(int id_type,
                            UVAValueStatus onid_value_status, int onid,
                            UVAValueStatus tsid_value_status, int tsid,
                            UVAValueStatus sid_value_status, int sid,
                            UVAValueStatus source_id_value_status, int source_id,
                            UVAValueStatus ip_broadcast_id_value_status,
                            const std::string& ip_broadcast_id,
                            UVAValueStatus* return_value_status,
                            std::shared_ptr<UVAChannel>& return_value)

{
    if (!m_vb_object)
    {
        return UVA_ERROR;
    }

    TRACE_INFO(("createChannelObject6(): has_onid: %d, onid: %d\n", onid_value_status, onid));
    TRACE_INFO(("createChannelObject6(): has_tsid: %d, tsid: %d\n", tsid_value_status, tsid));
    TRACE_INFO(("createChannelObject6(): has_sid: %d, sid: %d\n", sid_value_status, sid));
    TRACE_INFO(("createChannelObject6(): has_sourceId: %d, sourceId: %d\n", source_id_value_status, source_id));
    TRACE_INFO(("createChannelObject6(): has ipBroadcastID: %d, ipBroadcastID: %s\n", ip_broadcast_id_value_status, ip_broadcast_id.c_str()));

    if (tsid_value_status == UVA_NULL_VALUE)
    {
        tsid = -1;
    }

    bool success = false;
    NEBULA_NetworkType network_type;
    UString ccid;
    std::shared_ptr<UVAChannel> channel;

    if (mapIdTypeToNetworkType(id_type, network_type) &&
        m_vb_object->createChannel(network_type, onid, tsid, sid, ccid))
    {
        NEBULA_Channel channel_info;
        if (m_vb_object->getChannelInfoByID(ccid, channel_info) &&
            (channel_info.networkType & network_type) != 0)
        {
            channel.reset(new OperaOipfChannel(channel_info));
            success = true;
        }
        else
        {
            TRACE_ERROR(("Could not get channel info.\n"));
        }
    }
    else
    {
        TRACE_ERROR(("Could not create channel.\n"));
    }
    *return_value_status = success ? UVA_VALUE : UVA_NULL_VALUE;
    return_value = channel;
    return UVA_OK;
}

UVA_STATUS
OperaMuminVideoBroadcastBackend::getPlayerCapabilities(std::shared_ptr<UVAStringCollection>& capabilities)
{
    // TODO: Implement this
    CABOT_UNUSED_PARAM(capabilities);
    return UVA_NOT_SUPPORTED;
}

UVA_STATUS
OperaMuminVideoBroadcastBackend::applyVideoWindowSettings(const Rect& window_rect, const Rect& clip_rect)
{
    CABOT_UNUSED_PARAM(clip_rect);
    bool ok = false;
    if (m_vb_object)
    {
        NEBULA_DisplayWindow nebula_window = operaRectToNebulaWindow(window_rect.x,
                window_rect.y,
                window_rect.w,
                window_rect.h,
                m_vb_object->getVideoAspectRatio());
        ok = m_vb_object->setVideoOutputWindow(&nebula_window);
    }

    return ok? UVA_OK : UVA_ERROR;
}

UVA_STATUS 
OperaMuminVideoBroadcastBackend::createAVAudioComponent(UVABackend::AVComponent** component,
                           int component_tag,
                           std::string const& language,
                           bool audio_description,
                           int channels,
                           std::string const& encoding)
{
    char language_bcp_47[4] = "";
    LanguageCode(language.c_str()).findISO639Dash1Str(language_bcp_47);

    *component = new OperaMuminAVComponent(UVABackend::COMPONENT_TYPE_AUDIO,
                                            encoding.c_str(),
                                            false,
                                            -1,
                                            UString(language_bcp_47).c_str(),
                                            language.c_str(),
                                            audio_description,
                                            channels,
                                            false,
                                            false,
                                            NULL,
                                            false,
                                            0,
                                            true,
                                            component_tag,
                                            NULL,
                                            NULL);
    m_component_helper->addUserComponent(*component);
    return UVA_OK;
}

UVA_STATUS 
OperaMuminVideoBroadcastBackend::createAVVideoComponent(UVABackend::AVComponent** component,
                           int component_tag,
                           double aspect_ratio,
                           std::string const& encoding)
{
    *component = new OperaMuminAVComponent(UVABackend::COMPONENT_TYPE_VIDEO,
                                                encoding.c_str(),
                                                false,
                                                aspect_ratio,
                                                NULL,
                                                NULL,
                                                false,
                                                2,
                                                false,
                                                false,
                                                NULL,
                                                false,
                                                0,
                                                true,
                                                component_tag,
                                                NULL,
                                                NULL);
    m_component_helper->addUserComponent(*component);
    return UVA_OK;
}

UVA_STATUS
OperaMuminVideoBroadcastBackend::getPlayStateError(UVAValueStatus* play_state_error_status,
                                                   ChannelError& play_state_error)
{
    UVA_STATUS result = UVA_ERROR;

    if (m_vb_object)
    {
        if (m_backend_mode == BackendMode::MODE_SUPERVISOR)
        {
            const auto vbo_play_state_error = m_vb_object->playStateError();
            TRACE_VERBOSE(("%s: vbo_play_state_error<%d>\n", __func__, vbo_play_state_error));

            *play_state_error_status = UVA_UNDEFINED_VALUE;

            if ((vbo_play_state_error >= NEBULA_ChannelNotSupportedByTuner) &&
                (vbo_play_state_error <= NEBULA_UnknownError))
            {
                *play_state_error_status = UVA_VALUE;
                play_state_error = static_cast<ChannelError>(vbo_play_state_error);
            }
            
            result = UVA_OK;
        }
    }

    return result;
}

UVA_STATUS
OperaMuminVideoBroadcastBackend::blockAv()
{
    TRACE_INFO(("%s: backend_mode<%d>\n", __func__, m_backend_mode));

    UVA_STATUS result = UVA_ERROR;

    if (m_vb_object)
    {
        if (m_backend_mode == BackendMode::MODE_SUPERVISOR)
        {
            m_vb_object->blockAV(
                            NebulaBlockActivation::nebula_block_now_and_on_every_channel_change);

            result = UVA_OK;
        }
    }

    return result;
}

UVA_STATUS
OperaMuminVideoBroadcastBackend::unblockAv()
{
    TRACE_INFO(("%s: backend_mode<%d>\n", __func__, m_backend_mode));

    UVA_STATUS result = UVA_ERROR;

    if (m_vb_object)
    {
        if (m_backend_mode == BackendMode::MODE_SUPERVISOR)
        {
            m_vb_object->unblockAV();

            result = UVA_OK;
        }
    }

    return result;
}

void
OperaMuminVideoBroadcastBackend::notifyOpAppStartResult(std::uint16_t application_id,
                                        std::uint32_t organisation_id,
                                        bool started)
{
    TRACE_ONLY_PARAM(application_id);
    TRACE_ONLY_PARAM(organisation_id);

    TRACE_INFO(("%s(0x%x, 0x%x, %s)\n",
                __func__, application_id, organisation_id, started ? "true" : "false"));

    if (started)
    {
        m_opapp_running = started;

        m_vb_object->setActive(false);
    }
}

void
OperaMuminVideoBroadcastBackend::notifyOpAppStopped()
{
    TRACE_INFO(("%s\n", __func__));
    
    if (m_opapp_running)
    {
        // Ensure Operator block is disabled when exiting OpApp as there is no other way to dismiss.
        if (m_vb_object)
        {
            const bool terminating = true;
            m_vb_object->unblockAV(terminating);
        }

        m_opapp_running = false;
    }
}

void
OperaMuminVideoBroadcastBackend::notifyOpAppStateChanged(
                              nebula::OperatorApplicationState old_state,
                              nebula::OperatorApplicationState new_state)
{
    TRACE_INFO(("%s(%s -> %s)\n",
                __func__, toString(old_state), toString(new_state)));

    TRACE_ONLY_PARAM(old_state);

    if (m_vb_object)
    {
        // See ETSI TS 103 606 - V1.1.1 section A.2.4.1.
        const bool inactive = !m_opapp_running ||
                            (new_state == nebula::OperatorApplicationState::background || 
                             new_state == nebula::OperatorApplicationState::transient ||
                             new_state == nebula::OperatorApplicationState::overlaid_transient);
        
        m_vb_object->setActive(!inactive);
    }
}

void
OperaMuminVideoBroadcastBackend::notifyOpAppDiscovered(
                                                std::uint16_t application_id,
                                                std::uint32_t organisation_id,
                                                std::uint32_t application_version)
{
    TRACE_ONLY_PARAM(application_id);
    TRACE_ONLY_PARAM(organisation_id);
    TRACE_ONLY_PARAM(application_version);
    
    TRACE_INFO(("%s(0x%x, 0x%x, %u)\n",
                __func__, application_id, organisation_id, application_version));

    // No action required.
}

void
OperaMuminVideoBroadcastBackend::notifyOpAppNotDiscovered()
{
    TRACE_INFO(("%s()\n", __func__));

    // No action required.
}

void
OperaMuminVideoBroadcastBackend::notifyOpAppUninstalled(std::uint16_t application_id,
                                                        std::uint32_t organisation_id)
{
    TRACE_ONLY_PARAM(application_id);
    TRACE_ONLY_PARAM(organisation_id);
    
    TRACE_INFO(("%s(0x%x, 0x%x)\n", __func__, application_id, organisation_id));

    // No action required.
}

void
OperaMuminVideoBroadcastBackend::notifyOpAppSuspended()
{
    TRACE_INFO(("%s()\n", __func__));

    // No action required.
}

void
OperaMuminVideoBroadcastBackend::notifyOpAppResumed()
{
    TRACE_INFO(("%s()\n", __func__));

    // No action required.
}

void
OperaMuminVideoBroadcastBackend::registerAsOpAppListener()
{
    TRACE_INFO(("%s\n", __func__));

#ifdef INCLUDE_OPAPP_SUPPORT
    if (nebula::opAppEventPublisherProvider())
    {
        nebula::opAppEventPublisherProvider()->registerOpAppListener(*this);
    }
#endif
}

void
OperaMuminVideoBroadcastBackend::deregisterAsOpAppListener()
{
    TRACE_INFO(("%s\n", __func__));

#ifdef INCLUDE_OPAPP_SUPPORT
    if (nebula::opAppEventPublisherProvider())
    {
        nebula::opAppEventPublisherProvider()->deregisterOpAppListener(*this);
    }
#endif
}
