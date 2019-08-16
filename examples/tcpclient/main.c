#include <stdio.h>
#include <conio.h>

#ifdef _WIN32
#include <windows.h>
#endif

#include <cablernet.h>

#define EXAMPLE_NAME "tcpclient"
#define STR_DEST_ADDR "127.0.0.1:25602"   // [Tin] WARN: Set external server address!


cblrnetsocket_t sock;

// Clear sockets and network API.
void on_close()
{
    // Socket need to be closed anyway.
    cblrnet_socket_close(&sock);
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
    // [Tin] NOTE: Check this pls.
    signal(SIGINT, HandlerRoutine);
    signal(SIGBREAK, HandlerRoutine);
#endif

    cblrnetaddress_t destAddr;

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

    cblrnet_address_fromstring(&destAddr, STR_DEST_ADDR);
    printf("Server address: %s\n", STR_DEST_ADDR);

    // Error message buffer
    c8 msg[255];
    memset(msg, 0, sizeof (msg));

    // The main loop.
    int i = 0;
    while (i++ < 1000)
    {
        if(!cblrnet_socket_connect(&sock, &destAddr)){
            cblrnet_get_err_message(cblrnet_socket_errno(), msg, sizeof(msg));
            printf("Server not responding: %s", msg);
            continue;
        } else {
            // [Tin] NOTE: If the socket type is DGRAM, it just change destination address.
            printf("Server connected!\n");
            break;
        }
    }

    // Close socket and clear API.
    on_close();
    printf("Press any key to continue...");
    _getch();

    return 0;
}
