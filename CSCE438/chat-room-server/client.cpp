#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h> //memset
#include <unistd.h> //close
#include <stdio.h>  //printf

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
///////////////////////////////////////////////////////////////////////////////
int main(int argc, char* argv[]){
  int sd=-1, rc, bytesReceived;
  char buffer[buffer_length];
  char server[NETDB_MAX_HOST_NAME_LENGTH];
  struct sockaddr_in serveraddr;
  struct hostent* hostp;

  do{
    sd = socket(AF_INET, SOCK_STREAM, 0);
    if (sd<0) {
      perror("Error: Client couldn't create socket!");
    }

    if(argc>1) strcpy(server,argv[1]);
    else strcpy(server,server_name);

    memset(&serveraddr, 0, sizeof(serveraddr));
    serveraddr.sin_family      = AF_INET;
    serveraddr.sin_port        = htons(server_port);
    serveraddr.sin_addr.s_addr = inet_addr(server);

    if (serveraddr.sin_addr.s_addr == (unsigned long)INADDR_NONE)      {
       hostp = gethostbyname(server);
       if (hostp == (struct hostent *)NULL) {
          printf("Host not found --> ");
          break;
       }
       memcpy(&serveraddr.sin_addr,
                    hostp->h_addr,
                    sizeof(serveraddr.sin_addr));
          }

    rc = connect(sd, (struct sockaddr *)&serveraddr, sizeof(serveraddr));
    if (rc<0) {
      perror("Error: Client couldn't connect!");
    }

    memset(buffer, 'a', sizeof(buffer));
    rc = send(sd, buffer, sizeof(buffer), 0);
    if (rc<0) {
      perror("Error: Client couldn't send message!");
    }

     bytesReceived = 0;
     while (bytesReceived < buffer_length) {
           rc = recv(sd, & buffer[bytesReceived],
                  buffer_length - bytesReceived, 0);
           // test error rc < 0 or rc == 0
           if (rc<0) {
             perror("Error: Client couldn't recieve message!");
           }
           else if (rc == 0) {
             printf("Client's peer disconneted or sent a 0 byte message!\n");
           }
            bytesReceived += rc;
      }

  } while (false);

      if (sd != -1)
        close(sd);
}
