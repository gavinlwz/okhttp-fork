//
// Created by Good_Pudge.
//

#include "../../util/util.hpp"
#include "../../include/Exception.hpp"
#include "../../include/Socket.hpp"
#include "WSAInit.hpp"
#include "../../include/InetAddress.hpp"
#include <iostream>

namespace ohf {
    Socket::Socket() {
        if ((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
            throw Exception(Exception::Code::FAILED_TO_CREATE_SOCKET,
                            "Failed to create socket: " + util::getWSAError());
    }

    Socket::~Socket() {
        closesocket(socket_fd);
    }

    std::iostream &Socket::connect(const std::string &address, const int &port) {
        // Address setup
        sockaddr_in addr;
        ZeroMemory(&addr, sizeof(addr));
        addr.sin_family = AF_INET; // TCP/IP
        addr.sin_addr.S_un.S_addr = inet_addr(InetAddress(address).hostAddress().c_str());
        addr.sin_port = htons(port); // Port
        // Connect
        if (::connect(socket_fd, (sockaddr *) &addr, sizeof(addr)) == SOCKET_ERROR)
            throw Exception(Exception::Code::FAILED_TO_CREATE_CONNECTION, "Failed to create connection: " +
                                                                          util::getWSAError());

        // Init IO stream
        buf = std::make_shared<StreamBuf>(this);
        ios = std::make_shared<std::iostream>(buf.get());
        return *ios;
    }

    void Socket::send(const char *data, int size) {
        if (::send(socket_fd, data, size, 0) == SOCKET_ERROR)
            throw Exception(Exception::Code::FAILED_TO_SEND_DATA, "Failed to send data: " + util::getWSAError());
    }

    std::string Socket::receive(size_t size) {
        int len = 0;
        char *buffer = new char[size];
        if ((len = recv(socket_fd, buffer, size, 0)) == SOCKET_ERROR)
            throw Exception(Exception::Code::FAILED_TO_RECEIVE_DATA, "Failed to receive data: " + util::getWSAError());
        return std::string(buffer, len);
    }

    void Socket::shutdown(int how) {
        if (::shutdown(socket_fd, how) == SOCKET_ERROR)
            throw Exception(Exception::FAILED_TO_SHUTDOWN_SOCKET, "Failed to shutdown socket: " + util::getWSAError());
    }
}