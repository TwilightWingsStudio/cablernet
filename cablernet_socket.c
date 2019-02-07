// Copyright (C) 2018-2019 ZCaliptium.

#include "cablernet.h"

// _WIN32 macro is also present for MinGW. So it won't create any problems.
#ifdef _WIN32
#include <WinSock2.h>
#include <WS2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

#define close closesocket

#include <cerrno>

// Unix
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h> // mem...
#endif

int CBLRNET_Startup()
{
    // Windows needs sockets to be initialized before any calls.
#ifdef _WIN32
    WSADATA WsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &WsaData);

    if (result != NO_ERROR) {
        return result;
    }
#endif

    return 0; // Say no errors.
}

void CBLRNET_Shutdown()
{
#ifdef _WIN32
    WSACleanup();
#endif
}

int CBLRNET_Socket_Open(cblrnetsocket_t *pSocket, cblrnetlayerprotocol_t protocol, cblrnetsockettype_t type)
{
    // If socket is invalid...
    if (!pSocket) {
        return 1;
    }

    int address_family = (protocol == CBLRNET_LAYER_IPv4) ? AF_INET : AF_INET6;
    int socket_Type = (type == CBLRNET_SOCKET_TYPE_DGRAM) ? SOCK_DGRAM : SOCK_STREAM;

    pSocket->handle = socket(address_family, socket_Type, 0);

    // If failed to open new socket.
    if (pSocket->handle <= 0) {
        return 1;
    }

    pSocket->protocol = protocol;

    // If dual-stack socket.
    int ipv6_only = (type == CBLRNET_LAYER_ANYIP) ? 0 : 1;

    // If IPv6 connection. Report error if IPV6_V6ONLY can't be set.
    if (type != CBLRNET_LAYER_IPv4 && setsockopt(pSocket->handle, IPPROTO_IPV6, IPV6_V6ONLY, (const char *)&ipv6_only, sizeof(ipv6_only)) != 0) {
        return 1;
    }

    return 0;
}

int CBLRNET_Socket_Bind(cblrnetsocket_t *pSocket, cblrnetaddress_t *pAddress)
{
    // If socket or address is invalid...
    if (!pSocket || !pAddress) {
        return 1;
    }

    // If address protocol is invalid...
    if (pAddress->protocol == CBLRNET_LAYER_UNKNOWN) {
        return 1;
    }

    // If IPv4 socket and non-IPv4 address.
    if (pSocket->protocol == CBLRNET_LAYER_IPv4 && pAddress->protocol != pSocket->protocol) {
        return 1;
    }

    // IPv4
    if (pSocket->protocol == CBLRNET_LAYER_IPv4) {
        struct sockaddr_in addr;

        memset(&addr, 0, sizeof(addr));

        addr.sin_family = AF_INET;
        addr.sin_port = htons(pAddress->port);
        addr.sin_addr.s_addr = htonl(pAddress->data.v4);

        if (bind(pSocket->handle, (const struct sockaddr *)&addr, sizeof(struct sockaddr_in)) != 0) {
            return 1;
        }

    // IPv6
    } else {
        struct sockaddr_in6 addr;

        memset(&addr, 0, sizeof(addr));
        memcpy(&addr.sin6_addr, &pAddress->data.v6[0], 16);

        addr.sin6_family = AF_INET6;
        addr.sin6_port = htons(pAddress->port);
        addr.sin6_scope_id = pAddress->scopeId;

        if (bind(pSocket->handle, (const struct sockaddr *)&addr, sizeof(struct sockaddr_in6)) != 0) {
            return 1;
        }
    }

    return 0;
}

void CBLRNET_Socket_Close(cblrnetsocket_t *pSocket)
{
    // If socket is invalid then there is no need to close it.
    if (!pSocket) {
        return;
    }

    close(pSocket->handle);
}

int CBLRNET_Socket_Read(cblrnetsocket_t *pSocket, cblrnetaddress_t *pAddress, void *pData, int maxLength)
{
    s32 slReceived = 0;
    socklen_t address_len = 0;

    if (!pSocket || !pAddress || !pData) {
        return -1;
    }

    // IPv4
    if (pSocket->protocol == CBLRNET_LAYER_IPv4)
    {
        struct sockaddr_in address;
        memset(&address, 0, sizeof(address));
        address_len = sizeof(address);

        slReceived = recvfrom(pSocket->handle, (char *)pData, maxLength, 0, (struct sockaddr *)&address, &address_len);

        if (slReceived < 0) {
            return -1;
        }

        pAddress->data.v4 = ntohl(address.sin_addr.s_addr);
        pAddress->port = ntohs(address.sin_port);

    // IPv6
    } else if (pSocket->protocol == CBLRNET_LAYER_IPv6) {
        struct sockaddr_in6 address;
        memset(&address, 0, sizeof(address));
        address_len = sizeof(address);

        slReceived = recvfrom(pSocket->handle, (char *)pData, maxLength, 0, (struct sockaddr *)&address, &address_len);

        if (slReceived < 0) {
            return -1;
        }

        // Return sender IP & port as argument.
        memcpy(pAddress->data.v6, address.sin6_addr.s6_addr, 16);
        pAddress->scopeId = address.sin6_scope_id;
        pAddress->port = ntohs(address.sin6_port);
    }

    return slReceived;
}

