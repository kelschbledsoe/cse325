// Gabriella Kelsch Bledsoe
// Project 12 - Network Programming with Sockets Part 2
// CSE 325

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <iostream>
using namespace std;

#define BSIZE 128

int main(int argc, char* argv[]){
  // Utilizing skeleton code given in lecture example 23
  if(argc!= 4){
    char error[BSIZE] = {"usage: proj11.client <host> <port> <filename>\n"};
    write(2,error,BSIZE);
    exit(1);
  }
  char * hostname = argv[1];
  unsigned short int port = atoi(argv[2]);
  int sd = socket(AF_INET, SOCK_STREAM,0);
  if(sd<0){
    perror("socket");
    exit(2);
  }
  struct hostent * server = gethostbyname(hostname);
  if(server==NULL){
    char error[BSIZE] = {"Error: no such host\n"};
    write(2,error,BSIZE);
    exit(3);
  }
  // saddr = socket address
  struct sockaddr_in saddr;
  bzero(&saddr,sizeof(saddr));
  saddr.sin_family = AF_INET;
  saddr.sin_port = htons(port);
  bcopy(server->h_addr,&saddr.sin_addr.s_addr,server->h_length);
  int stat = connect(sd,(struct sockaddr*)&saddr, sizeof(saddr));
  if(stat<0){
    perror("connect");
    exit(4);
  }
  char buffer[BSIZE];
  strcpy(buffer,argv[3]);
  // Send server the file name
  int nsend = send(sd, buffer, strlen(buffer), 0);
  if(nsend<0){
    perror("send");
    exit(5);
  }
  bzero(buffer, BSIZE);
  int nrecv = recv(sd, buffer, BSIZE, 0);
  
  // Check if server said SUCCESS or FAILURE
  if(strcmp(buffer, "SUCCESS")!=0){
    exit(0);
  }
  
  // Tell server proceed
  bzero(buffer, BSIZE);
  char proceed[BSIZE] = {"PROCEED"};
  
  nsend = send(sd, proceed, strlen(proceed), 0);
  if(nsend<0){
    perror("send");
    exit(5);
  }

  // Server now sends contents of input file to client
  bzero(proceed, BSIZE);
  nrecv = recv(sd, proceed, BSIZE, 0);
  write(1,proceed,nrecv);
  // Loop to receive and write entire file due to buffer size
  while(nrecv==BSIZE){
    nrecv = recv(sd, proceed, BSIZE, 0);
    // Write to standard output
    // Use 1 as fd
    write(1,proceed,nrecv);
  }

  // Close the socket (sd)
  close(sd);
} // end of main
