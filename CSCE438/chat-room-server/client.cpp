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
int deleteRoom(Message *packet, int sd){
/*	
	int server_port = 6005;
	int rbufSize; // recieving buffer
	int NETDB_MAX_HOST_NAME_LENGTH = 512;
	char* server_name = (char*)"sun.cs.tamu.edu";
	int rc, bytesReceived;
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
	strcpy(server,server_name);

		// create socket and store id in sd
	close(sd);
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
			printf("Available commands:\
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
				 printf("Request succeeded!\n");
				 switch(packet.type){
					 case 1:
						deleteRoom(&packet);
					 case 2:
						joinRoom(&packet, sd);
					 case 3:
						printf("Creating room at port: %d", packet.port); 
					default:
						printf( "Packet out of range: %d", packet.type);
			 }
			 printf("Received bytes %d\nReceived string \"%s\"\n", bytecount, (Message*)packet.text);
		}
	}

// Clean up ///////////////////////////////////////////////////////////////////
	printf("Client: Shutting down!\n");
	if (sd != -1)
		close(sd);
	*/
}

// Description: Return -1 for error
int joinRoom(Message *packet, int sd){
	int server_port = packet->port;
	int rbufSize; // recieving buffer
	int NETDB_MAX_HOST_NAME_LENGTH = 512;
	char* server_name = (char*)"sun.cs.tamu.edu";
	int rc, bytesReceived;
	char server[NETDB_MAX_HOST_NAME_LENGTH];
	struct sockaddr_in serveraddr; // server address
	struct sockaddr_in fromAddr; // address of whoever replies to client
	uint fromSize; // size of the address from whoever replies to client
	struct hostent* hostp; // used to store DNS address information about server
	int * p_int;
	int bytecount;
	char buffer[1024];	
	int buffer_length = 0;
	
	printf("Initializing socket");
	close(sd);
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
    	printf("Connected to new Room!\n");
			printf("Instructions:\nStart your line with a colon to send a command\n");
			printf("Availible commands:\
			\n :join roomname \
			\n :create roomname \
			\n :delete roomname\
			\n Example - Enter a colon, your command, and the roomname \n\
			\n :join room5\n");
    }
	
	Message joinpacket;
    while(1){
    	int buffer_length = 1024;
		int packet_length = sizeof(Message);
    	memset(buffer, '\0', buffer_length);
		joinpacket.port = 0;
			
    	printf("\nPlease begin typing: ");
    	fgets(buffer, 1024, stdin);
		buffer[strlen(buffer)-1]='\0';
			
			// Let server know to treat message as command or text
			if (buffer[0] == ':'){
				joinpacket.type = 2;
			}
			else{
				joinpacket.type = 0;
			}
			memcpy(&joinpacket.text, &buffer, buffer_length); // put text in packet

			 
			 if( (bytecount=send(sd, &joinpacket, packet_length, 0))== -1){
			     fprintf(stderr, "Error sending data");
			     exit(1);
			 }
			 printf("Sent bytes %d\n", bytecount);

			 if((bytecount = recv(sd, &joinpacket, packet_length, 0))== -1){
			     fprintf(stderr, "Error receiving data");
			     exit(1);
			 }
			 if(joinpacket.type == -1){
				 printf("Request failed!\n");
			 }
			 else if ((joinpacket.type == 1) || joinpacket.type == 2 || joinpacket.type == 3){
				 printf("Request succeeded!\n");
				 switch(joinpacket.type){
					 case 1:
						deleteRoom(&joinpacket, sd);
						break;
					 case 2:
						joinRoom(&joinpacket, sd);
						break;
					 case 3:
						printf("Creating room at port: %d", packet->port);
						break;
					default:
						printf( "Packet out of range: %d", joinpacket.type);
						break;
			 }
			 printf("Received bytes %d\nReceived string \"%s\"\n", bytecount, (Message*)joinpacket.text);
		}
	}

// Clean up ///////////////////////////////////////////////////////////////////
	printf("Client: Shutting down!\n");
	if (sd != -1)
		close(sd);
}
//////////////////////////////////////////////////////////////////////////////

int main(int argc, char* argv[]){
	int server_port = 6005;
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
			printf("Available commands:\
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
				 printf("Request succeeded!\n");
				 switch(packet.type){
					 case 1:
						deleteRoom(&packet, sd);
						break;
					 case 2:
						joinRoom(&packet, sd);
						break;
					 case 3:
						printf("Creating room at port: %d\n", packet.port); 
						break;
					default:
						printf( "Packet out of range: %d\n", packet.type);
			 }
			 printf("Received bytes %d\nReceived string \"%s\"\n", bytecount, (Message*)packet.text);
		}
	}

// Clean up ///////////////////////////////////////////////////////////////////
	printf("Client: Shutting down!\n");
	if (sd != -1)
		close(sd);
}
