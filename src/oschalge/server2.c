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

#define MAX 392
#define REQUEST_PACKET_SIZE 49
#define SIZE_HASH 32
#define SIZE_START 8
#define SIZE_END 8
#define SIZE_OF_BYTE 8

#define SA struct sockaddr


typedef struct {
    uint8_t* hash;
    u_int64_t start;
    u_int64_t end;
    uint8_t p;
} Request;

Request *create_empty_request() {
    Request *output = NULL;
    output  = malloc(sizeof(Request));
    if (output != NULL) {
        output->hash = calloc(SIZE_HASH, sizeof(uint8_t));
        //output->start = calloc(SIZE_START, sizeof(char));
        //output->end = calloc(SIZE_END, sizeof(char));
        if (output->hash != NULL) {
            output->p = 0;
            output->start = 0;
            output->end = 0;
            printf("Request created sucessfully!\n");
            return output;
        }
    }
    return NULL;
}


Request *getRequest(const char *all_bytes) {
    if (all_bytes != NULL) {
        printf("----TEST----\n");
        printf("Original:\n");
        for(size_t i = 0; i<REQUEST_PACKET_SIZE;++i){
            printf("%ld",i);
            printf(": %d\n", (uint8_t)all_bytes[i]);
        }

        Request *output = create_empty_request();

        printf("Hash\n");
        for (size_t i=0; i<SIZE_HASH; ++i){
            output->hash[i]= (uint8_t)all_bytes[i];
            printf("%ld:",i);
            printf("%d\n",output->hash[i]);
        }


        for (size_t i=0; i<SIZE_START; ++i){
            output->start<<=SIZE_OF_BYTE;
            output->start|=(u_int64_t)all_bytes[SIZE_HASH+i];
            output->end<<=SIZE_OF_BYTE;
            output->end|=(u_int64_t)all_bytes[SIZE_HASH+SIZE_START+i];
        }
        printf("Start : %ld\n", output->start);
        printf("End : %ld\n", output->end);


        /*for (size_t i=0; i<SIZE_END; ++i){
            output->end[i]= all_bytes[SIZE_HASH+SIZE_START+i];
            printf("%ld:",i);
            printf("%d\n",(uint8_t)output->end[i]);
        }*/

        output->p =(u_int8_t)all_bytes[SIZE_HASH+SIZE_START+SIZE_END];
        printf("P: %d\n", output->p);
        
        
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
        printf("size of buffer : %ld\n", sizeof(buff));
        size_t length = read(connfd, buff, sizeof(buff));
        printf("The size of the packet received is: %ld\n", length);
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
