#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>

#pragma comment(lib, "ws2_32.lib")

#define DEFAULT_PORT "27015"
#define SERVER_ADDRESS "127.0.0.1"
#define BUFFER_SIZE 512

int main() {
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        printf("WSAStartup failed with error: %d\n", result);
        return 1;
    }

    struct addrinfo* addrResult = NULL;
    struct addrinfo hints;

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    result = getaddrinfo(SERVER_ADDRESS, DEFAULT_PORT, &hints, &addrResult);
    if (result != 0) {
        printf("getaddrinfo failed with error: %d\n", result);
        WSACleanup();
        return 1;
    }

    SOCKET connectSocket = INVALID_SOCKET;

    connectSocket = socket(addrResult->ai_family, addrResult->ai_socktype, addrResult->ai_protocol);
    if (connectSocket == INVALID_SOCKET) {
        printf("socket failed with error: %d\n", WSAGetLastError());
        freeaddrinfo(addrResult);
        WSACleanup();
        return 1;
    }

    result = connect(connectSocket, addrResult->ai_addr, (int)addrResult->ai_addrlen);
    if (result == SOCKET_ERROR) {
        printf("connect failed with error: %d\n", WSAGetLastError());
        closesocket(connectSocket);
        WSACleanup();
        return 1;
    }

    freeaddrinfo(addrResult);

    char buffer[BUFFER_SIZE];
    int bufferLen;

    while (1) {
        printf("Client: ");
        fgets(buffer, BUFFER_SIZE, stdin);
        bufferLen = strlen(buffer);

        if (buffer[bufferLen - 1] == '\n') {
            buffer[bufferLen - 1] = '\0';
            bufferLen--;
        }

        result = send(connectSocket, buffer, bufferLen, 0);
        if (result > 0) {
            result = recv(connectSocket, buffer, BUFFER_SIZE, 0);
            if (result > 0) {
                printf("Server: %.*s\n", result, buffer);
            }
            else {
                break;
            }
        }
        else if (result == 0) {
            printf("Connection closed by server.\n");
            break;
        }
        else {
            printf("send() failed with error: %d\n", WSAGetLastError());
            closesocket(connectSocket);
            WSACleanup();
            return 1;
        }
    }

    closesocket(connectSocket);
    WSACleanup();

    return 0;
}
