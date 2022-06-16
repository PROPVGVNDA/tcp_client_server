#ifndef CLIENT_H
#define CLIENT_H

#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>
#include <cstring>

const uint32_t kBUFFER_SIZE = 1024;

class Client
{
public:
    Client(const char* ip, uint16_t port);
    ~Client();
    bool SendMessage(const char* msg);
    std::string ReadMessage();
private:
    int client_socket_;
    sockaddr_in address_;
};

#endif