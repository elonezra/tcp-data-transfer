/*
 *  A server receiving and sending back a message multiple times.
 *  Usage: ./server.out -p <port> -n <message_size (bytes)>
 */
#include <stdio.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdlib.h>
#include <strings.h>
#include <fcntl.h>
#include <time.h>
#include <errno.h>
#include <netinet/tcp.h>

#define DEFAULT_N_BYTES 1048576
#define DEFAULT_PORT 8787
#define DEFAULT_ADDRESS "127.0.0.1"
#define N_ROUNDS 5

//#include "connection.h"
void panic(char *msg)
{
    perror(msg);
    exit(0);
}

void error(char *msg)
{
    perror(msg);
    exit(1);
}

int receive_message(size_t n_bytes, int sockfd, uint8_t *buffer) {
    int bytes_read = 0;
    int r;
    while (bytes_read < n_bytes) {
	// Make sure we read exactly n_bytes
        r = read(sockfd, buffer, n_bytes - bytes_read);
        if (r < 0 && !(errno == EAGAIN || errno == EWOULDBLOCK)) {
            panic("ERROR reading from socket");
        }
        if (r > 0) { bytes_read += r; }
    }
    return bytes_read;
}

int main()
{
    int sockfd, newsockfd;
    //struct Config config = get_config(argc, argv);
    uint8_t *buffer = malloc(DEFAULT_N_BYTES);
    struct sockaddr_in serv_addr, cli_addr;
    double avg  = 0;

    // Create listening socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        error("ERROR opening socket");
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(DEFAULT_PORT);
    if (bind(sockfd, (struct sockaddr *) &serv_addr,
        sizeof(serv_addr)) < 0) {
        error("ERROR on binding");
    }
    printf("Server ready, listening on port %d\n", DEFAULT_PORT); 
    fflush(stdout);
	
    listen(sockfd, 5); // listen for connections on a socket
	
    socklen_t clilen = sizeof(cli_addr);

    // Accept connection and set nonblocking and nodelay
    newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
    if (newsockfd < 0) {
        error("ERROR on accept");
    }
    fcntl(newsockfd, F_SETFL, O_NONBLOCK);//the function to anble syscall
    char a[255];
    socklen_t len;
    len = sizeof(a);
    getsockopt(newsockfd, IPPROTO_TCP, TCP_CONGESTION, a, &len);// we get the cc mode cubic or reno
	
    printf("\ncc mode is %s\n", a);
	
    // Receive-send loop
    printf("Connection accepted, ready to receive!\n");
	
    int i;
    clock_t start,end;
    for (i = 0; i < N_ROUNDS; i++) {
        start = clock();
        receive_message(DEFAULT_N_BYTES, newsockfd, buffer);
        end = clock();
        avg += (double)(end - start) / CLOCKS_PER_SEC;
             printf("\nmessage %d duration: %lf sec",i+1,(double)(end - start) / CLOCKS_PER_SEC);

    }
    
    avg /= N_ROUNDS;
    printf("\n\navarage = %lf ",avg);
    printf("\nDone!\n");
    avg = 0;
    
    printf("\nchange cc\n");
    strcpy(a, "reno"); 
    len = strlen(a);
    setsockopt(newsockfd, IPPROTO_TCP, TCP_CONGESTION, a, len);
    getsockopt(newsockfd, IPPROTO_TCP, TCP_CONGESTION, a, &len);
    printf("cc mode is %s \n", a);

    // Receive-send loop
    printf("Connection accepted, ready to receive!\n");
    
    for (i = 0; i < N_ROUNDS; i++) {
        start = clock();
        receive_message(DEFAULT_N_BYTES, newsockfd, buffer);
        end = clock();
        avg += (double)(end - start) / CLOCKS_PER_SEC;
        printf("\nmessage %d duration: %lf sec",i+1,(double)(end - start) / CLOCKS_PER_SEC);

    }
    avg /= N_ROUNDS;
     printf("\navarage = %lf ",avg);
    printf("\nDone!\n");

    // Clean state
    close(sockfd);
    close(newsockfd);

    return 0; 
}

