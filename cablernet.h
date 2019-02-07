// Copyright (C) 2018-2019 ZCaliptium.

#ifndef CBLRNET_H
#define CBLRNET_H

#include "zkttypes.h"

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

typedef enum socxxnetsocketoption_e
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

typedef struct socxxnetaddress_s
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
typedef enum socxxnetsocketstype_e
{
    CBLRNET_SOCKET_TYPE_DGRAM = 1,
    CBLRNET_SOCKET_TYPE_STREAM
}
cblrnetsockettype_t;

typedef struct socxxnetsocket_s
{
    int handle;
    cblrnetlayerprotocol_t protocol;
}
cblrnetsocket_t;


//! Convert zero-terminated string to address.
CBLRNET_API int CBLRNET_Address_FromString(cblrnetaddress_t *pAddress, const char *pString);

//! Convert address to string.
CBLRNET_API int CBLRNET_Address_ToString(cblrnetaddress_t *pAddress, c8 *pBuffer, u8 bufferLength);

//!
CBLRNET_API int CBLRNET_Address_Resolve(cblrnetaddress_t *pAddress, const c8 *pName);


//! Prepare sockets for usage. Call it once per your program.
//! Returns 0 if everything fine. Error index othervise.
CBLRNET_API int CBLRNET_Startup(void);

//! Shutdown sockets when you finished usage. Call it once per your program.
CBLRNET_API void CBLRNET_Shutdown(void);


//! Initialize new socket.
//! Returns 0 if everything fine. 1 othervise.
CBLRNET_API int CBLRNET_Socket_Open(cblrnetsocket_t *pSocket, cblrnetlayerprotocol_t protocol, cblrnetsockettype_t type);

//! Bind socket to given address.
CBLRNET_API int CBLRNET_Socket_Bind(cblrnetsocket_t *pSocket, cblrnetaddress_t *pAddress);

//! Close the socket.
CBLRNET_API void CBLRNET_Socket_Close(cblrnetsocket_t *pSocket);

//! Receive the data.
CBLRNET_API int CBLRNET_Socket_Read(cblrnetsocket_t *pSocket, cblrnetaddress_t *pAddress, void *pData, int maxLength);

//! Set specific option for the socket.
CBLRNET_API int CBLRNET_Socket_SetOption(cblrnetsocket_t *pSocket, cblrnetsocketoption_t option, int value);

//! Put current thread into sleep on given timeot.
CBLRNET_API int CBLRNET_Socket_WaitData(cblrnetsocket_t *pSocket, s32 timeout_ms);

//! Send the data.
CBLRNET_API int CBLRNET_Socket_Send(cblrnetsocket_t *pSocket, cblrnetaddress_t *pAddress, const void *data, u32 size);


// Listen

// Connect

// Poll



#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* include-once check */