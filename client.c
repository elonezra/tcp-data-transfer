/*
 *  A client timing the roundtrip time of a message sent to a server multiple times.
 *  Usage: ./client.out -a <address> -p <port> -b <message_size (bytes)>
 */
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include <netinet/tcp.h>
#include <inttypes.h>
#include <ctype.h>
#include <errno.h>


#define DEFAULT_N_BYTES 1048576
#define DEFAULT_PORT 8787
#define DEFAULT_ADDRESS "127.0.0.1"
#define N_ROUNDS 5
//#include "connection.h"

void error(char *msg)
{
    perror(msg);
    exit(0);
}

void panic(char *msg)
{
    perror(msg);
    exit(0);
}


int send_message(size_t n_bytes, int sockfd, uint8_t *buffer) {
    int bytes_sent = 0;
    int r;
    while (bytes_sent < n_bytes) {
	// Make sure we write exactly n_bytes
        r = write(sockfd, buffer, n_bytes - bytes_sent);
        if (r < 0 && !(errno == EAGAIN || errno == EWOULDBLOCK)) {
            panic("ERROR writing to socket");
        }
        if (r > 0) {
            bytes_sent += r;
        }
    }
    return bytes_sent;
}

int main(int argc, char *argv[]) {
    int sockfd;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    //struct Config config = get_config(argc, argv);
    
    // Init buffers
    uint8_t *rbuffer = malloc(DEFAULT_PORT);
    uint8_t *wbuffer = malloc(DEFAULT_PORT);

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        error("ERROR opening socket");
    }
    server = gethostbyname(DEFAULT_ADDRESS);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(DEFAULT_PORT);

    // Connect and set nonblocking and nodelay
    if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0) {
        error("ERROR connecting");
    }
    fcntl(sockfd, F_SETFL, O_NONBLOCK);
    int flag = 1;
    setsockopt(sockfd, IPPROTO_TCP, TCP_NODELAY, (void *)&flag, sizeof(int));

    printf("Connection successful! Starting...");
    fflush( stdout );

    // Timed send-receive loop
    uint64_t *times_send = malloc(sizeof(uint64_t) * N_ROUNDS);
    uint64_t *times_recv = malloc(sizeof(uint64_t) * N_ROUNDS);
    clock_t start,end;
    for (size_t i = 0; i < N_ROUNDS; i++) {

        // uint64_t tstart = rdtscp();
        // start = clock();
        send_message(DEFAULT_N_BYTES, sockfd, wbuffer);
        
        // uint64_t tsend = rdtsc();
        // end = clock();
     
        //receive_message(config.n_bytes, sockfd, rbuffer);

       // uint64_t tend = rdtsc();

        // times_send[i] = tsend - tstart;
        // times_recv[i] = tend - tsend;
        // printf("\nclock() send %lf",(double)(end - start) / CLOCKS_PER_SEC);
   
    }
    close(sockfd);
    // printf("\nDone!\nSummary: (time_send,\ttime_recv)");
    // for (size_t i = 0; i < N_ROUNDS; i++) {
    //     printf("(%" PRIu64 ",\t%" PRIu64 ")\n", times_send[i], times_recv[i]);
    // }
    free(times_send);
    free(times_recv);
    free(rbuffer);
    free(wbuffer);

    return 0;
}

