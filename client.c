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


int main(int argc, char *argv[]){
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    

    struct sockaddr_in ipv4_addr;
    struct sockaddr_in server;
    int len = (sizeof(struct sockaddr_in));
    memset(&ipv4_addr, 0, len);

    ipv4_addr.sin_family=AF_INET;
    ipv4_addr.sin_port = htons(atoi(argv[2]));
    inet_pton(AF_INET, argv[1], &(ipv4_addr.sin_addr));

    
    struct sockaddr* serv_addr;
    
    serv_addr = (struct sockaddr*)&ipv4_addr;

    size_t len_buff = 500; //try to declare outside main

    char *msg = malloc(len_buff);
    char msg_status[3];

    while(1){
        recvfrom(sockfd, msg, len_buff, 0, (struct sockaddr*)&server, &len);
        memcpy(msg_status,msg,3);
        if(!strcmp(msg_status,"FYI")){
            //Display content of the board
        }
        else if(!strcmp(msg_status,"TXT")){
            //Display in console string contained in msg
        }
        else if(!strcmp(msg_status,"MYM")){
            //Ask user for a new move (column and row)
            //Send a MOV message to the server with the col. and row numbers
        }
        else if(!strcmp(msg_status,"END")){
            //Indicate winner to user (if any)
            //Close socket and terminate
        }
    }


//What I had in fancyclient: 

/*     int len_sent = 0;

    int stop =1;
    while(stop){
        int x = getline(&msg, &len_buff,stdin);
        if(x==1){
            stop =0;
        }
        else{
            msg+=x;
            len_sent+=x;
        }
        
    }

    sendto(sockfd, (void*) msg-len_sent, len_buff, 0, serv_addr, len);
    msg -=len_sent;
    free(msg);

    recvfrom(sockfd, msg, len_buff, 0, (struct sockaddr*)&server, &len);
    printf("Message received : %s",msg);
    free(msg);
    return 0; */
}