// AVControlObjectIpcClient.hpp (snippet)
#pragma once

#include "nebula/core/browser_client/AnyAVControlObject.hpp"    // for base class and types
#include "nebula_src/adaptation_layer/ipc/external/NebulaIpcHelper.hpp" // for nebulaRpcCall, IPC_NAME
#include "nebula/core/browser_client/AnyAVControlObjectEventGenerator.hpp"

#include "utilities_public/AnyCommandThread.hpp"
#include "utilities_public/MediaDataSource.hpp"
#include "utilities_public/MediaDataSink.hpp"
#include <cstdint>
#include <string>

class AVControlObjectIpcClient : public AnyAVControlObject {
public:
    AVControlObjectIpcClient(AnyCommandThread& media_queue,
                             int streaming_type,
                             const MediaDataSource& media_data_source,
                             MediaDataSink& media_data_sink,
                             AnyAVControlObjectEventGenerator& event_generator,
                             char* origin);
    virtual ~AVControlObjectIpcClient();

    // implement the interface required by AnyAVControlObject
    ExplodingBoolean isInitialised() const override;

    bool setSource(const char* url) override;
    bool play() override;
    void stop() override;
    bool seek(double position_in_msecs) override;
    // ... implement or stub other pure virtuals from AnyAVControlObject to make class concrete
private:
    std::int64_t m_handle; // handle returned by server
};
