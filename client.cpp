/*
Data Comm Programming Assignment 1
Cameron Massengill ~ ctm351
Zachary Sahnger ~ zjs79
9/9/2021
client component
References:
Maxwell Young, example documentation
https://www.tutorialspoint.com/read-data-from-a-text-file-using-cplusplus
*/
#include <iostream>
#include <sys/types.h>   // defines types (like size_t)
#include <sys/socket.h>  // defines socket class
#include <netinet/in.h>  // defines port numbers for (internet) sockets, some address structures, and constants
#include <netdb.h> 
#include <iostream>
#include <fstream>
#include <arpa/inet.h>   // if you want to use inet_addr() function
#include <string.h>
#include <unistd.h>

using namespace std;

int main(int argc, char *argv[]){
  

  struct hostent *s; 
  s = gethostbyname(argv[1]);
  int n_port = std::stoi(argv[2]); //convert cmd line argument to int
  string filename = argv[3];
  struct sockaddr_in server;
  int mysocket = 0;
  socklen_t slen = sizeof(server);
  char payload[512]="1234"; //define handshake data
  
  if ((mysocket=socket(AF_INET, SOCK_DGRAM, 0))==-1)
    cout << "Error in creating socket.\n";
    
  memset((char *) &server, 0, sizeof(server));
  server.sin_family = AF_INET;
  server.sin_port = htons(n_port);
  bcopy((char *)s->h_addr, 
	(char *)&server.sin_addr.s_addr,
	s->h_length);
      

  if (sendto(mysocket, payload, 8, 0, (struct sockaddr *)&server, slen)==-1)
    cout << "Error in sendto function.\n";

  if (recvfrom(mysocket, payload, 512, 0, (struct sockaddr *)&server, &slen)==-1)
    cout << "Failed to receive new port.\n";
  
  int r_port = std::strtol(payload, nullptr, 10);
  server.sin_port = htons(r_port);
  bcopy((char *)s->h_addr, (char *)&server.sin_addr.s_addr, s->h_length);

  fstream infile(filename); 
  char fileData[6];
  char c;
  bool fileEnd;
  char ack[512];

  while(!fileEnd){
    memset(fileData, 0, sizeof fileData); //reset fileData to all 0

    for(int i =0; i < 4; i++){
      fileEnd = (infile.peek() == EOF);
      if(fileEnd){
        std::sprintf(&fileData[4], "%d", i);
      }
      else{
        fileData[4] = '0';
      }
      c = infile.get();
      fileData[i] = c;
    }

    if (sendto(mysocket, fileData, 512, 0, (struct sockaddr *)&server, slen) == -1){
      cout << "Error in creating socket.\n";
    }

    recvfrom(mysocket, ack, 512, 0, (struct sockaddr *)&server, &slen); 
    cout << ack << endl;

  }
  infile.close();
  
  close(mysocket);
  return 0;
}