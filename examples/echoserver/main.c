#include <stdio.h>

#include <cablernet.h>

#define EXAMPLE_NAME "echoserver"
#define BUFFER_SIZE 1472

int main()
{
    cblrnetsocket_t sock;
    cblrnetaddress_t addr, senderAddr;
    u8 recvBuffer[BUFFER_SIZE];

    printf("Example: %s\n", EXAMPLE_NAME);

    if (cblrnet_startup() != 0) {
        printf("Failed to initialize sockets subsystem!\n");
        return 1;
    }

    cblrnet_address_fromstring(&addr, "0.0.0.0:25600"); // Use default network interface.

    // Open UDP socket.
    if (cblrnet_socket_open(&sock, CBLRNET_LAYER_IPv4, CBLRNET_SOCKET_TYPE_DGRAM) != 0) {
        printf("Failed to open socket!\n");
        return 1;
    }

    // Bind socket to address.
    if (cblrnet_socket_bind(&sock, &addr) != 0) {
        printf("Failed to bind socket!\n");
        return 1;
    }

    printf("Listening port: %d\n", addr.port);

    cblrnet_socket_setoption(&sock, CBLRNET_SOCKOPT_NONBLOCKING, 1);

    memset(&recvBuffer[0], 0, BUFFER_SIZE);

    c8 senderAddrString[22];

    while(1)
    {
        // If no data then continue waiting.
        if (!cblrnet_socket_waitdata(&sock, 200)) {
            continue;
        }

        // While we have data continue getting.
        while (1)
        {

            int receivedBytes = cblrnet_socket_recvfrom(&sock, &senderAddr, &recvBuffer[0], BUFFER_SIZE);

            if (receivedBytes <= 0) {
                break;
            }

            memset(&senderAddrString[0], 0, 22);
            cblrnet_address_tostring(&senderAddr, &senderAddrString[0], 16);

            recvBuffer[receivedBytes] = 0x0; // Terminate before printout.
            printf("[%s] '%s'\n", &senderAddrString[0], &recvBuffer[0]);

            cblrnet_socket_sendto(&sock, &senderAddr, &recvBuffer[0], receivedBytes);
        }
    }

    return 0;
}
