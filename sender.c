/*
	TCP/IP client
*/ 


#include <stdio.h>

#include <stdlib.h> 
#include <errno.h> 
#include <string.h> 
#include <sys/types.h> 
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#define MAX 1024
#define SERVER_PORT 8080 
#define SERVER_IP_ADDRESS "127.0.0.1"

void send_file(FILE* fp, int sockfd) 
{ 
    char buff[MAX]; 
    int n; 
    for (int i = 0;i<5;i++) { 
        bzero(buff, sizeof(buff)); 
        printf("Enter the string : "); 
        n = 0; 
        // while ((buff[n++] = getchar()) != '\n') 
        //     ; 
        sprintf(buff, "%d", i);
        write(sockfd, buff, sizeof(buff)); 
        bzero(buff, sizeof(buff)); 
        // read(sockfd, buff, sizeof(buff)); 
        // printf("From Server : %s", buff); 
        // if ((strncmp(buff, "exit", 4)) == 0) { 
        //     printf("Client Exit...\n"); 
        //     break; 
        // } 
    } 
    write(sockfd, "exit", 4); 
} 
int main(){
  char *ip = "127.0.0.1";
  int port = 8081;
  int e;

  int sockfd;
  struct sockaddr_in server_addr;
  FILE *fp;
  char *filename = "1gb.txt";

  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if(sockfd < 0) {
    perror("[-]Error in socket");
    exit(1);
  }
  printf("[+]Server socket created successfully.\n");

  server_addr.sin_family = AF_INET;
  server_addr.sin_port = port;
  server_addr.sin_addr.s_addr = inet_addr(ip);

  e = connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr));
  if(e == -1) {
    perror("[-]Error in socket");
    exit(1);
  }
 printf("[+]Connected to Server.\n");

  fp = fopen(filename, "r");
  if (fp == NULL) {
    perror("[-]Error in reading file.");
    exit(1);
  }
  char buffer[10] = {0};

    send_file(fp, sockfd);


 shutdown(sockfd, 2); 
  close(sockfd);

  return 0;
}