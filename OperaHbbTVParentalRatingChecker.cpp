/*
 * Copyright 2017 Cabot Communications.  All rights reserved.
 *
 * @file
 * @brief Handles management of parental ratings for HbbTV applications
 */

#include "OperaHbbTVParentalRatingChecker.hpp"

#include "eclipse/core/AnyResultCallback.hpp"
#include "eclipse/core/Genre.hpp"
#include "language/Country.hpp"

#include "nebula/adaptation_layer/nebula_configuration.h"
#include "nebula/adaptation_layer/nebula_parental_control.h"

#include "nebula/core/ait_hub/ApplicationIterator.hpp"
#include "nebula/browser_interface/AnyWebBrowserUIController.hpp"

#include "frost/frost_semaphore.h"

#include "utilities_debug/unused.h"
#include "utilities_debug/trace.h"

#include <sstream>

TRACE_IMPLEMENT("opera/parental_rating_checker");

namespace
{
    ParentalRatingCode stringToParentalRatingCode(const std::string &rating)
    {
        int int_value = -1;
        std::stringstream ss(rating);
        ss >> int_value;
        if (!ss)
        {
            int_value = -1;
        }

        //convert to ParentalRatingCode: check if the value is out of range
        ParentalRatingCode rating_code = parental_rating_invalid;
        if (int_value > parental_rating_unrestricted && 
             int_value < parental_rating_unknown) 
        {
            rating_code = static_cast<ParentalRatingCode>(int_value);
        }

        return rating_code;
    }
};

// A simple success/failed blocking callback used to retrieve the 
// result of a PinDialog modal operation
class PinDialogResult : public AnyResultCallback
{
public:
    PinDialogResult(const char* app_url);
    PinDialogResult(unsigned short app_id, const char* app_url);
    ~PinDialogResult();

    bool initialise();

    void failed() override;
    void success() override;
    
    bool getResult() const;

private: // copy operations disallowed
    PinDialogResult(PinDialogResult const&); // not to be implemented
    PinDialogResult& operator=(PinDialogResult const&); // not to be implemented

private:    
    unsigned short m_app_id;
    bool m_has_app_id;
    const char* m_app_url;
    bool m_result;
    FROST_Semaphore* m_sema;
};

PinDialogResult::PinDialogResult(const char* app_url)
    : AnyResultCallback()
    , m_app_id(0)
    , m_has_app_id(false)
    , m_app_url(app_url)
    , m_result(false)
    , m_sema(0)
{
}

PinDialogResult::PinDialogResult(unsigned short app_id, const char* app_url)
    : AnyResultCallback()
    , m_app_id(app_id)
    , m_has_app_id(true)
    , m_app_url(app_url)
    , m_result(false)
    , m_sema(0)
{
}

PinDialogResult::~PinDialogResult()
{
    if (m_sema)
    {
        FROST_SemaphoreDestroy(m_sema);
    }
}

bool PinDialogResult::initialise()
{
    if (!m_sema)
    {
        m_sema = FROST_SemaphoreCreate(0);
    }

    return m_sema ? true : false;
}

void PinDialogResult::success()
{
    if (m_has_app_id)
    {
        TRACE_VERBOSE((
            "Got pin dialog for application %d url %s result: success",
            m_app_id,
            m_app_url)
        );
    }
    else
    {
        TRACE_VERBOSE(("Got pin dialog for url %s result: success", m_app_url));
    }

    m_result = true;
    FROST_SemaphoreSignal(m_sema);
}

void PinDialogResult::failed()
{
    if (m_has_app_id)
    {
        TRACE_VERBOSE((
            "Got pin dialog for application %d url %s result: failed",
            m_app_id,
            m_app_url)
        );
    }
    else
    {
        TRACE_VERBOSE((
            "Got pin dialog for url %s result: failed",
            m_app_url)
        );
    }

    m_result = false;
    FROST_SemaphoreSignal(m_sema);
}

bool PinDialogResult::getResult() const
{
    FROST_SemaphoreWait(m_sema);
    return m_result;
}

