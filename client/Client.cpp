#include "Client.h"

Client::Client(const char* ip, uint16_t port)
    : client_socket_{0}
{

    char buffer[kBUFFER_SIZE];

    client_socket_ = socket(AF_INET, SOCK_STREAM, 0);

    address_.sin_family = AF_INET;
    address_.sin_port = htons(port);

    if (inet_pton(AF_INET, ip, &address_.sin_addr) <= 0)
    {
        std::cerr << "Could not identify specified IP address\n";
        exit(EXIT_FAILURE);
    }

    if (connect(client_socket_, (sockaddr*)&address_, sizeof(address_)) < 0)
    {
        std::cerr << "Could not connect\n";
        exit(EXIT_FAILURE);
    }
}

Client::~Client()
{
    close(client_socket_);
}

bool Client::SendMessage(const char* msg)
{
    if (send(client_socket_, msg, strlen(msg), 0) != strlen(msg))
    {
        std::cout << "Failed\n";
        return false;
    }
    return true;
}

std::string Client::ReadMessage()
{
    char buffer[kBUFFER_SIZE];
    int valread = read(client_socket_, buffer, kBUFFER_SIZE);
    if (valread <= 0)
    {
        std::cout << "Client closed connection\n";
        exit(EXIT_FAILURE);
    }
    buffer[valread] = '\0';
    return std::string(buffer, valread);
}