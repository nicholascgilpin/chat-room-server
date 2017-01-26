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
	int server_port = 7005;
	int rbufSize; // recieving buffer
	int NETDB_MAX_HOST_NAME_LENGTH = 512;
	char* server_name = (char*)"sun.cs.tamu.edu";
	int sd=-1, rc, bytesReceived;
	char server[NETDB_MAX_HOST_NAME_LENGTH];
	struct sockaddr_in serveraddr; // server address
	struct sockaddr_in fromAddr; // address of whoever replies to client
	uint fromSize; // size of the address from whoever replies to client
	struct hostent* hostp; // used to store DNS address information about server
	int * p_int;
	int bytecount;
	char buffer[1024];	
	int buffer_length = 0;

	// Read command line arguments if present
	if(argc>1) strcpy(server,argv[1]);
	else strcpy(server,server_name);

		// create socket and store id in sd
    sd = socket(AF_INET, SOCK_STREAM, 0);
    if (sd<0) {
      perror("Error: Client couldn't create socket!");
      exit(1);
    }

		// Figure out client's from address and server to address
    memset(&serveraddr, 0, sizeof(serveraddr));
    serveraddr.sin_family      = AF_INET;
    serveraddr.sin_port        = htons(server_port);
    serveraddr.sin_addr.s_addr = INADDR_ANY;
   // y_addr.sin_addr.s_addr = INADDR_ANY;

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
      exit(1);
    }
    else{
    	printf("Connected to Server!\n");
			printf("Instructions:\nStart your line with a colon to send a command\n");
			printf("Availible commands:\
			\n :join roomname \
			\n :create roomname \
			\n :delete roomname\
			\n Example - Enter a colon, your command, and the roomname \n\
			\n :join room5\n");
    }

		Message packet;
    while(1){
    	int buffer_length = 1024;
			int packet_length = sizeof(Message);
    	memset(buffer, '\0', buffer_length);
			packet.port = 0;
			
    	printf("\nPlease begin typing: ");
    	fgets(buffer, 1024, stdin);
			buffer[strlen(buffer)-1]='\0';
			
			// Let server know to treat message as command or text
			if (buffer[0] == ':'){
				packet.type = 2;
			}
			else{
				packet.type = 0;
			}
			memcpy(&packet.text, &buffer, buffer_length); // put text in packet

			 
			 if( (bytecount=send(sd, &packet, packet_length, 0))== -1){
			     fprintf(stderr, "Error sending data");
			     exit(1);
			 }
			 printf("Sent bytes %d\n", bytecount);

			 if((bytecount = recv(sd, &packet, packet_length, 0))== -1){
			     fprintf(stderr, "Error receiving data");
			     exit(1);
			 }
			 if(packet.type == -1){
				 printf("Request failed!\n");
			 }
			 else if ((packet.type == 1) || packet.type == 2 || packet.type == 3){
				 printf("Request succeded!\n");
			 }
			 printf("Recieved bytes %d\nReceived string \"%s\"\n", bytecount, (Message*)packet.text);
	}
    
/*
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
		for (size_t i = 0; i < 10; i++) {
			sleep(1); // maintain connection for 10 Secs, so that we can test multiple connections
		}
		*/
// Clean up ///////////////////////////////////////////////////////////////////
	printf("Client: Shutting down!\n");
	if (sd != -1)
		close(sd);
}
