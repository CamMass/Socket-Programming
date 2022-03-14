/*
Data Comm Programming Assignment 1
Cameron Massengill ~ ctm351
Zachary Sahnger ~ zjs79
9/9/2021
server component
References: 
Maxwell Young, example documentation
https://www.tutorialspoint.com/read-data-from-a-text-file-using-cplusplus
*/

#include<iostream>
#include <sys/types.h>   // defines types (like size_t)
#include <sys/socket.h>  // defines socket class
#include <netinet/in.h>  // defines port numbers for (internet) sockets, some address structures, and constants
#include <time.h>        // used for random number generation
#include <string.h> // using this to convert random port integer to string
#include <arpa/inet.h>
#include <stdio.h>
#include <unistd.h>
#include <string>
#include <sstream>
#include <fstream>

using namespace std;

int main(int argc, char *argv[]){
  
  struct sockaddr_in server;
  struct sockaddr_in client;
  int mysocket = 0;
  int i = 0;
  int n_port = std::stoi(argv[1]); //convert cmd line input to int
  socklen_t clen = sizeof(client);
  char payload[512];
  char ack[512];
  
  if ((mysocket=socket(AF_INET, SOCK_DGRAM, 0))==-1)
    cout << "Error in socket creation.\n";
  
  memset((char *) &server, 0, sizeof(server));
  server.sin_family = AF_INET;
  server.sin_port = htons(n_port);
  server.sin_addr.s_addr = htonl(INADDR_ANY);
  if (bind(mysocket, (struct sockaddr *)&server, sizeof(server)) == -1)
    cout << "Error in binding.\n";

  //send recieve handshake payload from client through socket
  if (recvfrom(mysocket, payload, 512, 0, (struct sockaddr *)&client, &clen)==-1)
    cout << "Failed to receive.\n";

  //generate random port, convert to char array
  srand(time(NULL));
  int RanPort = 1024 + (rand() % 65535);
  stringstream ss;
  ss << RanPort << endl;
  string stringPortNum = ss.str();
  int n = stringPortNum.length();
  char r_port[n + 1];
  strcpy(r_port, stringPortNum.c_str());
  cout << "The random port chosen is " << r_port << endl;
  //cout << "Sending new port: " << r_port << endl;
  if (sendto(mysocket, r_port, 8, 0, (struct sockaddr *)&client, clen)==-1)
    cout << "Error in sending new port.\n";
  close(mysocket);
  
  //create new socket
  if ((mysocket=socket(AF_INET, SOCK_DGRAM, 0))==-1)
    cout << "Error in socket creation.\n";
  
  //update port
  server.sin_port = htons(RanPort);
  server.sin_addr.s_addr = htonl(INADDR_ANY);
  if (bind(mysocket, (struct sockaddr *)&server, sizeof(server)) == -1)
    cout << "Error in binding.\n";


  //recieve file packets
  ofstream file;
  file.open("blah.txt"); //open the file
  int index = 4;

  if (file){
    std::ofstream file("blah.txt", ios_base::trunc); //overwrite existing file
  }else{
    ofstream file("blah.txt"); //otherwise, create new file
  }
  memset(payload, 0, sizeof payload); //clear payload in memory

  payload[4] = '0';
  while(payload[4] == '0'){

    //recieve payload from client
    if (recvfrom(mysocket, payload, 512, 0, (struct sockaddr *)&client, &clen)==-1)
      cout << "Failed to receive.\n";

    //uppercase the payload contents
    for(int i = 0; i < 4; i++){
      ack[i] = toupper(payload[i]);
    }
    ack[4] = payload[4];
    //send ack
    sendto(mysocket, ack, 512, 0, (struct sockaddr *)&client, clen);
 
    if (payload[4] =='0'){
      file << payload[0] << payload[1] << payload[2] << payload[3];
    }

  }
  //when final character in payload is not zero, indicates last packet and specifies how many characters in packet.
  if (payload[4]!='0'){
    index = payload[4] -'0'; //convert character to int for index
      for(int j = 0; j < index; j++){
        file << payload[j];
    }
  }
  file.close();
  close(mysocket);
  return 0;
}
