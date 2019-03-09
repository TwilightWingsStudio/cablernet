#include <stdio.h>
#include <string.h>

#include <cablernet.h>

#define EXAMPLE_NAME "tcpclient"

int main()
{
    cblrnetaddress_t destAddr;
    cblrnetsocket_t sock;

    printf("Example: %s\n", EXAMPLE_NAME);

    if (cblrnet_startup() != 0) {
        printf("Failed to initialize sockets subsystem!\n");
        return 1;
    }

    // Open the socket.
    if (cblrnet_socket_open(&sock, CBLRNET_LAYER_IPv4, CBLRNET_SOCKET_TYPE_STREAM) != 0) {
        printf("Failed to open socket!\n");
        cblrnet_shutdown();
        return 1;
    }

    // WARN: Set external server address!
    cblrnet_address_fromstring(&destAddr, "127.0.0.1:25602");

    // The main loop.
    while (1)
    {
        if(!cblrnet_socket_connect(&sock, &destAddr)){
            printf("Server not responding!\n");
            continue;
        } else {
            // NOTE: If the socket type is DGRAM, it just change destination address.
            printf("Server connected!\n");
            break;
        }
    }

    cblrnet_shutdown();

    return 0;
}
