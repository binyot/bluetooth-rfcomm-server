#include <cstdlib>
#include <cstdio>

#include <unistd.h>
#include <sys/socket.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>
#include <bluetooth/sdp.h>
#include <bluetooth/sdp_lib.h>

#include <memory>
#include <array>
#include <string>
#include <system_error>

#include "SocketServer.h"

template<typename C, typename D, typename... Args>
auto make_c_unique(C c, D d, Args&&... args) {
    auto r = c(std::forward<Args>(args)...);
    if (!r) throw std::system_error(errno, std::generic_category());
    return std::unique_ptr<std::decay_t<decltype(*r)>, decltype(d)>(r, d);
}

auto register_service(
        std::string_view name,
        std::string_view desc,
        std::string_view prov,
        std::array<uint32_t, 4> uuid,
        uint8_t channel
) {
    uuid_t svc_uuid;
    ::sdp_uuid128_create(&svc_uuid, uuid.data());
    auto record = ::sdp_record_alloc();
    ::sdp_set_service_id(record, svc_uuid);

    auto list_free = [](auto x){::sdp_list_free(x, nullptr);};
    uuid_t root_uuid;
    ::sdp_uuid16_create(&root_uuid, PUBLIC_BROWSE_GROUP);
    auto root_list = make_c_unique(::sdp_list_append, list_free, nullptr, &root_uuid);
    ::sdp_set_browse_groups(record, root_list.get());

    uuid_t l2cap_uuid;
    ::sdp_uuid16_create(&l2cap_uuid, L2CAP_UUID);
    auto l2cap_list = make_c_unique(::sdp_list_append, list_free, nullptr, &l2cap_uuid);
    auto proto_list = make_c_unique(::sdp_list_append, list_free, nullptr, l2cap_list.get());

    uuid_t rfcomm_uuid;
    ::sdp_uuid16_create(&rfcomm_uuid, RFCOMM_UUID);
    auto sdp_channel = make_c_unique(::sdp_data_alloc, sdp_data_free, SDP_UINT8, &channel);
    auto rfcomm_list = make_c_unique(::sdp_list_append, list_free, nullptr, &rfcomm_uuid);
    ::sdp_list_append(rfcomm_list.get(), sdp_channel.get());
    ::sdp_list_append(proto_list.get(), rfcomm_list.get());

    auto access_proto_list = make_c_unique(::sdp_list_append, list_free, nullptr, proto_list.get());
    ::sdp_set_access_protos(record, access_proto_list.get());

    ::sdp_set_info_attr(record, name.data(), prov.data(), desc.data());

    bdaddr_t bdaddr_any {};
    bdaddr_t bdaddr_local {0, 0, 0, 0xff, 0xff, 0xff};
    auto session = make_c_unique(::sdp_connect, ::sdp_close, &bdaddr_any, &bdaddr_local, SDP_RETRY_IF_BUSY);

    if (int error = ::sdp_record_register(session.get(), record, 0); error != 0) {
        throw std::system_error(errno, std::generic_category());
    }

    return session;
}

SocketServer::SocketServer(uint8_t channel)
        : mSdpSession(register_service(
        "CommieBot", "Robot control service", "Communism",
        {0xDDDDDDDD, 0xDDDDDDDD, 0xDDDDDDDD, 0xDDDDDDDD},
        channel)) {
    struct sockaddr_rc localAddr{};

    mAcceptor = ::socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);

    localAddr.rc_family = AF_BLUETOOTH;
    bdaddr_t bdaddr_any {};
    localAddr.rc_bdaddr = bdaddr_any;
    localAddr.rc_channel = channel;
    ::bind(mAcceptor,
           reinterpret_cast<sockaddr *>(&localAddr),
           sizeof(localAddr));
}

void SocketServer::listen(const std::function<void(std::ostream &outputStream, std::istream &inputStream)> &handle) {
    sockaddr_rc remoteAddr{};
    socklen_t opt = sizeof(remoteAddr);

    ::listen(mAcceptor, 1);
    int client = ::accept(
            mAcceptor,
            reinterpret_cast<struct sockaddr *>(&remoteAddr),
            &opt);

    char buffer[1024]{};
    ba2str(&remoteAddr.rc_bdaddr, buffer);

    nonstd::FdBuffer streambuffer(client);
    std::ostream outputStream(&streambuffer);
    std::istream inputStream(&streambuffer);

    handle(outputStream, inputStream);

    close(client);
}
