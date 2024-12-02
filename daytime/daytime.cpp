#include <iostream>
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define BUFFER_SIZE 1024

int initializeSocket() {
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        std::cerr << "Не удалось создать сокет." << std::endl;
        return -1;
    }
    return sock;
}

void configureServerAddress(struct sockaddr_in &address) {
    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_port = htons(13); // Порт службы Daytime
    address.sin_addr.s_addr = inet_addr("172.16.40.1"); // IP адрес сервера
}

bool sendRequest(int socket, struct sockaddr_in &address) {
    if (sendto(socket, nullptr, 0, 0, (struct sockaddr*)&address, sizeof(address)) < 0) {
        std::cerr << "Ошибка при отправке запроса." << std::endl;
        return false;
    }
    return true;
}

std::string receiveResponse(int socket) {
    char buffer[BUFFER_SIZE];
    memset(buffer, 0, BUFFER_SIZE);
    
    ssize_t receivedBytes = recvfrom(socket, buffer, BUFFER_SIZE - 1, 0, nullptr, nullptr);
    if (receivedBytes < 0) {
        std::cerr << "Ошибка при получении ответа." << std::endl;
        return "";
    }

    return std::string(buffer, receivedBytes);
}

int main() {
    struct sockaddr_in serverAddress;
    
    int socketFD = initializeSocket();
    if (socketFD < 0) return 1;

    configureServerAddress(serverAddress);

    if (!sendRequest(socketFD, serverAddress)) {
        close(socketFD);
        return 1;
    }

    std::string response = receiveResponse(socketFD);
    if (!response.empty()) {
        std::cout << "Ответ от сервера: " << response << std::endl;
    }

    close(socketFD);
    
    return 0;
}
