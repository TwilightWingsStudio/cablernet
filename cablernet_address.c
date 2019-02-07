// Copyright (C) 2018-2019 ZCaliptium.

#include <stdio.h>
#include <string.h>

#include "cablernet.h"

#ifdef _WIN32
#include <WinSock2.h>
#include <WS2tcpip.h>
#endif

int CBLRNET_Address_FromString(cblrnetaddress_t *pAddress, const char *pString)
{
    if (!pAddress) {
        return 1;
    }

    u16 a, b, c, d;
    u16 port = 0;
    size_t length = strlen(pString);

    // If string is invalid or starts from zero..
    if (!pString || pString[0] == 0) {
        return 1;
    }

    if (length < 3) {
        return 1;
    }

    // IPv4 address with port.
    if (sscanf(pString, "%3hd.%3hd.%3hd.%3hd:%hd", &a, &b, &c, &d, &port) == 5) {
        pAddress->protocol = CBLRNET_LAYER_IPv4;
        pAddress->data.v4 = (u32)((a << 24) | (b << 16) | (c << 8) | d);
        pAddress->port = port;

        return 0; // Report success.
    }

    // IPv4 address without port.
    if (sscanf(pString, "%3hd.%3hd.%3hd.%3hd", &a, &b, &c, &d) == 4) {
        pAddress->protocol = CBLRNET_LAYER_IPv4;
        pAddress->data.v4 = (u32)((a << 24) | (b << 16) | (c << 8) | d);

        return 0; // Report success.
    }

    // IPv6
    {
        char *data = (char*)pString;
        int isWasOpenBracket = 0;
        int isWasCloseBracket = 0;
        int isWasScopeIdSeparator = 0;

        char buffer[64];
        char scope_buffer[16];
        memset(&buffer[0], 0, 64);
        memset(&scope_buffer[0], 0, 16);
        u32 it = 0;
        u32 scopeIt = 0;
        u32 scopeId = 0;

        while (*data != 0)
        {
            // If there was close bracket but it is not end then here can be port number.
            if (isWasCloseBracket) {
                if (sscanf(data, ":%d", &port) != 1) {
                    return 1;
                }

                pAddress->port = port;
                break; // If scanned then exit from the cycle.
            }

            switch (data[0])
            {
                case '.': {
                    return 1;
                }

                case '[': {
                    // Square bracket can be only first symbol in address.
                    if (data != pString) {
                        return 1;
                    }

                    isWasOpenBracket = 1;
                } break;

                case ']': {
                    // If were wasn't open bracket then we don't need to close anything.
                    if (!isWasOpenBracket) {
                        return 1;
                    }

                    isWasCloseBracket = 1;
                } break;

                case '%': {
                    // Here can't be two scopeid separators!
                    if (isWasScopeIdSeparator) {
                        return 1;
                    }

                    isWasScopeIdSeparator = 1;
                } break;

                default: {
                    if (isWasScopeIdSeparator) {
                        scope_buffer[scopeIt] = *data; // Copy char.
                        scopeIt++;
                    } else {
                        buffer[it] = *data; // Copy char.
                        it++;
                    }
                } break;
            }

            data += 1;
        }

        // If was open bracket but close bracket is missing.
        if (isWasOpenBracket && !isWasCloseBracket) {
            return 1;
        }

        // Parse scopeId.
        if (strlen(&scope_buffer[0]) > 0) {
            if (sscanf(&scope_buffer[0], "%lu", &scopeId) != 1) {
                return 1;
            }

            pAddress->scopeId = scopeId;
        }

        struct sockaddr_in6 sa;

        // TODO: Solve it!
        #ifdef __MINGW32__
        printf("inet_pton is unsupported by MINGW32!\n");
        return 1;
        #else
        inet_pton(AF_INET6, &buffer[0], &(sa.sin6_addr));
        #endif

        memcpy(pAddress->data.v6, &sa.sin6_addr, 16);
    }


    return 0;
}

int CBLRNET_Address_ToString(cblrnetaddress_t *pAddress, c8 *pBuffer, u8 bufferLength)
{
    if (!pAddress) {
        return 1;
    }

    memset(pBuffer, 0, bufferLength);

    // IPv6
    if (pAddress->protocol == CBLRNET_LAYER_IPv6) {

        pBuffer[0] = '[';

        for (int i = 0; i < 16; i += 2)
        {
            u16 section = (u16)((pAddress->data.v6[i] << 8) | (pAddress->data.v6[i + 1]));

            _snprintf(pBuffer, bufferLength, "%s%04x", pBuffer, section);

            if (i < 14) {
                _snprintf(pBuffer, bufferLength, "%s:", pBuffer);
            }
        }

        if (pAddress->scopeId != 0) {
            _snprintf(pBuffer, bufferLength, "%s%%%d", pBuffer, pAddress->scopeId);
        }

        _snprintf(pBuffer, bufferLength, "%s]:%d", pBuffer, pAddress->port);

        return 0;

    // IPv4
    } else {
        u8 a, b, c, d;

        a = (pAddress->data.v4 & 0xFF000000) >> 24;
        b = (pAddress->data.v4 & 0x00FF0000) >> 16;
        c = (pAddress->data.v4 & 0x0000FF00) >> 8;
        d = (pAddress->data.v4 & 0x000000FF);

        _snprintf(pBuffer, bufferLength, "%d.%d.%d.%d:%d", a, b, c, d, pAddress->port);

        return 0;
    }

    return 1;
}

int CBLRNET_Address_Resolve(cblrnetaddress_t *pAddress, const c8 *pName)
{
    if (!pAddress) {
        return 1;
    }

    // If received address is null or string which starts with zero.
    if (!pName || pName[0] == 0) {
        return 1;
    }

    struct addrinfo *resolved, hints;
    memset(&hints, 0, sizeof(hints));

    // Setup hints.
    hints.ai_socktype = 0;
    hints.ai_protocol = 0;
    hints.ai_flags = 0;
    hints.ai_family = AF_UNSPEC;

    int result = getaddrinfo(pName, NULL, &hints, &resolved);

    // If we have error, then exit from the function!
    if (result != 0) {
        return 1;
    }

    // IPv6
    if (resolved->ai_addr->sa_family == AF_INET6) {
        struct sockaddr_in6 *pIn = (struct sockaddr_in6 *)resolved->ai_addr;

        pAddress->protocol = CBLRNET_LAYER_IPv6;
        pAddress->port = pIn->sin6_port;
        pAddress->scopeId = pIn->sin6_scope_id;
        memcpy(&pAddress->data.v6[0], pIn->sin6_addr.s6_addr, 16);

    // IPv4
    } else {
        struct sockaddr_in *pAddrIn = (struct sockaddr_in *)resolved->ai_addr;

        pAddress->protocol = CBLRNET_LAYER_IPv4;
        pAddress->port = pAddrIn->sin_port;
        pAddress->scopeId = 0;
        pAddress->data.v4 = ntohl(pAddrIn->sin_addr.s_addr);
    }

    freeaddrinfo(resolved);
    return 0;
}
