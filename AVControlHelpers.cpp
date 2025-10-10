// ============================================================================
// FILE: AVControlHelpers.cpp (FIXED - SEPARATE FILE, NOT INCLUDED!)
// ============================================================================

#include "AVControlHelpers.hpp"
#include "utilities_public/AnyCommandThread.hpp"
#include "utilities_public/MediaDataSink.hpp"
#include "utilities_public/AnyCommand.hpp"
#include "nebula/core/browser_client/AnyAVControlObjectEventGenerator.hpp"
#include "frost/frost_basictypes.h"
#include "utilities_debug/trace.h"

TRACE_IMPLEMENT("avcontrolobject/helpers");

// ============================================================================
// Dummy Command Thread Implementation
// ============================================================================

class DummyCommandThread : public AnyCommandThread
{
public:
    DummyCommandThread() {
        TRACE_WARN(("Creating DummyCommandThread - fallback implementation\n"));
    }

    virtual ~DummyCommandThread() {}

    // Implement AnyCommandThread interface
    virtual bool enqueueCommand(AnyCommand* command) override {
        TRACE_WARN(("DummyCommandThread::enqueueCommand - executing immediately\n"));
        if (command) {
            command->execute();
            delete command;
        }
        return true;
    }

    // Check your AnyCommandThread base class for exact signature
    // Option 1: If it takes FrostThreadPriority enum:
    virtual bool initialise(const char* /*name*/,
                          frost_int /*priority*/,
                          unsigned int /*stack_size*/) {
        TRACE_INFO(("DummyCommandThread::initialise called\n"));
        return true;
    }

    virtual void terminateAndWaitForExit() {
        TRACE_INFO(("DummyCommandThread::terminateAndWaitForExit called\n"));
    }
};

static DummyCommandThread g_dummy_command_thread;

AnyCommandThread& getGlobalMediaCommandThread() {
    return g_dummy_command_thread;
}

// ============================================================================
// Dummy Media Data Sink
// ============================================================================

static MediaDataSink g_dummy_media_data_sink;

MediaDataSink* getMediaDataSink() {
    return &g_dummy_media_data_sink;
}

// ============================================================================
// Dummy Event Generator Implementation
// ============================================================================

class DummyAVControlObjectEventGenerator : public AnyAVControlObjectEventGenerator
{
public:
    DummyAVControlObjectEventGenerator() {
        TRACE_WARN(("Creating DummyAVControlObjectEventGenerator\n"));
    }

    virtual ~DummyAVControlObjectEventGenerator() {}

    virtual void generateFullScreenChangedEvent() override {
        TRACE_INFO(("DummyEventGen: FullScreenChanged\n"));
    }

    virtual void generatePlayStateChangedEvent(NEBULA_MediaPlayerStatus state) override {
        TRACE_INFO(("DummyEventGen: PlayStateChanged to %d\n", state));
    }

    virtual void generatePlaySpeedChangedEvent(int play_speed) override {
        TRACE_INFO(("DummyEventGen: PlaySpeedChanged to %d\n", play_speed));
    }

    virtual void generatePlayPositionChangedEvent(std::int64_t play_position) override {
        TRACE_VERBOSE(("DummyEventGen: PlayPositionChanged to %lld\n", play_position));
    }

    virtual void generateFileDetailsParsedEvent(NEBULA_MediaFileDetails* /*source_details*/) override {
        TRACE_INFO(("DummyEventGen: FileDetailsParsed\n"));
    }

    // Remove this method if it doesn't exist in base class
    // virtual void generateComponentsChangedEvent() override {
    //     TRACE_INFO(("DummyEventGen: ComponentsChanged\n"));
    // }

    virtual void generateComponentActivenessChangedEvent(
        const NEBULA_AVComponent* /*component_data*/, bool active) override {
        TRACE_INFO(("DummyEventGen: ComponentActivenessChanged active=%d\n", active));
    }

    virtual void generateUpdateAudioComponentEvent(char const* language, bool ad_on) override {
        TRACE_INFO(("DummyEventGen: UpdateAudioComponent lang=%s ad_on=%d\n",
                   language ? language : "(null)", ad_on));
    }
};

static DummyAVControlObjectEventGenerator g_dummy_event_generator;

AnyAVControlObjectEventGenerator* getEventGeneratorForAv() {
    return &g_dummy_event_generator;
}
