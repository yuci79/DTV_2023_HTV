#include "utilities_public/AnyCommandThread.hpp"
#include "nebula/core/browser_client/AnyAVControlObject.hpp"
#include "utilities_public/MediaDataSink.hpp"
#include "nebula/core/browser_client/AnyAVControlObjectEventGenerator.hpp"

AnyCommandThread& getGlobalMediaCommandThread() {
    static AnyCommandThread dummyThread;
    return dummyThread;
}

MediaDataSink* getMediaDataSink() {
    static MediaDataSink dummySink;
    return &dummySink;
}

AnyAVControlObjectEventGenerator* getEventGeneratorForAv() {
    static AnyAVControlObjectEventGenerator dummyEvent;
    return &dummyEvent;
}
