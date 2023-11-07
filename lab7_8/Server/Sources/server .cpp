#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>

#pragma comment(lib, "ws2_32.lib")

#define DEFAULT_PORT "27015"
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
    hints.ai_flags = AI_PASSIVE;

    result = getaddrinfo(NULL, DEFAULT_PORT, &hints, &addrResult);
    if (result != 0) {
        printf("getaddrinfo failed with error: %d\n", result);
        WSACleanup();
        return 1;
    }

    SOCKET listenSocket = INVALID_SOCKET;

    listenSocket = socket(addrResult->ai_family, addrResult->ai_socktype, addrResult->ai_protocol);
    if (listenSocket == INVALID_SOCKET) {
        printf("socket failed with error: %d\n", WSAGetLastError());
        freeaddrinfo(addrResult);
        WSACleanup();
        return 1;
    }

    result = bind(listenSocket, addrResult->ai_addr, (int)addrResult->ai_addrlen);
    if (result == SOCKET_ERROR) {
        printf("bind failed with error: %d\n", WSAGetLastError());
        freeaddrinfo(addrResult);
        closesocket(listenSocket);
        WSACleanup();
        return 1;
    }

    freeaddrinfo(addrResult);

    result = listen(listenSocket, SOMAXCONN);
    if (result == SOCKET_ERROR) {
        printf("listen failed with error: %d\n", WSAGetLastError());
        closesocket(listenSocket);
        WSACleanup();
        return 1;
    }

    printf("Waiting for client connection...\n");

    SOCKET clientSocket;
    clientSocket = accept(listenSocket, NULL, NULL);
    if (clientSocket == INVALID_SOCKET) {
        printf("accept failed with error: %d\n", WSAGetLastError());
        closesocket(listenSocket);
        WSACleanup();
        return 1;
    }

    closesocket(listenSocket);

    char buffer[BUFFER_SIZE];

    char response[BUFFER_SIZE];

    while (1) {
    result = recv(clientSocket, buffer, BUFFER_SIZE, 0);
    if (result > 0) {
        printf("Client: %.*s\n", result, buffer);

        printf("Server: ");
        fgets(response, BUFFER_SIZE, stdin);
        int responseLen = strlen(response);

        if (response[responseLen - 1] == '\n') {
            response[responseLen - 1] = '\0';
            responseLen--;
        }
        result = send(clientSocket, response, responseLen, 0);
        if (result == SOCKET_ERROR) {
            printf("send failed with error: %d\n", WSAGetLastError());
            closesocket(clientSocket);
            WSACleanup();
            return 1;
        }
    }
    else if (result == 0) {
        printf("Connection closed by client.\n");
        break;
    }
    else {
        printf("recv failed with error: %d\n", WSAGetLastError());
        closesocket(clientSocket);
        WSACleanup();
        return 1;
        }
    }

    closesocket(clientSocket);
    WSACleanup();

    return 0;
}
