#include <stdio.h>
#include <stdlib.h>

#if defined(_WIN32) || defined(_WIN64) || defined(WIN32)
/* Windows headers */
#include <winsock2.h>
#include <windows.h>
#include <io.h>


#else /* or check here for unix / linux & fail if OS unrecognized */
/* POSIX headers */
#include <netdb.h>
#include <poll.h>
#include <sys/resource.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <sys/un.h>
#include <unistd.h>

#endif /* _WIN32 */

/* shared headers */
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>


//===================================TCP Server===================================//

int main(int argc, char**argv) {

    if (argc<2) printf('%s',"Not enough argument, please specify the port number.");

    char server_message[256] = "You have reached the server!\n\n";

    // Create the server_socket
    int server_socket;
    server_socket = socket(AF_INET, SOCK_STREAM, 0);

    // Define the address structure
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(*argv[1]);
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

