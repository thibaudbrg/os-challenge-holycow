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
//imports de server youtube en haut
#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>





#include <inttypes.h>

#define MAX 360
#define REQUEST_PACKET_SIZE 48 + 1
#define SIZE_HASH 32
#define SIZE_START 8
#define SIZE_P 1

#define SA struct sockaddr


typedef struct {
    char *hash;
    char *start;
    char p;
} Request;

Request *create_empty_request() {
    Request *output = NULL;
    output  = malloc(sizeof(Request));
    if (output != NULL) {
        output->hash = calloc(SIZE_HASH, sizeof(char));
        output->start = calloc(SIZE_START, sizeof(char));
        if (output->hash != NULL && output->start != NULL) {
            output->start = 0;
            output->p = 0;
            return output;
        }
    }
    return NULL;
}


Request *getRequest(const char *all_bytes) {
    if (all_bytes != NULL) {
        printf("----TEST----\n");
        printf("Original: %s\n", all_bytes);

        Request *output = create_empty_request();

        strncpy(output->hash, all_bytes, SIZE_HASH);
        all_bytes = all_bytes + SIZE_HASH;
        printf("Hash: %s\n", output->hash);

        strncpy(output->start, all_bytes, SIZE_START); // SEG FAULT ICI. PROBLEME AVEC L'ACCES A ALL_BYTES...
        all_bytes = all_bytes + SIZE_START;
        printf("Start: %s\n", output->start);

        output->p = all_bytes[0];
        printf("P: %c\n", output->p);      
        
        
        return output;
    }
    

}

int func(int connfd)
{
    char buff[MAX];
    int n;
    // infinite loop for chat
    for (;;) {
        bzero(buff, MAX);

        // read the message from client and copy it in buffer
        size_t length = read(connfd, buff, sizeof(buff));
        if (length != REQUEST_PACKET_SIZE) {
            fprintf(stderr, "ERROR: Unable to read %d elements, read only %zu elements.\n", REQUEST_PACKET_SIZE, length);
            return -2;
        }

        // print buffer which contains the client contents
        printf("From client: %s\t To client : \n", buff);
        



        Request *request = getRequest(buff);
        if (request == NULL) {
            fprintf(stderr, "ERROR: Request is NULL.\n");
            return -3;
        }        



        n = 0;
        // copy server message in the buffer
        while ((buff[n++] = getchar()) != '\n');

        // and send that buffer to client
        write(connfd, buff, sizeof(buff));

        // if msg contains "Exit" then server exit and chat ended.
        if (strncmp("exit", buff, 4) == 0) {
            printf("Server Exit...\n");
            break;
        }

        return 0;


        free(request->hash);
        free(request);
    }
}

int main(int argc, char* argv[]){
    if(argc < 2) {
        fprintf(stderr, "Not enough argument, port number must be specified.\n");
        exit(0);
    }

    int port = atoi(argv[1]);
    int sockfd, connfd, len;
    struct sockaddr_in servaddr, cli;

    // socket create and verification
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        printf("socket creation failed...\n");
        exit(0);
    }
    else
        printf("Socket successfully created..\n");
    bzero(&servaddr, sizeof(servaddr));

    // assign IP, PORT
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(port);

    // Binding newly created socket to given IP and verification
    if ((bind(sockfd, (SA*)&servaddr, sizeof(servaddr))) != 0) {
        printf("socket bind failed...\n");
        exit(0);
    }
    else
        printf("Socket successfully binded..\n");

    // Now server is ready to listen and verification
    if ((listen(sockfd, 5)) != 0) {
        printf("Listen failed...\n");
        exit(0);
    }
    else
        printf("Server listening..\n");
    len = sizeof(cli);

    // Accept the data packet from client and verification
    connfd = accept(sockfd, (SA*)&cli, &len);
    if (connfd < 0) {
        printf("server accept failed...\n");
        exit(0);
    }
    else
        printf("server accept the client...\n");

    // Function for chatting between client and server
    int err = func(connfd);
    if (err != 0) {
        fprintf(stderr, "Programm interrupted by an error of number: %d\n", err);
        exit(0);
    }


    // After chatting close the socket
    close(sockfd);

    return 0;
}
