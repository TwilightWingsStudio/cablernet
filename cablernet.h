// Copyright (C) 2018-2019 ZCaliptium.

#ifndef CBLRNET_H
#define CBLRNET_H

#include "cablernet_types.h"

#if defined(CBLRNET_DYNAMIC_LIB)
    #if defined(_WIN32)
        #if defined(CBLRNET_EXPORTS)
            #define CBLRNET_API __declspec(dllexport)
        #else
            #define CBLRNET_API __declspec(dllimport)
        #endif
    #else
        #define CBLRNET_API __attribute__((visibility("default")))
    #endif
#else
    #define CBLRNET_API
#endif

#ifdef __cplusplus
extern "C" {
#endif

//! An enum for network layer types.
typedef enum cblrnetlayerprotocol_e
{
    //! Unknown. Used for invalid addresses.
    CBLRNET_LAYER_UNKNOWN = 0,

    //! Internet Protocol version 4.
    CBLRNET_LAYER_IPv4,

    //! Internet Protocol version 6.
    CBLRNET_LAYER_IPv6,

    //! Any IP protocol (IPv4 or IPv6).
    CBLRNET_LAYER_ANYIP
}
cblrnetlayerprotocol_t;

typedef enum cblrnetsocketoption_e
{
    //! Switch socket between blocking and non-blocking modes.
    CBLRNET_SOCKOPT_NONBLOCKING = 1,

    //! Toggle broadcast packets sending. Datagram only.
    CBLRNET_SOCKOPT_BROADCAST,

    //! Incoming packet buffer size.
    CBLRNET_SOCKOPT_RXBUF,

    //! Outgoing packet buffer size.
    CBLRNET_SOCKOPT_TXBUF,

    //! Allows socket use already used address.
    CBLRNET_SOCKOPT_REUSEADDR
}
cblrnetsocketoption_t;

typedef struct cblrnetaddress_s
{
    u16 port;
    u32 scopeId;

    union {
        u32 v4;
        u8 v6[16];
    } data;

    cblrnetlayerprotocol_t protocol;
}
cblrnetaddress_t;

//! This enum specifies socket type.
typedef enum cblrnetsocketstype_e
{
    CBLRNET_SOCKET_TYPE_DGRAM = 1,
    CBLRNET_SOCKET_TYPE_STREAM
}
cblrnetsockettype_t;

typedef struct cblrnetsocket_s
{
    int handle;
    cblrnetlayerprotocol_t protocol;
}
cblrnetsocket_t;


//! Convert zero-terminated string to address.
CBLRNET_API int cblrnet_address_fromstring(cblrnetaddress_t *pAddress, const char *pString);

//! Convert address to string.
CBLRNET_API int cblrnet_address_tostring(cblrnetaddress_t *pAddress, c8 *pBuffer, u8 bufferLength);

//!
CBLRNET_API int cblrnet_address_resolve(cblrnetaddress_t *pAddress, const c8 *pName);

//!
CBLRNET_API int cblrnet_address_compare(cblrnetaddress_t *pFirst, cblrnetaddress_t *pSecond);


//! Prepare sockets for usage. Call it once per your program.
//! Returns 0 if everything fine. Error index othervise.
CBLRNET_API int cblrnet_startup(void);

//! Shutdown sockets when you finished usage. Call it once per your program.
CBLRNET_API void cblrnet_shutdown(void);


//! Initialize new socket.
//! Returns 0 if everything fine. 1 othervise.
CBLRNET_API int cblrnet_socket_open(cblrnetsocket_t *pSocket, cblrnetlayerprotocol_t protocol, cblrnetsockettype_t type);

//! Bind socket to given address.
CBLRNET_API int cblrnet_socket_bind(cblrnetsocket_t *pSocket, cblrnetaddress_t *pAddress);

//! Close the socket.
CBLRNET_API void cblrnet_socket_close(cblrnetsocket_t *pSocket);

//! Set specific option for the socket.
CBLRNET_API int cblrnet_socket_setoption(cblrnetsocket_t *pSocket, cblrnetsocketoption_t option, int value);

//! Put current thread into sleep on given timeot.
CBLRNET_API int cblrnet_socket_waitdata(cblrnetsocket_t *pSocket, s32 timeout_ms);

//! Receive the data.
CBLRNET_API int cblrnet_socket_recvfrom(cblrnetsocket_t *pSocket, cblrnetaddress_t *pAddress, void *pData, int maxLength);

//! Send the data.
CBLRNET_API int cblrnet_socket_sendto(cblrnetsocket_t *pSocket, cblrnetaddress_t *pAddress, const void *data, u32 size);


//! Listen for connections on a socket. Used by stream sockets.
CBLRNET_API int cblrnet_socket_listen(cblrnetsocket_t *pSocket, s32 connection_count);

//! Accept a connection on a socket. Used by stream sockets.
//! @param[in]  pSocket
//! @param[out] pClientAddress
//! @param[out] pAcceptSocket
CBLRNET_API int cblrnet_socket_accept(cblrnetsocket_t *pSocket, cblrnetaddress_t *pClientAddress, cblrnetsocket_t *pAcceptSocket);

//! Connect to address. Used by stream sockets.
CBLRNET_API int cblrnet_socket_connect(cblrnetsocket_t *pSocket, cblrnetaddress_t *pAddress);

//! TODO: description.
CBLRNET_API int cblrnet_socket_poll(cblrnetsocket_t *pSocket);

//! Get code of last error.
CBLRNET_API int cblrnet_socket_errno();

//! Get error description by errno
//! @warning You need to know max size of message string.
CBLRNET_API int cblrnet_get_err_message(int err, c8 *pBuffer, u8 bufferLength);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* include-once check */
