#include <sys/socket.h>
#include <netdb.h>
#include <string.h> //memset
#include <string> // c++ strings
#include <vector>
#include <unistd.h> //close
#include <stdio.h>  //printf

using namespace std;

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
///////////////////////////////////////////////////////////////////////////////
int main(){
  int sd=-1, sd2=-1;
  int rc, length, on=1;
  char buffer[buffer_length];

  fd_set read_fd;
  struct timeval timeout;
  struct sockaddr_in serveraddr;

  do {
    sd = socket(AF_INET, SOCK_STREAM, 0);
    if (sd < 0) {
      perror("Error: Server couldn't create master socket!\n");
    }

    memset(&serveraddr, 0, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(server_port);
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);

    rc = bind(sd,(struct sockaddr*) &serveraddr, sizeof(serveraddr));
    if (rc<0) {
      perror("Error: Server couldn't bind master socket!\n");
    }

    rc = listen(sd,10);
    if (rc<0) {
      perror("Error: Server couldn't listen to master socket!");
    }

    printf("Ready for client connect().\n");

    sd2 = accept(sd, NULL, NULL);
    if (sd2<0) {
      perror("Error: Server couldn't accept a master socket connection!");
    }

    timeout.tv_sec = 30;
    timeout.tv_usec = 0;

    FD_ZERO(&read_fd);
    FD_SET(sd2, &read_fd);

    rc = select(sd2+1, &read_fd, NULL, NULL, &timeout);
    if (rc<0) {
      perror("Error: Server couldn't select modified file discriptors!");
    }

    int length = buffer_length;

    rc = recv(sd2, buffer, sizeof(buffer),0);
    // rc is the number of bytes recieved, assuming everything works
    if (rc<0) {
      perror("Error: Server recv failed!");
    }
    else if (rc == 0){
      printf("Server's peer has disconneted or sent a 0 byte message!");
    }
    else if (rc > sizeof(buffer)) {
      perror("Error: Server recieved a message too large to process!");
    }

    rc = send(sd2, buffer, sizeof(buffer), 0);
    if (rc<0) {
      perror("Error: Server couldn't send message!");
    }
  }while(false);

  if(sd!=-1){
    close(sd);
  }
  if(sd2!=-1){
    close(sd2);
  }
}