int CBLRNET_Socket_SetOption(cblrnetsocket_t *pSocket, cblrnetsocketoption_t option, int value)
{
    if (!pSocket) {
        return -1;
    }

    int result = -1;

    switch (option)
    {
        case CBLRNET_SOCKOPT_NONBLOCKING: {
#ifdef _WIN32
            u32 mode = (u32) value;
            result = ioctlsocket (pSocket->handle, FIONBIO, &mode);
#else
            int flags = fcntl(_handle, F_GETFL, 0);

            if (flags == -1) {
                return 0;
            }

            flags = value ? (flags | O_NONBLOCK) : (flags & ~O_NONBLOCK);
            return (fcntl(pSocket->handle, F_SETFL, flags) == 0) ? true : false;
#endif
        } break;

        case CBLRNET_SOCKOPT_BROADCAST: {
            result = setsockopt(pSocket->handle, SOL_SOCKET, SO_BROADCAST, (char *)&value, sizeof(int));
        } break;

        case CBLRNET_SOCKOPT_RXBUF: {
            result = setsockopt(pSocket->handle, SOL_SOCKET, SO_RCVBUF, (char *)&value, sizeof (int));
        } break;

        case CBLRNET_SOCKOPT_TXBUF: {
            result = setsockopt(pSocket->handle, SOL_SOCKET, SO_SNDBUF, (char *)&value, sizeof (int));
        } break;

        case CBLRNET_SOCKOPT_REUSEADDR: {
            result = setsockopt(pSocket->handle, SOL_SOCKET, SO_REUSEADDR, (char *)&value, sizeof (int));
        } break;

        default: {
            break;
        }
    }

    return result;
}

int CBLRNET_Socket_WaitData(cblrnetsocket_t *pSocket, s32 timeout_ms)
{
    fd_set readset;
    int result;
    struct timeval tv;

    // Initialize the set.
    FD_ZERO(&readset);
    FD_SET(pSocket->handle, &readset);

    // Initialize time out struct.
    tv.tv_sec = 0;
    tv.tv_usec = timeout_ms * 1000;

    result = select(pSocket->handle + 1, &readset, NULL, NULL, &tv);

    // If socket destroyed (EBADF) then don't throw exception.
    // EINTR sometimes can occur with non-blocking socket on some systems. Don't throw exception too!
    if (result < 0 && (errno == EINTR || errno == EBADF)) {
        return 0; // Get out of here!
    }

    // Timeout with no data.
    if (result == 0) {
        return 0; // Get out of here!
    }

    // Error.
    if (result < 0) {
        // TODO: Maybe throw exception or do something.
        return 0;
    } else if (!FD_ISSET(pSocket->handle, &readset)) {
        return 0; // No data!
    }

    // There is some data!
    return 1;
}

int CBLRNET_Socket_Send(cblrnetsocket_t *pSocket, cblrnetaddress_t *pAddress, const void *data, u32 size)
{
    s32 sent;

    // IPv4
    if (pSocket->protocol == CBLRNET_LAYER_IPv4)
    {
        struct sockaddr_in address;

        // Setup the destination point.
        address.sin_family = AF_INET;
        address.sin_addr.s_addr = htonl(pAddress->data.v4);
        address.sin_port = htons(pAddress->port);

        // Send the data.
        sent = sendto(pSocket->handle, (const char *)data, size, 0, (struct sockaddr *)&address,
                      sizeof(struct sockaddr_in));

    // IPv6
    } else if (pSocket->protocol == CBLRNET_LAYER_IPv6) {
        struct sockaddr_in6 address;

        // Setup the destination point.
        address.sin6_family = AF_INET6;
        memcpy(&address.sin6_addr, pAddress->data.v6, 16);
        address.sin6_port = htons(pAddress->port);
        address.sin6_scope_id = pAddress->scopeId;

        // Send the data.
        sent = sendto(pSocket->handle, (const char *)data, size, 0, (struct sockaddr *)&address,
                      sizeof(struct sockaddr_in6));
    }

    return sent > 0; // TODO: Maybe add more extensive check.
}