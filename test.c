#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>


int main(){
    char* msg="FYI 3 1 2 3 1 2 2 2 0 0";
    printf("%d", msg[4] - '0' );


}