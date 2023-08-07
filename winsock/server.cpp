#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

typedef struct XrVector3f {
    float    x;
    float    y;
    float    z;
} XrVector3f;

typedef struct XrQuaternionf {
    float    x;
    float    y;
    float    z;
    float    w;
} XrQuaternionf;

typedef struct XrPosef {
    XrQuaternionf    orientation;
    XrVector3f       position;
} XrPosef;

int main(){
    // Create a Posef
    XrPosef pose;
    pose.orientation.x = 0.0f;
    pose.orientation.y = 0.2323f;
    pose.orientation.z = 0.0f;
    pose.orientation.w = 1.0f;
    pose.position.x = 0.0f;
    pose.position.y = 0.0f;
    pose.position.z = 0.0f;
    
    // Create a buffer
    char buffer[sizeof(XrPosef)];
    memcpy(buffer, &pose, sizeof(XrPosef));
    
    std::cout << "buffer: " << buffer << std::endl;
    // Initialize Winsock

    WSADATA wsaData;
    int iResult = WSAStartup(MAKEWORD(2,2), &wsaData); // MAKEWORD(2,2) = 0x0202
    if (iResult != 0) {
        printf("WSAStartup failed: %d\n", iResult);
        return 1;
    }
    std::cout << "WSAStartup: " << iResult << std::endl;
    // Create a SOCKET for connecting to server
    SOCKET ListenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (ListenSocket == INVALID_SOCKET) {
        printf("socket failed: %ld\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }
    std::cout << "ListenSocket: " << ListenSocket << std::endl;
    // Setup the TCP listening socket with local address
    sockaddr_in service;
    service.sin_family = AF_INET;
    service.sin_addr.s_addr = inet_addr("127.0.0.1");
    service.sin_port = htons(27015);
    if (bind(ListenSocket, (SOCKADDR*)&service, sizeof(service)) == SOCKET_ERROR) {
        printf("bind failed: %ld\n", WSAGetLastError());
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }
    std::cout << "bind: " << ListenSocket << std::endl;
    // Listen for incoming connection requests
    if (listen(ListenSocket, 1) == SOCKET_ERROR) {
        printf("listen failed: %ld\n", WSAGetLastError());
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }
    std::cout << "listen: " << ListenSocket << std::endl;
    // Accept the connection
    SOCKET ClientSocket = accept(ListenSocket, NULL, NULL);
    if (ClientSocket == INVALID_SOCKET) {
        printf("accept failed: %ld\n", WSAGetLastError());
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }
    std::cout << "ClientSocket: " << ClientSocket << std::endl;
    // send the buffer
    iResult = send(ClientSocket, buffer, sizeof(XrPosef), 0);
    if (iResult == SOCKET_ERROR) {
        printf("send failed: %ld\n", WSAGetLastError());
        closesocket(ClientSocket);
        WSACleanup();
        return 1;
    }
    std::cout << "iResult: " << iResult << std::endl;

    // Receive until the peer shuts down the connection
        iResult = recv(ClientSocket, buffer, sizeof(XrPosef), 0);
        if (iResult > 0) {
            printf("Bytes received: %d\n", iResult);
            std::cout << "buffer: " << buffer << std::endl;
        } else if (iResult == 0)
            printf("Connection closed\n");
        else
            printf("recv failed: %d\n", WSAGetLastError());

    // shutdown the connection since we're done
    iResult = shutdown(ClientSocket, SD_SEND);
    if (iResult == SOCKET_ERROR) {
        printf("shutdown failed: %ld\n", WSAGetLastError());
        closesocket(ClientSocket);
        WSACleanup();
        return 1;
    }

    // Copy buffer to Posef
    memcpy(&pose, buffer, sizeof(XrPosef));

    // Print Posef
    std::cout << "pose.orientation.x: " << pose.orientation.x << std::endl;
    std::cout << "pose.orientation.y: " << pose.orientation.y << std::endl;
    std::cout << "pose.orientation.z: " << pose.orientation.z << std::endl;
    std::cout << "pose.orientation.w: " << pose.orientation.w << std::endl;
    std::cout << "pose.position.x: " << pose.position.x << std::endl;
    std::cout << "pose.position.y: " << pose.position.y << std::endl;
    std::cout << "pose.position.z: " << pose.position.z << std::endl;

    // cleanup
    closesocket(ClientSocket);
    WSACleanup();

}