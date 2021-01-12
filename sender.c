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

uint8_t * getfile( ) 
{ 
    int n; 
    FILE *fp;
    char *filename = "1mb.txt";

    fp = fopen(filename, "r");
    if (fp == NULL) {
      perror("[-]Error in reading file.");
      exit(1);
    }
    fseek(fp, 0, SEEK_END); // seek to end of file
    size_t size = ftell(fp); // get current file pointer
    fseek(fp, 0, SEEK_SET); // seek back to beginning of file
    printf("the size of file %ld\n", size);
    uint8_t *wbuffer = malloc(size);
    int i = 0;
    char temp;
     while ((temp =  fgetc( fp )) != EOF)
     {
         *(wbuffer + i++) = temp;
     }

    return wbuffer;
    
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


    // Init buffers
    uint8_t *wbuffer =  getfile();//malloc(DEFAULT_N_BYTES);// will contain the info
    /// now it contain rubbish

   


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
	
	/**
	 An IP socket address is defined as a combination of an IP
       interface address and a 16-bit port number.  The basic IP
       protocol does not supply port numbers, they are implemented by
       higher level protocols like udp(7) and tcp(7).  On raw sockets
       sin_port is set to the IP protocol.

           struct sockaddr_in {
               sa_family_t    sin_family; /* address family: AF_INET */
            // in_port_t      sin_port;   /* port in network byte order */
            //   struct in_addr sin_addr;   /* internet address */
          // };
	**/
	
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(DEFAULT_PORT);

    // Connect and set nonblocking and nodelay
    // initiate a connection on a socket
    if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0) {
        error("ERROR connecting");
    }
    fcntl(sockfd, F_SETFL, O_NONBLOCK);
    int flag = 1;
    setsockopt(sockfd, IPPROTO_TCP, TCP_NODELAY, (void *)&flag, sizeof(int));

    printf("Connection successful! Starting...");
    fflush( stdout );

    // Timed send-receive loop
    clock_t start,end;
    for (size_t i = 0; i < N_ROUNDS; i++) {

        
        send_message(DEFAULT_N_BYTES, sockfd, wbuffer);
        
        
    }
   //sleep(3);
        for (size_t i = 0; i < N_ROUNDS; i++) {

        
        send_message(DEFAULT_N_BYTES, sockfd, wbuffer);
        
        
    }
    close(sockfd);
   
    free(wbuffer);

    return 0;
}

