#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <iostream>
#include <cstdio>

int main()
{
    // Create a socket
    int server_fd, new_socket;
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0)
    {
        std::cerr << "Error creating socket" << std::endl;
        return -1;
    }

    // Set up server address
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(4242);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    // Bind the socket to the address and port
    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        std::cerr << "Error binding socket" << std::endl;
        return -1;
    }

    // Listen for incoming connections
    if (listen(server_fd, 3) < 0)
    {
        std::cerr << "Error listening" << std::endl;
        return -1;
    }

    // Accept incoming connections
    while (true)
    {
        new_socket = accept(server_fd, NULL, NULL);
        if (new_socket < 0)
        {
            std::cerr << "Error accepting connection" << std::endl;
            return -1;
        }

        // Handle the connection
        char buffer[1024];
        int bytes_received = recv(new_socket, buffer, 1024, 0);
        if (bytes_received < 0)
        {
            std::cerr << "Error receiving data" << std::endl;
            return -1;
        }

        // Parse the HTTP request
        char *request_line = strtok(buffer, "\r\n");
        char *method = strtok(request_line, " ");
        char *url = strtok(NULL, " ");
        char *http_version = strtok(NULL, "\r\n");

        // Serve the requested resource
        if (strcmp(method, "GET") == 0)
        {
            // Open the requested file
            FILE *file = fopen(url, "rb");
            if (file == NULL)
            {
                std::cerr << "Error opening file" << std::endl;
                return -1;
            }

            // Send the file contents back to the client
            char file_buffer[1024];
            while (fread(file_buffer, 1, 1024, file) > 0)
            {
                send(new_socket, file_buffer, 1024, 0);
            }

            // Close the file
            fclose(file);
        }

        // Close the socket
        close(new_socket);
    }

    return 0;
}
