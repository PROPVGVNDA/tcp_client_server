#ifndef SERVER_H
#define SERVER_H

#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <iostream>
#include <cstring>

const uint16_t kMAX_CLIENTS = 50;
const uint32_t kBUFFER_SIZE = 1024;

class Server
{
public:
    Server(uint16_t port);
    ~Server();
private:
    void main_loop();
    void listen_on_master();
    int update_descriptors();
    void add_socket();
    void process_socket(uint16_t index);
    void close_socket(uint16_t index);
    void send_message(int& socket_descriptor, const char* msg) const;
private:
    int server_socket_;
    int clients_sockets_[kMAX_CLIENTS];
    sockaddr_in address_;
    fd_set socket_fds_;
    uint64_t addrlen_;
    
};

#endif