namespace
{
    bool getPinAuthorisationResult(AnyWebBrowserUIController& ui_controller, PinDialogResult& dialog_result)
    {
        if (dialog_result.initialise())
        {
            ui_controller.doModalDialog(DialogType::pin_dialog, &dialog_result);
            // This call will block the current thread until the modal operation is finished
            return dialog_result.getResult();
        }

        // If we made this call, the application did not meet parental rating requirements,
        // and, since we couldn't prompt the user for a pin entry, we should default to blocking the app
        return false;
    }
}

OperaHbbTVParentalRatingChecker::OperaHbbTVParentalRatingChecker()
    : m_ui_controller(nullptr)
{
}

OperaHbbTVParentalRatingChecker::~OperaHbbTVParentalRatingChecker()
{
    m_parental_rating.clear();
}

ExplodingBoolean
OperaHbbTVParentalRatingChecker::initialise(AnyWebBrowserUIController* ui_controller)
{
    m_ui_controller = ui_controller;
    
    return ExplodingBoolean(m_ui_controller != nullptr);
}

bool
OperaHbbTVParentalRatingChecker::isRatingBlocked(unsigned short app_id, const char* app_url)
{
    CABOT_UNUSED_PARAM(app_url);

    bool blocked = false;

    // We assume that the AIT was processed correctly before attempting to start an
    // application contained within it
    VectorOfParentalRatings ratings;
    ApplicationParentalRating::iterator it = m_parental_rating.find(app_id);
    if (it != m_parental_rating.end())
    {
        ratings = it->second;
    }  

    // If we lack any rating data, we can't block the application
    if (!ratings.empty())
    {
        for (VectorOfParentalRatings::iterator rating_it = ratings.begin();
            rating_it != ratings.end();
            ++rating_it)
        {
            const std::pair<UString, ParentalRatingCode>& rating = *rating_it;

            if (isRatingBlocked(rating.second, rating.first.c_str()))
            {
                blocked = true;
                break;
            }
        }
    }

    return blocked;
}

bool
OperaHbbTVParentalRatingChecker::isRatingBlocked(const char* app_url,
                                                 std::string const& query_data)
{
    bool result = false;

    CABOT_UNUSED_PARAM(app_url);

    //HbbTV only supports the dvb-si parental rating scheme
    const std::string scheme("dvb-si");

    frost_int rating_value = NEBULA_GetParentalRatingCodeFromMpegDash(query_data.c_str());

    if (rating_value != -1)
    {
        ParentalRatingCode parental_rating_code = static_cast<ParentalRatingCode>(rating_value);

        result = isRatingBlocked(parental_rating_code, scheme.c_str());
    }

    return result;
}

bool
OperaHbbTVParentalRatingChecker::isRatingBlocked(ParentalRatingCode rating_code,
                                                 const char* scheme)
{
    const frost_int rating_value = static_cast<frost_int>(rating_code);

    // Nebula handles the proper comparison of rating codes depending on
    // the scheme, country, user preferences, etc
    return NEBULA_IsParentalRatingBlocked(scheme, rating_value);
}

bool
OperaHbbTVParentalRatingChecker::authorise(unsigned short app_id, const char* app_url)
{
    PinDialogResult result(app_id, app_url);

    return getPinAuthorisationResult(*m_ui_controller, result);
}

bool
OperaHbbTVParentalRatingChecker::authorise(const char* app_url)
{
    PinDialogResult result(app_url);

    return getPinAuthorisationResult(*m_ui_controller, result);
}

void
OperaHbbTVParentalRatingChecker::processRating(unsigned short app_id,
                                                    const char* app_url,
                                                    const std::string &rating,
                                                    const std::string &scheme,
                                                    const std::string &country)
{
    CABOT_UNUSED_PARAM(app_url);

    if (!rating.empty() && !scheme.empty() && !country.empty())
    {
        const ParentalRatingCode rating_code = stringToParentalRatingCode(rating);

        // Since the data is parsed externally, we should only keep valid ratings                                                                    
        if (rating_code != parental_rating_invalid)
        {
            // This call will create a new entry in the rating map if it didn't' exist before or
            // overwrite the existing values
            m_parental_rating[app_id].push_back(
                std::make_pair(UString(scheme.c_str()), rating_code)
            );
        }
    }
}
