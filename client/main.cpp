#include "Client.h"
#include <string>

int main()
{
    Client client("127.0.0.1", 54000);
    while (true)
    {
        std::cout << "Enter message to send to server: ";

        std::string buffer;
        std::getline(std::cin, buffer);
        
        if (buffer == "exit") break;
        if (!client.SendMessage(buffer.c_str()))
        {
            std::cout << "Closing client\n";
            exit(EXIT_SUCCESS);
        }
        buffer = "From server: " + client.ReadMessage();
        std::cout << buffer << std::endl;
    }
}