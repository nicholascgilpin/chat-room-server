#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h> //memset
#include <string> // c++ strings
#include <vector>
#include <unistd.h> //close
#include <stdio.h>  //printf
#include <pthread.h>
#include <resolv.h>

using namespace std;

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
    ChatRoom temp = getARoom(roomName, db);
    // send to client
  }
  else{
    // @TODO: Create room and send info to client
  }
}

// Description: Return -1 for error
int rDelete(){

}

void* SocketHandler(void* lp){
  int *csock = (int*)lp;

  char buffer[1024];
  int buffer_len = 1024;
  int bytecount;

  memset(buffer, 0, buffer_len);

  while(1){
    if((bytecount = recv(*csock, buffer, buffer_len, 0))== -1){
      fprintf(stderr, "Error receiving data");
      free(csock);
      return 0;
    }
    printf("Received bytes %d\nReceived string \"%s\"\n", bytecount, buffer);
    strcat(buffer, " ");

    if((bytecount = send(*csock, buffer, strlen(buffer), 0))== -1){
      fprintf(stderr, "Error sending data");
      free(csock);
      return 0;
    }
     
    printf("Sent bytes %d\n", bytecount);
}
  free(csock);
  return 0;
}

///////////////////////////////////////////////////////////////////////////////
int main(){
  int server_port = 3005;
  int buffer_length = 250;
  int masterSD=-1, incomingSD=-1; //id's if positive or errors if negative
  int rc, length, on=1; 
  char buffer[buffer_length]; // buffer to store incoming message
  fd_set read_fd;
  int* ssock;
  sockaddr_in sadr;
  struct timeval timeout;
  struct sockaddr_in serveraddr; // local address
  struct sockaddr_in clientaddr; // client address
  uint clientAddrLen; // length of incoming message
  int recvMsgSize; //size of recieved message
  pthread_t thread_id=0;
// Start accepting connections ////////////////////////////////////////////////

    // create a master socket to recieve connection requests
    masterSD = socket(AF_INET, SOCK_STREAM, 0);
    if (masterSD < 0) {
      perror("Error: Server couldn't create master socket!\n");
    }

    // Figure out our address
    memset(&serveraddr, 0, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET; // Use ip addresses with 4 dots
    // The port tells packets which program to go to after entering the copmuter
    serveraddr.sin_port = htons(server_port); 
    // INADDR_ANY means read to any incoming connections on this computer
    serveraddr.sin_addr.s_addr = INADDR_ANY;
    // htoXX means to flip numbers backwards 100 --> 001 (It's what networks use)

    // If a socket is a mailbox, then bind adds the steet address to the mailbox
    rc = bind(masterSD,(struct sockaddr*) &serveraddr, sizeof(serveraddr));
    if (rc<0) {
      perror("Error: Server couldn't bind master socket!\n");
    }

    const int backlogSize = 10; 
    // listen tells the post office that there's an active mailbox here
    rc = listen(masterSD, backlogSize);
    if (rc<0) {
      perror("Error: Server couldn't listen to master socket!");
    }

    socklen_t addr_size = sizeof(sockaddr_in);
// Stand ready to handle new clients or requests //////////////////////////////
   while(true){
    printf("waiting for a connection\n");
    ssock = (int*)malloc(sizeof(int));
    if((*ssock = accept(masterSD, (sockaddr*)&sadr, &addr_size))!= -1){
      printf("---------------------\nReceived connection from %s\n",inet_ntoa(sadr.sin_addr));
      pthread_create(&thread_id,0,&SocketHandler, (void*)ssock );
      pthread_detach(thread_id);
    }
    else{
      fprintf(stderr, "Error accepting");
    }
  }



/*
    printf("Server: Ready for client connect().\n");
    for(;;){
      // accept() Extracts the first connection
      //  request on the queue of pending connections for the listening socket,
      //  sockfd, creates a new connected socket, and returns a new file
      //  descriptor referring to that socket.  The newly created socket is not
      //  in the listening state.  The original socket sockfd is unaffected by
      //  this call.
      incomingSD = accept(masterSD, NULL, NULL);
      if (incomingSD<0) {
        perror("Error: Server couldn't accept a master socket connection!");
      }
      
      printf("Server: Client connected!\n");
      
      // Reset the address size for different clients
      clientAddrLen = sizeof(clientaddr);
      
      // Wait for a message to arrive, copy the message to buffer, copy address to clientaddr
      recvMsgSize = recvfrom(incomingSD, buffer, sizeof(buffer),0, (struct sockaddr*)&clientaddr, &clientAddrLen);
      if (recvMsgSize<0) {
        perror("Error: Server recv failed!");
      }
      else if (recvMsgSize == 0){
        printf("Server's peer has disconneted or sent a 0 byte message!");
      }
      else if (recvMsgSize > sizeof(buffer)) {
        perror("Error: Server recieved a message too large to process!");
      }
      
      printf("Server: handling client: %s\n", inet_ntoa(clientaddr.sin_addr));
      
      // Send buffer to the client on socket with id incomingSD
      rc = send(incomingSD, buffer, sizeof(buffer), 0);
      // rc is the number of bytes sent
      if (rc<0) {
        perror("Error: Server couldn't send message!");
      }

    }
// Clean up code///////////////////////////////////////////////////////////////
  printf("Server: Shutting down!\n");

  if(masterSD!=-1){
    close(masterSD);
  }
  if(incomingSD!=-1){
    close(incomingSD);
  } */
}
