// Copyright (C) 2018 ZCaliptium.

#ifndef SOCXXNET_H
#define SOCXXNET_H

#include "zkttypes.h"

#if defined(SOCXXNET_DYNAMIC_LIB)
    #if defined(_WIN32)
        #if defined(SOCXXNET_EXPORTS)
            #define SOCXXNET_API __declspec(dllexport)
        #else
            #define SOCXXNET_API __declspec(dllimport)
        #endif
    #else
        #define SOCXXNET_API __attribute__((visibility("default")))
    #endif
#else
    #define SOCXXNET_API
#endif

#ifdef __cplusplus
extern "C" {
#endif

//! An enum for network layer types.
typedef enum socxxnetlayerprotocol_e
{
    //! Unknown. Used for invalid addresses.
    SOCXXNET_LAYER_UNKNOWN = 0,

    //! Internet Protocol version 4.
    SOCXXNET_LAYER_IPv4,

    //! Internet Protocol version 6.
    SOCXXNET_LAYER_IPv6,

    //! Any IP protocol (IPv4 or IPv6).
    SOCXXNET_LAYER_ANYIP
}
socxxnetlayerprotocol_t;

typedef enum socxxnetsocketoption_e
{
    //! Switch socket between blocking and non-blocking modes.
    SOCXXNET_SOCKOPT_NONBLOCKING = 1,

    //! Toggle broadcast packets sending. Datagram only.
    SOCXXNET_SOCKOPT_BROADCAST,

    //! Incoming packet buffer size.
    SOCXXNET_SOCKOPT_RXBUF,

    //! Outgoing packet buffer size.
    SOCXXNET_SOCKOPT_TXBUF,

    //! Allows socket use already used address.
    SOCXXNET_SOCKOPT_REUSEADDR
}
socxxnetsocketoption_t;

typedef struct socxxnetaddress_s
{
    u16 port;
    u32 scopeId;

    union {
        u32 v4;
        u8 v6[16];
    } data;

    socxxnetlayerprotocol_t protocol;
}
socxxnetaddress_t;

//! This enum specifies socket type.
typedef enum socxxnetsocketstype_e
{
    SOCXXNET_SOCKET_TYPE_DGRAM = 1,
    SOCXXNET_SOCKET_TYPE_STREAM
}
socxxnetsockettype_t;

typedef struct socxxnetsocket_s
{
    int handle;
    socxxnetlayerprotocol_t protocol;
}
socxxetsocket_t;


//! Convert zero-terminated string to address.
SOCXXNET_API int SOCXXNET_Address_FromString(socxxnetaddress_t *pAddress, const char *pString);

//! Convert address to string.
SOCXXNET_API int SOCXXNET_Address_ToString(socxxnetaddress_t *pAddress, c8 *pBuffer, u8 bufferLength);

//!
SOCXXNET_API int SOCXXNET_Address_Resolve(socxxnetaddress_t *pAddress, const c8 *pName);


//! Prepare sockets for usage. Call it once per your program.
//! Returns 0 if everything fine. Error index othervise.
SOCXXNET_API int SOCXXNET_Startup(void);

//! Shutdown sockets when you finished usage. Call it once per your program.
SOCXXNET_API void SOCXXNET_Shutdown(void);


//! Initialize new socket.
//! Returns 0 if everything fine. 1 othervise.
SOCXXNET_API int SOCXXNET_Socket_Open(socxxetsocket_t *pSocket, socxxnetlayerprotocol_t protocol, socxxnetsockettype_t type);

//! Bind socket to given address.
SOCXXNET_API int SOCXXNET_Socket_Bind(socxxetsocket_t *pSocket, socxxnetaddress_t *pAddress);

//! Close the socket.
SOCXXNET_API void SOCXXNET_Socket_Close(socxxetsocket_t *pSocket);

//! Receive the data.
SOCXXNET_API int SOCXXNET_Socket_Read(socxxetsocket_t *pSocket, socxxnetaddress_t *pAddress, void *pData, int maxLength);

//! Set specific option for the socket.
SOCXXNET_API int SOCXXNET_Socket_SetOption(socxxetsocket_t *pSocket, socxxnetsocketoption_t option, int value);

//! Put current thread into sleep on given timeot.
SOCXXNET_API int SOCXXNET_Socket_WaitData(socxxetsocket_t *pSocket, s32 timeout_ms);

//! Send the data.
SOCXXNET_API int SOCXXNET_Socket_Send(socxxetsocket_t *pSocket, socxxnetaddress_t *pAddress, const void *data, u32 size);


// Listen

// Connect

// Poll



#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* include-once check */
