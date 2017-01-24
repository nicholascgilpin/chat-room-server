#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h> //memset
#include <unistd.h> //close
#include <stdio.h>  //printf
#include <stdlib.h>
#include <string>
#include <netinet/in.h>

using namespace std;
// Constants //////////////////////////////////////////////////////////////////


// Client Functions ////////////////////////////////////////////////////////////
// Description: Return -1 for error
int createRoom(){

}

// Description: Return -1 for error
int joinRoom(){

}

// Description: Return -1 for error
int deleteRoom(){

}
///////////////////////////////////////////////////////////////////////////////
int main(int argc, char* argv[]){
	int server_port = 3005;
	int rbufSize; // recieving buffer
	int NETDB_MAX_HOST_NAME_LENGTH = 512;
	char* server_name = (char*)"sun.cs.tamu.edu";
  int sd=-1, rc, bytesReceived;
  char server[NETDB_MAX_HOST_NAME_LENGTH];
  struct sockaddr_in serveraddr; // server address
	struct sockaddr_in fromAddr; // address of whoever replies to client
	uint fromSize; // size of the address from whoever replies to client
  struct hostent* hostp; // used to store DNS address information about server
	
	// Read command line arguments if present
	if(argc>1) strcpy(server,argv[1]);
	else strcpy(server,server_name);

		// create socket and store id in sd
    sd = socket(AF_INET, SOCK_STREAM, 0);
    if (sd<0) {
      perror("Error: Client couldn't create socket!");
    }

		// Figure out client's from address and server to address
    memset(&serveraddr, 0, sizeof(serveraddr));
    serveraddr.sin_family      = AF_INET;
    serveraddr.sin_port        = htons(server_port);
    serveraddr.sin_addr.s_addr = inet_addr(server);

		// Figure out the ip address that matches the url we have
    if (serveraddr.sin_addr.s_addr == (unsigned long)INADDR_NONE)      {
       hostp = gethostbyname(server);
       if (hostp == (struct hostent *)NULL) {
          perror("Host not found --> ");
          exit(1);
       }
       memcpy(&serveraddr.sin_addr,
                    hostp->h_addr,
                    sizeof(serveraddr.sin_addr));
          }

		printf("Client: connecting...");

		// Request a connection to the server
    rc = connect(sd, (struct sockaddr *)&serveraddr, sizeof(serveraddr));
    if (rc<0) {
      perror("Error: Client couldn't connect!");
    }
		printf("Done\n");
		
		// Create message
		char* message = "test";
		// recievingBuffer must be at least the size of the message + data end mark
		rbufSize = 256;
		char recievingBuffer[rbufSize];
		memset(recievingBuffer,0,rbufSize);
		// Send message
    rc = send(sd, message, sizeof(message), 0);
    if (rc<0) {
      perror("Error: Client couldn't send message!");
    }
		
		 fromSize = sizeof(fromAddr);
		 // @TODO: Build connection infastructure before messaging infastructure
		 // See last slide of lecture 2 for details
	   rc = recvfrom(sd, recievingBuffer, rbufSize, 0, (struct sockaddr*) &fromAddr, &fromSize);
	   // test error rc < 0 or rc == 0
	   if (rc<0) {
	     perror("Error: Client couldn't recieve message!");
	   }
	   else if (rc == 0) {
	     printf("Client's peer disconneted or sent a 0 byte message!\n");
	   }
		recievingBuffer[sizeof(rbufSize)] = '\0'; // Mark the end of the data for printf
		printf("Client received: %s\n", recievingBuffer);
// Clean up ///////////////////////////////////////////////////////////////////

      if (sd != -1)
        close(sd);
}
