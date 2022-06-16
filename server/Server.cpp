#include "Server.h"

Server::Server(uint16_t port)
    : server_socket_{0}, clients_sockets_{0}, addrlen_(0)
{
    int opt = true;

    server_socket_ = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket_ == 0)
    {
        perror("Failed to create master socket\n");
        exit(EXIT_FAILURE);
    }

    if (setsockopt(server_socket_, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt)) < 0)
    {
        perror("Failed to configure master socket\n");
        exit(EXIT_FAILURE);
    }

    address_.sin_family = AF_INET;
    address_.sin_addr.s_addr = INADDR_ANY;
    address_.sin_port = htons(port);

    listen_on_master();

    addrlen_ = sizeof(address_);

    main_loop();
}

Server::~Server()
{
    for (int i = 0; i < kMAX_CLIENTS; i++)
        close_socket(i);
}

void Server::main_loop()
{
    int max_sd; // socket descriptors
    char buffer[kBUFFER_SIZE];

    while (true)
    {

        max_sd = update_descriptors();

        // max_sd + 1 to include master socket
        int activity = select(max_sd + 1, &socket_fds_, NULL, NULL, NULL);

        if (activity < 0 && errno != EINTR)
            perror("Select error\n");
        
        // if there's new connection, master socket will get activated and new descriptor will be opened
        if (FD_ISSET(server_socket_, &socket_fds_))
            add_socket();

        for (int i = 0; i < kMAX_CLIENTS; i++)
           process_socket(i);
    }
}

int Server::update_descriptors()
{
    int max_sd = server_socket_;

    FD_ZERO(&socket_fds_);
    FD_SET(server_socket_, &socket_fds_);

    for (int i = 0; i < kMAX_CLIENTS; i++)
    {
        int sd = clients_sockets_[i];
        if (sd > 0) FD_SET(sd, &socket_fds_);
        if (sd > max_sd) max_sd = sd;
    }

    return max_sd;
}

void Server::listen_on_master()
{
    // assign address to master socket
    if (bind(server_socket_, (sockaddr*)&address_, sizeof(address_)) < 0)
    {
        perror("Failed to bind master socket\n");
        exit(EXIT_FAILURE);
    }

    // listen for client sockets trying to connect
    if (listen(server_socket_, 3) < 0)
    {
        perror("Failed to listen on master socket\n");
        exit(EXIT_FAILURE);
    }
}

void update_descriptors();

void Server::add_socket()
{
    int new_socket;
    if ((new_socket = accept(server_socket_, (sockaddr*)&address_, (socklen_t*)&addrlen_)) < 0)
    {
        perror("Failed to accept new connection\n");
        return;
    }

    for (int i = 0; i < kMAX_CLIENTS; i++)
    {
        if (clients_sockets_[i] == 0)
        {
            clients_sockets_[i] = new_socket;
            break;
        }
    }   
}

void Server::process_socket(uint16_t index)
{
    int socket_descriptor = clients_sockets_[index];
    int valread = 0;
    char buffer[kBUFFER_SIZE];

    // check if active
    if (!FD_ISSET(socket_descriptor, &socket_fds_)) return;

    if ((valread = read(socket_descriptor, buffer, kBUFFER_SIZE)) == 0)
        close_socket(index);
    else
    {
        buffer[valread] = '\0';

        if (!strncmp(buffer, "hello", strlen(buffer))) send_message(socket_descriptor, "world");
        else send_message(socket_descriptor, buffer);
    }
}

void Server::close_socket(uint16_t index)
{
    if (clients_sockets_[index] == 0) return;

    close(clients_sockets_[index]);
    clients_sockets_[index] = 0;
}

void Server::send_message(int& socket_descriptor, const char* msg) const
{
    send(socket_descriptor, msg, strlen(msg), 0);
}
