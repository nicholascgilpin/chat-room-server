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
    // test error: sd<0

    memset(&serveraddr, 0, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(server_port);
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);

    rc = bind(sd,(struct sockaddr*) &serveraddr, sizeof(serveraddr));
    // test error rc<0

    rc = listen(sd,10);
    // test error rc<0

    printf("Ready for client connect().\n");

    sd2 = accept(sd, NULL, NULL);
    //test error sd2<0

    timeout.tv_sec = 30;
    timeout.tv_usec = 0;

    FD_ZERO(&read_fd);
    FD_SET(sd2, &read_fd);

    rc = select(sd2+1, &read_fd, NULL, NULL, &timeout);
    // test error rc < 0

    int length = buffer_length;

    rc = recv(sd2, buffer, sizeof(buffer),0);
    // test error rc < 0 or rc == 0 or rc < sizeof(buffer)

    rc = send(sd2, buffer, sizeof(buffer), 0);
    //test error rc < 0
  }while(FALSE);

  if(sd!=-1){
    close(sd);
  }
  if(sd2!=-1){
    close(sd2);
  }
}
