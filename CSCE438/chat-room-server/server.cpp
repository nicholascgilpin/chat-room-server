// server.cpp
// Chat Room Server
// Homework 1
// Alexandria Stacy and Nicholas Gilpin
// Due January 27, 2017

#include <sys/socket.h>
#include <netdb.h>
#include <string.h> //memset
#include <unistd.h> //close
#include <stdio.h>  //printf
#include"stdlib.h"
#include"sys/types.h"
#include <iostream>
#include"string.h"
#include <arpa/inet.h>
#include"netinet/in.h"
#include"pthread.h"
#include <stdint.h>
#include <iostream>
#include <string.h> //memset
#include <unistd.h> //close
#include <stdio.h>  //printf
#include <cstdlib>
using namespace std;
//Disregard all these includes I'll clean them up later I promise -Alex

const int server_port = 3005;
const int buffer_length = 250;

class ChatRoom{
  int roomSocketPortNumber;
  int population;
  string roomName;

public:
  // gets name of chat roomName
  string getName(){
    return roomName;
  }
  // Gets the port number of the socket for the chatroom labeld roomName
  int getPortNum(){
    return roomSocketPortNumber;
  }

  // Gets the number of people in the chatroom labeld roomName
  int getPopulation(){
    return population;
  }
};
// Database Functions //////////////////////////////////////////////////////////
std::vector<ChatRoom> db;
// Returns true if a room exists in the database
bool roomExists(string roomName){
  bool exists = false;
    for (size_t i = 0; i < db.size(); i++) {
      string nameX = db[i].getName();
      if(roomName.compare(nameX) == 0){
        exists = true;
      }
    }
  return exists;
}
// Returns the room object for roomName. Doesn't check if room exists!
ChatRoom getARoom(string roomName, std::vector<ChatRoom> rooms){
  int roomIndex = -1;
    for (size_t i = 0; i < db.size(); i++) {
      string nameX = db[i].getName();
      if(roomName.compare(nameX) == 0){
        roomIndex = i;
      }
    }
    if (roomIndex == -1){
      printf("Server couldn't find room!\n");
    }
    return db[roomIndex];
}
// Server Functions ////////////////////////////////////////////////////////////
// Description: Return -1 for error
int rCreate(){

}

// Sends chatroom socket port and member population size to client for roomName
int rJoin(string roomName){
  if(roomExists(roomName)){
    ChatRoom temp = getRoom(roomName, db);
    // send to client
  }
  else{
    // @TODO: Create room and send info to client
  }
}

// Description: Return -1 for error
int rDelete(){

}

void * msg(void * socket) {
 int sd, rc;
 char buffer[buffer_length]; 
 sd = *((int*)&socket);
 memset(buffer, 0, buffer_length);  
 for (;;) {
  rc = recvfrom(sd, buffer, buffer_length, 0, NULL, NULL);  
  if (rc < 0) {  
   printf("Error receiving data!\n");    
  } else {
   printf("client: ");
   fputs(buffer, stdout);
   //printf("\n");
  }  
 }
}

///////////////////////////////////////////////////////////////////////////////
int main(){
	int sd=-1, sd2=-1;
	int rc, on=1;
	socklen_t length;
	char buffer[buffer_length];
	pthread_t rThread;
	fd_set read_fd;
	char client_addr[client_length];
	struct timeval timeout;
	struct sockaddr_in serveraddr, clientaddr;


    sd = socket(AF_INET, SOCK_STREAM, 0);
    if (sd < 0) {
      perror("Error: Server couldn't create master socket!\n");
	  exit(1);
    }

    memset(&serveraddr, 0, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(server_port);
    serveraddr.sin_addr.s_addr = INADDR_ANY;
	
    rc = bind(sd,(struct sockaddr*) &serveraddr, sizeof(serveraddr));
	
    if (rc<0) {
      perror("Error: Server couldn't bind master socket!\n");
	  exit(1);
    }
	printf("Binding done...\n");
	
    rc = listen(sd,10);
    if (rc<0) {
      perror("Error: Server couldn't listen to master socket!\n");
    }
	
    printf("Ready for client connect().\n");

    sd2 = accept(sd, (struct sockaddr *)&clientaddr, &length);

    if (sd2<0) {
      perror("Error: Server couldn't accept a master socket connection!\n");
	  exit(1);
    }
	else{
		printf("Client connected!\n");
	}
	
    timeout.tv_sec = 30;
    timeout.tv_usec = 0;
	
	inet_ntop(AF_INET, &(clientaddr.sin_addr), client_addr, client_length);
	printf("Connection accepted from %s...\n", client_addr);
	
	memset(buffer, 0, buffer_length);
	printf("Enter your messages one by one and press return key!\n");
	
	 //creating a new thread for receiving messages from the client
	 rc = pthread_create(&rThread, NULL, msg, (void *) sd2);
	 if (rc) {
	  printf("ERROR: Return Code from pthread_create() is %d\n", rc);
	  exit(1);
	 }

	 while (fgets(buffer, buffer_length, stdin) != NULL) {
	  rc = sendto(sd2, buffer, buffer_length, 0, (struct sockaddr *) &clientaddr, length);  
	  if (rc < 0) {  
	   printf("Error sending data!\n");  
	   exit(1);
	  }
	 }  


  if(sd!=-1){
    close(sd);
  }
  if(sd2!=-1){
    close(sd2);
  }
  pthread_exit(NULL);
}