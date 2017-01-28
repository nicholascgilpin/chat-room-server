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
#include <queue>
#include <algorithm>
#include <unistd.h> //close
#include <stdio.h>  //printf
#include <pthread.h>
#include <resolv.h>

using namespace std;
int lastUsablePort = 7005;

struct Message{
	// -1 = error
	// 0 = text message
	// 1 = delete
	// 2 = join success
	// 3 = create sucess
	int type;
	int port; // used to store port number in replies from server
	int pop;
	char text[1024]; // stores a message or command
};
// Clears and marshalls a Message packet for sending accross the network
void msgBuilder(Message* m,string text, int type = 0, int port = -1, int pop = -1){
	memset(m->text,0,sizeof(Message));
	m->type = type;
	m->port = port;
	m->pop = pop;
	strncpy(m->text, text.c_str(), sizeof(Message));
	m->text[sizeof(Message)-1] = 0; //NULL terminate the c string
}
class ChatRoom{
  int roomSocketPortNumber;
  int population;
  int masterSD;
  string roomName;
	std::queue<string> inbox;
	pthread_mutex_t inboxLock;

public:
	std::vector<int> sockfds;
	ChatRoom(int roomSock, int pop, string name){
		roomSocketPortNumber = roomSock;
		population = pop;
		roomName = name;

		int status = -1;
		status = pthread_mutex_init(&inboxLock, NULL);
		if(status != 0){
			printf("Error: Chatroom inbox couldn't be locked!\n%s\n",strerror(errno));
		}
	}

  // Adds a message to the inbox in a thread safe manner
  void depositMsg(char* Msg){
		int status = -1337;
		
		status = pthread_mutex_lock(&inboxLock);
		if(status != 0){
			printf("Error: Mutex error %d!\n",status);
		}
		
		string letter(Msg);
		inbox.push(letter);
		
		pthread_mutex_unlock(&inboxLock);
	}
	
	// Removes a message from the inbox in a thread safe manner
	string getMsg(){
		int status = -1337;
		
		status = pthread_mutex_lock(&inboxLock);
		if(status != 0){
			printf("Error: Mutex error %d!\n",status);
		}
		
		string m(inbox.front());
		inbox.pop();
		
		pthread_mutex_unlock(&inboxLock);
		
		return m;
	}
	
	int inboxSize(){
		return inbox.size();
	}
	
	// gets name of chat roomName
  string getName(){
    return roomName;
  }
  // Gets the port number of the socket for the chatroom labeld roomName
  int getPortNum(){
    return roomSocketPortNumber;
  }
	int getMasterSD(){
		return masterSD;
	}
	
	//@TODO: This looks like it will overwrite the socket that is used to accept 
	//	new clients to the room, thus preventing new clients from entering the room
	int setMasterSD(int newSD){
		masterSD = newSD;
	}

	// Gets the number of people in the chatroom labeld roomName
	int getPopulation(){
		return sockfds.size();
	}
	
	// Adds client socket descriptor to the room list  
	void storeSockfds(int fd){
		sockfds.push_back(fd);
	}
	
	void printSockfds(){
		if(sockfds.empty()){
			printf("\nSocket list: Empty\n");
		}
		else{
			for(size_t i = 0; i < sockfds.size(); i++){
				printf("Sockfds[%d]: %d\n", i, sockfds[i]);
			}
		}
	}
	void clearSockfds(){
		for(size_t i = 0; i < sockfds.size(); i++){
			close(sockfds[i]);
		}
		while (!sockfds.empty()){
			sockfds.pop_back();
		}
	}
};

