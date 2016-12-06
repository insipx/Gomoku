#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/socket.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <arpa/inet.h>

#include "../../lib/gips.h"

#include "../../lib/database.h"

void initp(Player **player, int userid, char *username, int wins, int losses, int ties);

//record player data to player's entry in struct
//if player doesn't exist, create an entry

int recPlayer(uint32_t uPID, BYTE PID, char *username, int isWin, Node *head, int sockfd, int
fd)
{

  Player *player;
  //if the player doesn't exist create a new player record, and add it
  //else just update the existing player record
  if(doesPlayerExist(&head, uPID, username) == false){

    player = calloc(1, sizeof(Player));
    initp(&player, uPID, username, isWin == PID, !(isWin == PID), 0 );
    head = add(fd, getIndex(fd), &head, &player);

  }else update(fd, &head, uPID, isWin==PID, !(isWin == PID), 0);


  player = getPlayer(uPID, fd, username, &head);
  player->userid = htonl(player->userid);
  player->wins = htonl(player->wins);
  player->losses = htonl(player->losses);
  player->ties = htonl(player->ties);
  player->index = htonl(player->index);
 
  if(send(sockfd, player, sizeof(Player), 0) == -1){
    free(player);
    return -1;
  }else{
    free(player);
    return 0;
  }
}

//index is set in update/add
//just an initializer function (pass by ref)
void initp(Player **player, int userid, char *username, int wins, int losses, int ties){
  Player *play = *((Player **) player);

  play->userid = userid;
  strncpy(play->username, username, 20);
  play->userid = userid;
  play->wins = wins;
  play->losses = losses;
  play->ties = ties;
}

