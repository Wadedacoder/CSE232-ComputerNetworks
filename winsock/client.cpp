#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
// #include <sys/types.h>
// #include <unistd.h>
// #include <sys/socket.h>
// #include <netdb.h>
// #include <arpa/inet.h>
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

    // Create a buffer
    char buffer[sizeof(XrPosef)];
    int recvbuflen = sizeof(buffer);
    // Initialize Winsock
    WSADATA wsaData;
    int iResult = WSAStartup(MAKEWORD(2,2), &wsaData); // MAKEWORD(2,2) = 0x0202
    if (iResult != 0) {
        printf("WSAStartup failed: %d\n", iResult);
        return 1;
    }

    // Create a SOCKET to connect to server
    SOCKET ConnectSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (ConnectSocket == INVALID_SOCKET) {
        printf("socket failed: %ld\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }
    std::cout << "ConnectSocket: " << ConnectSocket << std::endl;

    // Connect to server.
    sockaddr_in clientService;
    clientService.sin_family = AF_INET;
    clientService.sin_addr.s_addr = inet_addr("127.0.0.1");
    clientService.sin_port = htons(27015);

    iResult = connect(ConnectSocket, (SOCKADDR *) & clientService, sizeof (clientService));
    if (iResult == SOCKET_ERROR) {
        printf("connect failed: %ld\n", WSAGetLastError());
        closesocket(ConnectSocket);
        WSACleanup();
        return 1;
    }
    std::cout << "iResult: " << iResult << std::endl;

    // Receive data for Posef

    iResult = recv(ConnectSocket, buffer, recvbuflen, 0);
    if (iResult > 0)
        printf("Bytes received: %d\n", iResult);
    else if (iResult == 0)
        printf("Connection closed\n");
    else
        printf("recv failed: %d\n", WSAGetLastError());


    // Convert buffer to Posef
    memcpy(&pose, buffer, sizeof(XrPosef));

    // Print Posef
    std::cout << "pose.orientation.x: " << pose.orientation.x << std::endl;
    std::cout << "pose.orientation.y: " << pose.orientation.y << std::endl;
    std::cout << "pose.orientation.z: " << pose.orientation.z << std::endl;
    std::cout << "pose.orientation.w: " << pose.orientation.w << std::endl;
    std::cout << "pose.position.x: " << pose.position.x << std::endl;
    std::cout << "pose.position.y: " << pose.position.y << std::endl;
    std::cout << "pose.position.z: " << pose.position.z << std::endl;

    pose.position.x = 1.0f;
    pose.position.y = 2.0f;

    // Convert Posef to buffer
    memcpy(buffer, &pose, sizeof(XrPosef));

    // Send buffer to server
    iResult = send(ConnectSocket, buffer, sizeof(buffer), 0);
    if (iResult == SOCKET_ERROR) {
        printf("send failed: %d\n", WSAGetLastError());
        closesocket(ConnectSocket);
        WSACleanup();
        return 1;
    }


    return 0;

}