//Data structure to send through threads, because threads can't take multiple arguments
//This allows us to pass filedescriptors to the threads of different rooms 
//So different rooms have a list of clients.
struct thread_args {
    ChatRoom* room;
    int sockfds;
		// true if thread is connected to a room socket, false otherwise
		bool clientIsInRoom; 
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

void deleteRoom(string roomName){
	int roomIndex = -1;
    for (size_t i = 0; i < db.size(); i++) {
      string nameX = db[i].getName();
      if(roomName.compare(nameX) == 0){
        roomIndex = i;
      }
    }
	db.erase(db.begin()+roomIndex);
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

ChatRoom* getARoomPointer(string roomName, std::vector<ChatRoom> &rooms){
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
    return &db[roomIndex];
}

string getAllRoomNames(){
	string names = "";
	for (size_t i = 0; i < db.size(); i++) {
		names += db[i].getName() + "\n";
	}
	return names;
}
// Server Functions ////////////////////////////////////////////////////////////
void printPacket(Message *m){
	printf("type:%d\nport:%d\npopulation:%d\ntext:\n%s\n",m->type,m->port,m->pop,m->text);
}

void runRequest(Message* packet);

// Handles: all message recieving and routing. Sending for non-room messages
void* SocketHandler(void* roomAndFD){
	// Unbundle arguments
	thread_args* passedRoomAndFD;
	passedRoomAndFD = (thread_args*)roomAndFD;
	ChatRoom* c = passedRoomAndFD->room;
	int csock = passedRoomAndFD->sockfds; // roomfds modified by RoomHandler

  int bytecount;
	Message packet;
	int packet_length = sizeof(Message);

  while(1){
    
    bytecount = recv(csock, &packet, packet_length, 0);
		if (bytecount<0) {
			perror("Error: Server recv failed!\n");
			// free(csock);
			// return 0;
		}
		else if (bytecount == 0){
			//printf("Server's peer has disconneted or sent a 0 byte message!\n");
		}
		else if (bytecount > sizeof(Message)) {
			perror("Error: Server recieved a message too large to process!\n");
		}
		else{
			printf("\nReceived bytes %d\nReceived string \"%s\"\n", bytecount, (Message*) packet.text);
			// Deposit message to room if in room. Print to server display otherwise
			if (packet.type == 0 && passedRoomAndFD->clientIsInRoom == true){
				c->depositMsg(packet.text);
				// string asdf = c->getMsg();
				// printf("Should be the deposited message:\n%s\n", asdf.c_str());
			}
			else if(packet.type == 0){
				// Just let the message get echoed to teh server display
			}
			else{
				// run request, record results in packet, and send back packeted results
				runRequest(&packet);
			}
			if (passedRoomAndFD->clientIsInRoom == false) {
				printf("\nPacket immediatly before sending:\n");
				printPacket(&packet);
				if((bytecount = send(csock, &packet, packet_length, 0))== -1){
					// fprintf(stderr, "Error sending data");
					//free(csock);
					return 0;
				}
				printf("Sent bytes from non-bulk %d\n", bytecount);
			}
		}
}
  // free(csock);
  return 0;
}

// Room handler accepts new connections to a room
void* RoomHandler(void *roomAndFD){
	// Unbundle arguments
	thread_args* passedRoomAndFD;
	passedRoomAndFD = (thread_args*)roomAndFD;
	ChatRoom* c = passedRoomAndFD->room;
	int roomMasterSD = passedRoomAndFD->sockfds;
	passedRoomAndFD->room->setMasterSD(roomMasterSD);
	
	printf("\nAccepting connections for: RoomName %s / sd %d\n", c->getName().c_str(), roomMasterSD);
	
	int ssock = -1; //Writing to this after accept will send message to client
	socklen_t addr_size = sizeof(sockaddr_in);
	struct sockaddr_in clientaddr; // client address to be filled in by accept
	pthread_t thread_id=-1;

	printf("\nRoom thread (not main server) waiting to accept join connection\n");
	while(true){
	    if((ssock = accept(roomMasterSD, (sockaddr*)&clientaddr, &addr_size))!= -1){
	      c->storeSockfds(ssock);
		  	c->printSockfds();
		  	printf("\n---------------------\nReceived connection from %s\n",inet_ntoa(clientaddr.sin_addr));
				passedRoomAndFD->clientIsInRoom = true;
				passedRoomAndFD->sockfds = ssock;
	      pthread_create(&thread_id,0,&SocketHandler, (void*)passedRoomAndFD );
	      pthread_detach(thread_id);
	    }
		else{
			printf("Error: Room failed accepting\n%s\n", strerror(errno));
			break;
		}
	}
}

// Handles: Bulk sending for room messages
void* MessageHandler(void* roomAndFD){
	// Unbundle arguments
	thread_args* passedRoomAndFD;
	passedRoomAndFD = (thread_args*)roomAndFD;
	ChatRoom* c = passedRoomAndFD->room;

  int bytecount;
	Message packet;
	int packet_length = sizeof(Message);
	int inboxSZ = 0;
  while(true){
		inboxSZ = c->inboxSize();
		if (inboxSZ > 0) {
			int population = c->getPopulation();
			string message(c->getMsg());
			msgBuilder(&packet,message);
			printf("\nPacket immediatly before bulk send:\n");
			printf("Number of places to send: %d\n",population);
			printPacket(&packet);
			bytecount = -1;
			for (size_t i = 0; i < population; i++) {
				printf("sending to index: %d/%d\n", i, population-1);
				bytecount += send(c->sockfds[i], &packet, packet_length, 0);
				if(bytecount == -1){
					perror("Error: Couldn't send message!\n");
					return 0;
				}
				printf("%d\n",1 );
			}
			printf("%d\n",2 );
			printf("Bulk sent bytes %d\n", bytecount);
		} // Segfault occurs here
		printf("%d\n",3 );
	}
	printf("%d\n",4 );
}

// Creates a room if none exist, then sends the port number to the client
// @TODO: Make robust by adding mutex lock on db operations and checking port use
void  rCreate(string roomName, Message* packet){
	int roomSD = -1, status = -1, port = -1, population = 0;
	struct sockaddr_in roomaddr;
	pthread_t acceptorThread=-1,messageThread=-1;
	
	if(roomExists(roomName)){
		printf("Room %s found!\n", roomName.c_str());
		ChatRoom c = getARoom(roomName, db);
		packet->port = c.getPortNum();
		packet->type = 2;
		string s = "\nCurrent rooms:\n";
		string catted = s + getAllRoomNames(); 
		const char * msg = catted.c_str();
		char buffer[1024];
		memset(buffer,0,1024);
		strncpy(buffer,msg,1024 -1);
		memcpy(packet->text,buffer,1024);
	}
	else{
		printf("Creating room %s\n",roomName.c_str());
		// create a master socket to recieve connection requests
		roomSD = socket(AF_INET, SOCK_STREAM, 0);
		if (roomSD < 0) {
			perror("Error: Server couldn't create room socket!\n");
		}
		memset(&roomaddr, 0, sizeof(roomaddr));
		roomaddr.sin_family = AF_INET;
		lastUsablePort = lastUsablePort + 1;
		printf("lastUsablePort is: %d", lastUsablePort);

		roomaddr.sin_port = htons(lastUsablePort);
		roomaddr.sin_addr.s_addr = INADDR_ANY;

		// If a socket is a mailbox, then bind adds the steet address to the mailbox
		for (size_t i = 0; i < 100; i++) {
			status = bind(roomSD,(struct sockaddr*) &roomaddr, sizeof(roomaddr));
			if (status<0) {
				break;
			}
			//lastUsablePort += 1;
			roomaddr.sin_port = htons(lastUsablePort);
		}
		status = -1;
		status = listen(roomSD, 10);
		if (status<0) {
			perror("Error: Room couldn't listen to socket!");
		}
		port = lastUsablePort;

		// Critical Section!///////////////////////////////////////////////////////
		ChatRoom d = ChatRoom(port,population,roomName);
		printf("\nChatRoom's Port outside db: %d\n", d.getPortNum());

		db.push_back(d);
		printf("In db:\n");
		ChatRoom r = getARoom(roomName,db);
		ChatRoom *threadedRoom = getARoomPointer(roomName, db);
		printf("port %d\n",r.getPortNum() );
		printf("pop  %d\n",r.getPopulation() );
		printf("name %s\n",r.getName().c_str());
		// Critical Section!^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
		
		if (roomExists(roomName)){
			printf("Create room %s success!\n", roomName.c_str());
			status = 3;
		}
		memset(packet,0,sizeof(Message));
		packet->type = status;
		packet->port = port;
		
		// Create a thread to accept connections to this room's socket
		//	RoomHandler modifies the threadArgs and forwareds them to SocketHandler
		thread_args* passingRoomAndsd = new thread_args;
		passingRoomAndsd->room = threadedRoom;
		passingRoomAndsd->sockfds = roomSD;
		pthread_create(&acceptorThread,0,&RoomHandler, (void*)passingRoomAndsd);
		pthread_detach(acceptorThread);
		
		thread_args* roomInfo = new thread_args;
		roomInfo->room = threadedRoom;
		roomInfo->sockfds = roomSD;
		pthread_create(&messageThread,0,&MessageHandler,(void*)roomInfo);
		pthread_detach(messageThread);
	}
}

// Sends chatroom socket port and member population size to client for roomName
void rJoin(string roomName, Message* packet){
  if(roomExists(roomName)){
		printf("Sending client to %s\n",roomName.c_str());
		ChatRoom temp = getARoom(roomName, db);
		packet->port = temp.getPortNum();
		packet->type = 2;
		packet->pop = temp.getPopulation();
  }
  else{
		printf("Client cannot join %s because room doesn't exist.\n",roomName.c_str());
		packet->type = -1;
  }
}

// Description: Return -1 for error
void rDelete(string roomName, Message* packet){
	if(roomExists(roomName)){
		int sd=-1;
		ChatRoom temp = getARoom(roomName, db);
		packet->port = temp.getPortNum();
		packet->type = 1;
		packet->pop = temp.getPopulation();
		printf("Deleting Room %s\n",roomName.c_str());
		//ChatRoom temp = getARoom(roomName, db);
		ChatRoom *c = getARoomPointer(roomName, db);
		c->printSockfds();
		c->clearSockfds();
		c->printSockfds();
		
		printf("TESTING TESTING: %d", c->getMasterSD());
		close(c->getMasterSD());
		deleteRoom(roomName);
		

		/*
		packet->port = temp.getPortNum();
		packet->type = 2;
		packet->pop = temp.getPopulation();
		int roomIndex; 
		
		for (size_t i = 0; i < db.size(); i++) {
		  string nameX = db[i].getName();
		  if(roomName.compare(nameX) == 0){
			roomIndex = i;
		  }
		}
		db.erase(db.begin() + roomIndex);
		
		sd = socket(AF_INET, SOCK_STREAM, 0);
		if (sd < 0) {
			perror("Error: Server couldn't create master socket!\n");
		}*/

		// Figure out our address
		/*
		memset(&serveraddr, 0, sizeof(serveraddr));
		serveraddr.sin_family = AF_INET; // Use ip addresses with 4 dots
		// The port tells packets which program to go to after entering the copmuter
		serveraddr.sin_port = htons(packet->port); 
		// INADDR_ANY means read to any incoming connections on this computer
		serveraddr.sin_addr.s_addr = INADDR_ANY;*/
  }
  else{
		printf("Client cannot delete %s because room doesn't exist.\n",roomName.c_str());
		packet->type = -1;
  }
}

// Returns the message type status of the request
void runRequest(Message* packet){
	char command[6], roomName[80];
	sscanf(packet->text, "%s %s", command, roomName);
	char type = command[1];
	if (type == 'j') {
		printf("Handling join for room %s\n",roomName);
		memset(packet, 0, sizeof(Message));	
		rJoin(roomName, packet);
	}
	else if (type == 'c'){
		printf("Handling create for room %s\n",roomName);
		memset(packet, 0, sizeof(Message));
		rCreate(roomName, packet);
	}
	else if (type == 'd'){
		printf("Handling delete for room %s\n",roomName);
		memset(packet, 0, sizeof(Message));
		rDelete(roomName, packet);
	}
	else{
		printf("Error: unrecognized client command %s!\n", type);
		memset(packet,0,sizeof(Message));
		packet->type = -1;
	}
}

///////////////////////////////////////////////////////////////////////////////
int main(){
	int server_port = lastUsablePort;
	int buffer_length = 250;
	int masterSD=-1, incomingSD=-1; //id's if positive or errors if negative
	int rc, length, on=1; 
	char buffer[buffer_length]; // buffer to store incoming message
	fd_set read_fd;
	int* ssock;
	sockaddr_in sadr;
	struct timeval timeout;
	struct sockaddr_in serveraddr; // local address
	// struct sockaddr_in clientaddr; // client address
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
    printf("waiting for a connection on port %d\n", server_port);
		int sd = -1;
		// Bundle args
		thread_args* justSD = new thread_args;
		justSD->room = NULL;
		justSD->clientIsInRoom = false;
		justSD->sockfds = sd;
		
		if((sd = accept(masterSD, (sockaddr*)&sadr, &addr_size))!= -1){
      printf("---------------------\nReceived connection from %s\n",inet_ntoa(sadr.sin_addr));
			justSD->sockfds = sd;
      pthread_create(&thread_id,0,&SocketHandler, (void*)justSD );
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
