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

//===================================TCP Client===================================//

int main(void) {

    // Create a socket
    int network_socket;
    network_socket = socket(AF_INET, SOCK_STREAM, 0);

    // Specify an address for the socket
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(9002); // takes care of converting the port number to the right bit format

    server_address.sin_addr.s_addr = INADDR_ANY; // 0.0.0.0 or INADDR_ANY for local machine

    int connection_status = connect(network_socket, (struct socketaddr *) &server_address, sizeof(server_address));
    if (connection_status != 0) {
        fprintf(stderr, "There was an error making a connection to the remote socket. \n\n");
        return -1;
    }

    // Receive data from the server
    char server_response[256];
    recv(network_socket, &server_response, sizeof(server_response), 0);

    // Print out the server's response
    printf("The server sent the data: %s\n", server_response);

    close(network_socket);
    return 0;
}

