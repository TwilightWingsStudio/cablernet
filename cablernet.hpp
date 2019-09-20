// Copyright (C) 2018-2019 ZCaliptium.

#ifndef CBLRNET_HPP
#define CBLRNET_HPP

#include "cablernet.h"

#ifdef __cplusplus
namespace cblrnet
{
namespace address
{

inline int fromstring(cblrnetaddress_t &address, const char *pString)
{
  return cblrnet_address_fromstring(&address, pString);
}

inline int tostring(cblrnetaddress_t &address, c8 *pBuffer, u8 bufferLength)
{
  return cblrnet_address_tostring(&address, pBuffer, bufferLength);
}

inline int resolve(cblrnetaddress_t &address, const c8 *pName)
{
  return cblrnet_address_resolve(&address, pName);
}

inline int compare(cblrnetaddress_t &first, cblrnetaddress_t &second)
{
  return cblrnet_address_compare(&first, &second);
}

} // namespace address

inline int startup()
{
  return cblrnet_startup();
}

inline void shutdown()
{
  cblrnet_shutdown();
}

inline int get_err_message(int err, c8 *pBuffer, u8 bufferLength)
{
  return cblrnet_get_err_message(err, pBuffer, bufferLength);
}
  
namespace socket
{

inline int open(cblrnetsocket_t &socket, cblrnetlayerprotocol_t protocol, cblrnetsockettype_t type)
{
  return cblrnet_socket_open(&socket, protocol, type);
}
  
inline int bind(cblrnetsocket_t &socket, cblrnetaddress_t &address)
{
  return cblrnet_socket_bind(&socket, &address);
}
  
inline void close(cblrnetsocket_t &socket)
{
  cblrnet_socket_close(&socket);
}
  
inline int setoption(cblrnetsocket_t &socket, cblrnetsocketoption_t option, int value)
{
  return cblrnet_socket_setoption(&socket, option, value);
}
  
inline int waitdata(cblrnetsocket_t &socket, s32 timeout_ms)
{
  return cblrnet_socket_waitdata(&socket, timeout_ms);
}
  
inline int recvfrom(cblrnetsocket_t &socket, cblrnetaddress_t &address, void *pData, int maxLength)
{
  return cblrnet_socket_recvfrom(&socket, &address, pData, maxLength);
}
  
inline int sendto(cblrnetsocket_t &socket, cblrnetaddress_t &address, const void *pData, u32 size)
{
  return cblrnet_socket_sendto(&socket, &address, pData, size);
}
  
inline int listen(cblrnetsocket_t &socket, s32 connection_count)
{
  return cblrnet_socket_listen(&socket, connection_count);
}
  
inline int accept(cblrnetsocket_t &socket, cblrnetaddress_t &client_address, cblrnetsocket_t &accept_socket)
{
  return cblrnet_socket_accept(&socket, &client_address, &accept_socket);
}
  
inline int connect(cblrnetsocket_t &socket, cblrnetaddress_t &address)
{
  return cblrnet_socket_connect(&socket, &address);
}
  
inline int poll(cblrnetsocket_t &socket)
{
  return cblrnet_socket_poll(&socket);
}
  
inline int errno()
{
  return cblrnet_socket_errno();
}

} // namespace socket

} // namespace cblrnet
#endif

#endif