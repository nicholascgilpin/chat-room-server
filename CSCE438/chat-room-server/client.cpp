#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <pthread.h>
#include <stdint.h>
#include <iostream>
#include <string.h> //memset
#include <unistd.h> //close
#include <stdio.h>  //printf
#include <cstdlib>
using namespace std;

// Constants //////////////////////////////////////////////////////////////////
const int server_port = 3005;
const int buffer_length = 250;
const int NETDB_MAX_HOST_NAME_LENGTH = 512;
#define server_name "compute.cs.tamu.edu"

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
  }  
 }
}

///////////////////////////////////////////////////////////////////////////////
int main(int argc, char* argv[]){
  int sd=-1, rc, bytesReceived, length;
  char buffer[buffer_length];
  char server[NETDB_MAX_HOST_NAME_LENGTH];
  struct sockaddr_in serveraddr, clientaddr;
  struct hostent* hostp;
  char name[20];
  pthread_t rThread;
  
 // printf("Howdy! Welcome to the TAMU Chat service. Please enter your username to continue: ");
  //scanf("%s", name);
  
  
	//Make Socket
    sd = socket(AF_INET, SOCK_STREAM, 0);

    if (sd<0) {
      perror("Error: Client couldn't create socket!\n");
    }

    if(argc>1) strcpy(server,argv[1]);
    else strcpy(server,server_name);

	struct hostent *hostentPtr;
    hostentPtr = gethostbyname(server_name);
    if(hostentPtr == NULL) {
        cout << "ERROR: Server Host Name Could Not Be Resolved... Program is Terminating...\n";
        return 0;
    }
	
    memcpy(&serveraddr.sin_addr, hostentPtr->h_addr, sizeof(serveraddr.sin_addr));
	//memset(&serveraddr, 0, sizeof(serveraddr));
    serveraddr.sin_family      = AF_INET;
    serveraddr.sin_port        = htons(server_port);
    serveraddr.sin_addr.s_addr = INADDR_ANY;
	

	
	
  /*  if (serveraddr.sin_addr.s_addr == (unsigned long)INADDR_NONE)      {
       hostp = gethostbyname(server);
       if (hostp == (struct hostent *)NULL) {
          printf("Host not found --> ");
          exit(1);;
       }
	}*/
    /*   memcpy(&serveraddr.sin_addr,
                    hostp->h_addr,
                    sizeof(serveraddr.sin_addr));
          }*/

    rc = connect(sd, (struct sockaddr *)&serveraddr, sizeof(serveraddr));
		
    if (rc<0) {
      perror("Error: Client couldn't connect!\n");
	  exit(1);;
    }
	else{
		printf("Building Connection, please wait...\n");
		printf("Connection Successful.\n");
	}

    memset(buffer, 0, buffer_length);
	printf("Welcome to the chat room! You may now begin your conversations\n.");
	
	rc = pthread_create(&rThread, NULL, msg, (void *) sd);
	if (rc) {
		printf("ERROR: Return Code from pthread_create() is %d\n", rc);
		exit(1);
	}

	
  /*  rc = send(sd, buffer, sizeof(buffer), 0);
    if (rc<0) {
      perror("Error: Client couldn't send message!");
    }
//TODO: Append username to beginning of buffer
     bytesReceived = 0;*/
     while (fgets(buffer, sizeof(buffer), stdin) != NULL) {
		rc = sendto(sd, buffer, buffer_length, 0, (struct sockaddr *) &serveraddr, sizeof(serveraddr));  
           // test error rc < 0 or rc == 0
           if (rc<0) {
             perror("Error: Client couldn't recieve message!\n");
           }
           else if (rc == 0) {
             printf("Client's peer disconneted or sent a 0 byte message!\n");
           }
      }

 close(sd);
 pthread_exit(NULL);
 
 return 0;    
}