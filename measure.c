/*
    TCP/IP-server
*/

#include<stdio.h>


// Linux and other UNIXes
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <time.h>
#include <netinet/in.h>
#include <netinet/tcp.h>


#define MAX 1024
#define SERVER_PORT 8080
static int count = 0;
char EOF_flag = 0;

void write_file(int sockfd)
{
  char buff[MAX]; 
    int n; 
    // infinite loop for chat 
    for (;;) { 
        bzero(buff, MAX); 
  
        // read the message from client and copy it in buffer 
        while(read(sockfd, buff, sizeof(buff))==-1);
        // print buffer which contains the client contents 
        printf("From client: %s\n", buff); 
        bzero(buff, MAX); 
        n = 0; 
        // copy server message in the buffer 
        //Todo: I need to put her wait for system call so i wont loose calls
  
        // and send that buffer to client 
        // write(sockfd, buff, sizeof(buff)); 
  
        // if msg contains "Exit" then server exit and chat ended. 
        if (strncmp("exit", buff, 4) == 0) { 
            printf("Server Exit...\n"); 
            break; 
        } 
    } 
}

int main()
{
  char *ip = "127.0.0.1";
  int port = 8081;
  int e;
  char buf[256];
  int len;
  clock_t t;

  int sockfd, new_sock;
  struct sockaddr_in server_addr, new_addr;
  socklen_t addr_size;
  char buffer[MAX];

  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  
  if(sockfd < 0) { perror("[-]Error in socket");exit(1); }
  
  printf("[+]Server socket created successfully.\n");

  server_addr.sin_family = AF_INET;
  server_addr.sin_port = port;
  server_addr.sin_addr.s_addr = inet_addr(ip);

  e = bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr));
  if(e < 0) {
    perror("[-]Error in bind");
    exit(1);
  }
  printf("[+]Binding successfull.\n");

  if(listen(sockfd, 10) == 0){ printf("[+]Listening....\n");}
  else{ perror("[-]Error in listening"); exit(1);}

   len = sizeof(buf);
  if (getsockopt(sockfd, IPPROTO_TCP, TCP_CONGESTION, buf, &len) != 0) {
  perror("getsockopt");
  return -1;
  }

printf("Current: %s\n", buf);



  addr_size = sizeof(new_addr);
  new_sock = accept(sockfd, (struct sockaddr*)&new_addr, &addr_size);
  EOF_flag = 0;
  
     t = clock();
    write_file(new_sock);
     t = clock() - t;
     double time_taken = ((double)t)/CLOCKS_PER_SEC; // in seconds

  printf("[+]Data written in the file successfully.\n It's took %f seconds",time_taken);

  return 0;
}