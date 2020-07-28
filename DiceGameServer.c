#include <arpa/inet.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/shm.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netinet/ip.h>

void servicePlayers(int sd1,int sd2) {
    int round = 1;
    int count = 0;
    int total1 = 0;
    int total2 = 0;
    char *message0 = "Game over: you won the game";
    char *message1 = "Game over: you lost the game";
    char *mes = "Your rival has left, You won the game !";

    while (1) { 
        struct tcp_info info1;
        struct tcp_info info2;
        int len1=sizeof(info1);
        int len2=sizeof(info2);
        getsockopt(sd1, IPPROTO_TCP, TCP_INFO, &info1, (socklen_t *)&len1);
        getsockopt(sd2, IPPROTO_TCP, TCP_INFO, &info2, (socklen_t *)&len2);
        if ((info1.tcpi_state == TCP_ESTABLISHED) && (info2.tcpi_state == TCP_ESTABLISHED)) {
            if(count == 0){
                char message[255];
                int n, pid;
                count = count + 1;
                write(sd1, "You can play now", 17);
                if( (n = read(sd1, message, 255)) ) {
                    message[n] = '\0';
                    printf("*************Round %d*************\n",round);
                    round++;
                    printf("TOTO GETS: %s\n", message);
                    total1 = total1 + atoi(message);
                    printf("TOTO'S Total Score: %d\n\n",total1);
                    if(total1 >= 100 || total2>= 100) {
                        write(sd1, message0, strlen(message0) + 1);
                        write(sd2, message1, strlen(message1) + 1);
                        printf("*********************************\n\n");
                        printf("Game Over!\n");
                        printf("TOTO wins\n");
                        exit(0);
                    }
                }
            sleep(1);
            } else {
                char message[255];
                int n, pid;
                count = count - 1;
                write(sd2, "You can play now", 17);
                if( (n = read(sd2, message, 255)) ) {
                    message[n] = '\0';

                    printf("TITI GETS: %s\n", message);
                    total2 = total2 + atoi(message);
                    printf("TITI'S Total Score: %d\n\n",total2);
                    printf("*********************************\n\n");
                    if(total2 >= 100 || total1 >= 100) {
                        write(sd2, message0, strlen(message0) + 1);
                        write(sd1, message1, strlen(message1) + 1);
                        printf("*********************************\n\n");
                        printf("Game Over!\n");
                        printf("TITI wins\n");
                        exit(0);
                    }
                }
                sleep(1);
            }
        }else if((info1.tcpi_state == TCP_ESTABLISHED) && (info2.tcpi_state != TCP_ESTABLISHED)) {
            printf("TITI LEFT ! TOTO WINS !\n");
            write(sd1, mes, strlen(mes) + 1);
            exit(0);
        } else {
            printf("TOTO LEFT ! TITI WINS !\n");
            write(sd2, mes, strlen(mes) + 1);
            exit(0);
        }
    }
}

int main(int argc, char *argv[]) {
    int sd, client1,client2, portNumber, status;
    socklen_t len;
    struct sockaddr_in servAdd;

    if(argc != 2){  // argument error
            printf("Call model: %s <Port #>\n", argv[0]);
            exit(0);
    }

    // AF_INET for internet
    // SOCK_STREAM for TCP
    // 0 for use the correct/default protocol
    if( (sd = socket(AF_INET, SOCK_STREAM, 0)) < 0 ){       // create socket error
            printf("Cannot create socket\n");
            exit(1);
    }

    servAdd.sin_family = AF_INET;
    servAdd.sin_addr.s_addr = htonl(INADDR_ANY);
    sscanf(argv[1], "%d", &portNumber);
    servAdd.sin_port = htons((uint16_t)portNumber);

    bind(sd,(struct sockaddr*)&servAdd,sizeof(servAdd));
    listen(sd, 1);

    printf("*********************************\n");
    printf("*                               *\n");
    printf("*      Welcome to the Game      *\n");
    printf("*                               *\n");
    printf("*********************************\n\n");
    while(1) {
        client1 = accept(sd, (struct sockaddr *) NULL, NULL);
        printf("\n\nGot a client, wait for the other one.\n");
        client2 = accept(sd, (struct sockaddr *) NULL, NULL);
        printf("Got the second client ! Let's play!\n\n");
        struct tcp_info info1;
        struct tcp_info info2;
        int len1=sizeof(info1);
        int len2=sizeof(info2);
        getsockopt(client1, IPPROTO_TCP, TCP_INFO, &info1, (socklen_t *)&len1);
        getsockopt(client2, IPPROTO_TCP, TCP_INFO, &info2, (socklen_t *)&len2);
        if(client1 > 0 && client2 > 0){
            if((info1.tcpi_state == TCP_ESTABLISHED) && (info2.tcpi_state == TCP_ESTABLISHED)) {
                if(!fork()){
                    servicePlayers(client1,client2);
                }
                    close(client1);
                    close(client2);
                    waitpid(0, &status, WNOHANG);
                } else {
                    printf("one of the players has left...Cannot start Game.\n");
                    close(client1);
                    close(client2);
                }
            }
        }
}
