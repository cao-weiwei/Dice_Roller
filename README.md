# Dice_Roller
 This is the project to implement a online dice roller game in C using socket programming in Linux.
 
The server process and the client process will run on two different machines and the communication between the two processes is achieved using Sockets.

The server-end is to do as follows :
- The server starts running before any clients
- When the server gets a client, it waits for another client (at least two players are needed)
- When the server gets the other client (now two clients), the dice game is going to begin.

The client-end is connected to the server, and:
- reads the server’s message (waits for the message starting the game),
- if the read message is "You can now play", client plays its dice, prints obtained score on its screen and sends it to the server
