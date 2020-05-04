// Gabriella Kelsch Bledsoe
// Project 12 - Network Programming with Sockets Part 2
// CSE 325

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>
// For open
#include <sys/stat.h>
#include <fcntl.h>
using namespace std;
#define BSIZE 128

int main(int argc, char* argv[]){
  // Logic utilized as skeleton code from lecture examples 23 and 24
  if(argc != 1){
    char error[BSIZE] = {"Usage: proj12.server\n"};
    // Any additional output send to standard error stream
    write(2,error,BSIZE);
    exit(1);
  }
  int listen_sd = socket(AF_INET, SOCK_STREAM, 0);
  if (listen_sd < 0){
    perror("socket");
    exit(2);
  }
  struct sockaddr_in saddr;
  bzero(&saddr, sizeof(saddr));
  saddr.sin_family = AF_INET;
  saddr.sin_port = htons(0);
  saddr.sin_addr.s_addr = htonl(INADDR_ANY);
  int bstat = bind(listen_sd, (struct sockaddr *) &saddr, sizeof(saddr));
  if (bstat < 0){
    perror("bind");
    exit(3);
  }
  int lstat = listen(listen_sd,5);
  if (lstat<0){
    perror("listen");
    exit(4);
  }
  // Send host and port info
  char name[256];
  gethostname(name, 256);
  struct hostent* hostName;
  hostName = gethostbyname(name);
  socklen_t saddr_size = sizeof(saddr);
  getsockname(listen_sd,(struct sockaddr *) &saddr, &saddr_size);
  // Display on standard output stream
  cout << hostName->h_name << " " << ntohs(saddr.sin_port) << endl;
  
  struct sockaddr_in caddr;
  unsigned int clen = sizeof(caddr);
  // blocks until client connects
  int comm_sd = accept(listen_sd, (struct sockaddr *) &caddr, &clen);
  if(comm_sd < 0){
    perror("accept");
    exit(5);
  }
  struct hostent *host_info = gethostbyaddr(
      (const char *) &caddr.sin_addr.s_addr,
      sizeof(caddr.sin_addr.s_addr), AF_INET);
  if (host_info == NULL){
    perror("gethostbyaddr");
    exit(6);
  }
  char * host = inet_ntoa(caddr.sin_addr);
  if (host==NULL){
    perror("inet_ntoa");
    exit(7);
  }

  char buffer[BSIZE];
  bzero(buffer,BSIZE);
  int nrecv = recv(comm_sd, buffer, BSIZE,0);
  if (nrecv<0){
    perror("recv");
    exit(8);
  }
  int file = open(buffer, O_RDONLY);
  int nsend;
  // If cannot open file
  if (file==-1){
    char error[BSIZE] = {"FAILURE"};
    nsend = send(comm_sd, error, strlen(error),0);
    exit(0);  
  }
  // Tell client opened file
  char success[BSIZE] = {"SUCCESS"};
  nsend = send(comm_sd,success,strlen(success),0);
  if (nsend < 0){
    perror("send");
    exit(9);
  }
 
  bzero(success,BSIZE);
  nrecv = recv(comm_sd, success, BSIZE, 0);
  // If client does not send PROCEED
  if(strcmp(success, "PROCEED")!=0){
    char error[BSIZE] = {"Client did not send PROCEED"};
    write(2,error,BSIZE);
    exit(1);
  }
  // Send file info to client
  int size;
  char fileInfo[BSIZE];
  
  while(1){
    size = read(file, fileInfo,BSIZE);
    if (size==0) break;
    nsend = send(comm_sd, fileInfo, size,0);
  }

  close(comm_sd);
  
}


