#include <stdio.h>
#include <conio.h>

#ifdef _WIN32
#include <windows.h>
#endif

#include <cablernet.h>

#define EXAMPLE_NAME "tcpechoserver"
#define BUFFER_SIZE 1472
#define MAX_CONNECTIONS 1
#define STR_ADDR "0.0.0.0:25602"    // Using default network interface.


cblrnetsocket_t sock, senderSock;

// Clear sockets and network API.
void on_close() {
    // Sockets need to be closed anyway.
    cblrnet_socket_close(&sock);
    cblrnet_socket_close(&senderSock);
    cblrnet_shutdown();

    printf("CLOSED\n");
}

#ifdef _WIN32
    // Application close handler.
    // [Tin] NOTE: You can handle it your way.
    BOOL WINAPI HandlerRoutine(_In_ DWORD dwCtrlType)
    {
        if(dwCtrlType == CTRL_CLOSE_EVENT || dwCtrlType == CTRL_C_EVENT){
            on_close();
            return 1;
        }
    }
#else
    void HandlerRoutine(int sig)
    {
        on_close();
    }
#endif

int main()
{
#ifdef _WIN32
    // Set close handler.
    SetConsoleCtrlHandler(HandlerRoutine, TRUE);
#else
    // [Tin] NOTE: Cheak it pls.
    signal(SIGINT, HandlerRoutine);
    signal(SIGBREAK, HandlerRoutine);
#endif

    cblrnetaddress_t addr, senderAddr;
    u8 recvBuffer[BUFFER_SIZE];

    printf("Example: %s\n", EXAMPLE_NAME);

    if (cblrnet_startup() != 0) {
        printf("Failed to initialize sockets subsystem!\n");
        return 1;
    }

    cblrnet_address_fromstring(&addr, STR_ADDR);
    printf("Server address: %s\n", STR_ADDR);

    // Open TCP socket.
    if (cblrnet_socket_open(&sock, CBLRNET_LAYER_IPv4, CBLRNET_SOCKET_TYPE_STREAM) != 0) {
        printf("Failed to open socket!\n");
        return 1;
    }

    // Bind socket to address.
    if (cblrnet_socket_bind(&sock, &addr) != 0) {
        printf("Failed to bind socket!\n");
        return 1;
    }

    if(cblrnet_socket_listen(&sock, MAX_CONNECTIONS) != 0) {
        printf("Failed to listen socket!\n");
        return 1;
    }

    c8 addrString[22];
    memset(&addrString[0], 0, 22);
    cblrnet_address_tostring(&addr, &addrString[0], 16);

    printf("Listening: %s\n", addrString);

    memset(&recvBuffer[0], 0, BUFFER_SIZE);

    c8 senderAddrString[22];

    while(1)
    {
        // Wait for connection
        if(cblrnet_socket_accept(&sock, &senderAddr, &senderSock)){
            continue;
        }

        memset(&senderAddrString[0], 0, 22);
        cblrnet_address_tostring(&senderAddr, &senderAddrString[0], 16);

        printf("Acepted: %s\n", &senderAddrString[0]);

        while(1)
        {
            // If no data then continue waiting.
            if (!cblrnet_socket_waitdata(&senderSock, 200)) {
                continue;
            }

            // While we have data continue getting.
            while (1)
            {
                int receivedBytes = cblrnet_socket_recvfrom(&senderSock, NULL, &recvBuffer[0], BUFFER_SIZE);

                if (receivedBytes <= 0) {
                    break;
                }

                recvBuffer[receivedBytes] = 0x0; // Terminate before printout.
                printf("[%s] '%s'\n", &senderAddrString[0], &recvBuffer[0]);

                cblrnet_socket_sendto(&senderSock, NULL, &recvBuffer[0], receivedBytes);
            }
            break;
        }
    }

    // Close socket and clear API.
    on_close();

    return 0;
}
