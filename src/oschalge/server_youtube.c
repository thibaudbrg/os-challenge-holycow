#include <stdio.h>
#include<stdlib.h>

#include <sys/types.h>
//#include <sys/socket.h>

//#include <netinet.in.h>
#include <winsock.h>
#include <io.h>


//===================================TCP Server===================================//

int main(void) {

    char server_message[256] = "You have reached the server!\n\n";

    // Create the server_socket
    int server_socket;
    server_socket = socket(AF_INET, SOCK_STREAM, 0);

    // Define the address structure
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(9002);
    server_address.sin_addr.s_addr = INADDR_ANY; // 0.0.0.0 or INADDR_ANY for local machine

    // Bind the socket to our specified IP and port
    bind(server_socket, (struct sockaddr *) &server_address, sizeof(server_address));


    listen(server_socket, 5); // Backlog = How many connections can be waiting while listening

    int client_socket;
    client_socket = accept(server_socket, NULL, NULL);

    // We send the message
    send(client_socket, server_message, sizeof(server_message), 0);


    // Close the socket
    close(server_socket);
    return 0;
}

