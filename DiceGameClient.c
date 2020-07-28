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
#include <arpa/inet.h>
#include <time.h>

void action(){}

int main(int argc, char const *argv[]) {
        char message[255];
        int server, portNumber, pid, n;
        socklen_t len;

        long int ss = 0;
        int totalScore = 0;

        struct sockaddr_in servAdd;

        if ( argc != 3 ) {
                printf("Call model: %s <IP> <Port #>\n", argv[0]);
                exit(0);
        }

        if ( (server = socket(AF_INET, SOCK_STREAM, 0)) < 0 ) {
                fprintf(stderr, "Cannot create socket.\n");
                exit(1);
        }
        // The domain
        servAdd.sin_family = AF_INET;
        sscanf(argv[2], "%d", &portNumber);
        servAdd.sin_port = htons((uint16_t)portNumber);
        // string IP to network
        if ( inet_pton(AF_INET, argv[1], &servAdd.sin_addr) < 0 ) {
                fprintf(stderr, "inet_pton() has failed.\n");
                exit(2);
        }

        if ( connect(server, (struct sockaddr *) &servAdd, sizeof(servAdd)) < 0 ) {
                fprintf(stderr, "connect() has failed.\n");
                exit(3);
        }

        printf("*********************************\n");
        printf("*                               *\n");
        printf("*      Welcome to the Game      *\n");
        printf("*                               *\n");
        printf("*********************************\n\n");
        
        pid = fork();

        if (pid > 0) {
                sleep(1);
                signal(SIGUSR1, action);

                while (1) {     // reading server’s messages
                        read(server, message, 255);
                        fprintf(stderr, "Messages received from server: %s\n", message);
                        // check whether the game is terminated
                        if (!strcasecmp(message, "Your rival has left, You won the game !")) {
                                printf("I won the game!\n");
                                kill(pid, SIGTERM);
                                exit(0);
                        }
                        else if (!strcasecmp(message, "Game over: you won the game")) {
                                printf("I won the game!\n");
                                kill(pid, SIGTERM);
                                exit(0);
                        } else if (!strcasecmp(message, "Game over: you lost the game")) {
                                printf("I lost the game!\n");
                                kill(pid, SIGTERM);
                                exit(0);
                        } else if (!strcasecmp(message, "You can play now")) {  // sending signal to child to prepare throwing dice
                                kill(pid, SIGUSR1);
                                pause();
                        } else if (!strcasecmp(message, "")){
                                printf("Your rival has left, game over!\n");
                                kill(pid, SIGTERM);
                                exit(0);
                        }
                }
        } else {
                while (1) {     // sending messages to server
                        signal(SIGUSR1, action);        // got signal to throw dice
                        pause();
                        printf("I'm playing my dice...\n");
                        //int dice = (int)time(&ss) % 20 + 1;
                        srand((unsigned)time(NULL));
                        int dice = rand() % 20;
                        totalScore += dice;
                        printf("My obtained score: %d, total score:  %d.\n", dice, totalScore);

                        sprintf(message, "%d", dice);
                        write(server, message, strlen(message) + 1);
                        kill(getppid(), SIGUSR1);       // send signal to parent to wait from server
                        printf("waiting for server...\n\n\n");
                }
        }
}

