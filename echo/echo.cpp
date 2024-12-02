#include <iostream>
#include <string>
#include <cstring>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#define ADDR "172.16.40.1"
#define PORT 7
#define SIZE 1024

int createSocket() {
    return socket(AF_INET, SOCK_STREAM, 0);
}

void setupAddress(sockaddr_in &address) {
    address.sin_family = AF_INET;
    address.sin_port = htons(PORT);
    if (inet_pton(AF_INET, ADDR, &address.sin_addr) <= 0) {
        throw std::runtime_error("Ошибка установки адреса сервера");
    }
}

void connectToServer(int socketFD, sockaddr_in &address) {
    if (connect(socketFD, (struct sockaddr*)&address, sizeof(address)) < 0) {
        throw std::runtime_error("Ошибка подключения к серверу");
    }
}

void sendMessage(int socketFD, const std::string &message) {
    if (send(socketFD, message.c_str(), message.length(), 0) < 0) {
        throw std::runtime_error("Ошибка отправки сообщения");
    }
}

std::string receiveResponse(int socketFD) {
    char buffer[SIZE] = {0};
    if (recv(socketFD, buffer, SIZE, 0) < 0) {
        throw std::runtime_error("Ошибка получения ответа от сервера");
    }
    return std::string(buffer);
}

void cleanup(int socketFD) {
    close(socketFD);
}

int main() {
    try {
        int socketFD = createSocket();
        if (socketFD == -1) {
            std::cerr << "Ошибка создания сокета" << std::endl;
            return 1;
        }

        sockaddr_in serverAddress{};
        setupAddress(serverAddress);

        connectToServer(socketFD, serverAddress);

        std::string message = "Hello, server!";
        sendMessage(socketFD, message);

        std::string response = receiveResponse(socketFD);
        std::cout << "Ответ от сервера: " << response << std::endl;

        cleanup(socketFD);
    } catch (const std::runtime_error &e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    return 0;
}
