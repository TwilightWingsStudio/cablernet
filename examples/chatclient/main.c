#include <stdio.h>
#include <string.h>

#include <cablernet.h>

#define EXAMPLE_NAME "chatclient"
#define RECV_BUFFER_SIZE 1472
#define INPUT_BUFFER_SIZE 160

int main()
{
    c8 inputBuffer[INPUT_BUFFER_SIZE];
    u8 recvBuffer[RECV_BUFFER_SIZE];
    cblrnetaddress_t senderAddr;
    cblrnetaddress_t destAddr;
    cblrnetsocket_t sock;

    printf("Example: %s\n", EXAMPLE_NAME);

    if (cblrnet_startup() != 0) {
        printf("Failed to initialize sockets subsystem!\n");
        return 1;
    }

    // Open the socket.
    if (cblrnet_socket_open(&sock, CBLRNET_LAYER_IPv4, CBLRNET_SOCKET_TYPE_DGRAM) != 0) {
        printf("Failed to open socket!\n");
        cblrnet_shutdown();
        return 1;
    }

    cblrnet_address_fromstring(&destAddr, "0.0.0.0:25600");

    // The main loop.
    while (1)
    {
        memset(&inputBuffer[0], 0, INPUT_BUFFER_SIZE);
        scanf("%s", inputBuffer);

        if (inputBuffer[0] == 'e') {
            break;
        }

        cblrnet_socket_send(&sock, &destAddr, &inputBuffer[0], strlen(&inputBuffer[0]));

        // Await data for 1 second.
        if (!cblrnet_socket_waitdata(&sock, 1000)) {
            printf("Server not responding!\n");
            continue;
        }

        int receivedBytes = cblrnet_socket_read(&sock, &senderAddr, &recvBuffer[0], RECV_BUFFER_SIZE);

        recvBuffer[receivedBytes] = 0x0;

        printf("Server: %s\n", &recvBuffer[0]);
    }

    cblrnet_shutdown();

    return 0;
}
