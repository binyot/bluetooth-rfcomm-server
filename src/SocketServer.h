#ifndef BTRFCOMM_SOCKETSERVER_H
#define BTRFCOMM_SOCKETSERVER_H


#include <string>
#include <functional>
#include <memory>
#include <bluetooth/sdp.h>
#include <bluetooth/sdp_lib.h>
#include "FdBuffer.h"

// run bluez5 with --compat and chmod 777 /var/run/sdp
class SocketServer {
public:
    explicit SocketServer(
            std::string_view serviceName,
            std::string_view serviceDesc,
            std::string_view serviceProv,
            std::array<uint32_t, 4> uuid,
            uint8_t channel = 11);
    void listen(const std::function<void(std::ostream &outputStream, std::istream &inputStream)> &handle);

private:
    int mAcceptor;
    std::unique_ptr<sdp_session_t, decltype(&::sdp_close)> mSdpSession;
};

#endif //BTRFCOMM_SOCKETSERVER_H
