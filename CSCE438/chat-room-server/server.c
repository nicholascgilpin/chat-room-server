#include <sys/socket.h>
#include <netdb.h>
#include <string.h> //memset
#include <unistd.h> //close
#include <stdio.h>  //printf

const int server_port = 3005;
const int buffer_length = 250;
const int FALSE = 0;

void main(){
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
  }while(FALSE);

  if(sd!=-1){
    close(sd);
  }
  if(sd2!=-1){
    close(sd2);
  }
}
