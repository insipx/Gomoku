/* Program: Programming Assignment 4, Binary Files
 * author: Andrew Plaza
 * Date: Sept 29, 2016
 * File Name: asgn4-plazaa2.c
 * compile: cc -o asgn4.out asgn4-plazaa2.c -g -Wall
 * run: ./asgn4.out
 * debug: gdb ./asgn4.out
 *
 * This C program accepts player records from the keyboard.
 * '+ userid lastname firstname wins losses ties' to add
 * '* userid wins losses ties' to update
 * '? userid' to query
 * '#' to terminate program
 * Records are read and then written in binary form to a file.
 * There is an option to load this file the next time the program is run.
 * The program keeps data relating to where each record is in a file with a linked list (struct node).
 * Upon termination the records and linked list are printed.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "andrews-db-prog.h"
#include "gips.h"

//method definitions (same order as methods in file)

void readp(int fd, int index, Player *play);
void writep(int fd, int index, Player *play);

//read in previous records

void ask_input(int fd, int index, Node **head);



void query(int fd, Node **head);

void kill(Node **head);
void die(const char *message);

//LL = Linked List

//SAMPLE MAIN (DO NOT DELETE)
/*int main(int argc, char *argv[]) {

  int index = 0, fd = 0;
  char *filename;
  Node *head = NULL; //head node initialized in main


  if (argc <= 1){
  fprintf(stderr, "Usage: './asgn4.out filename'\n");
  exit(1);
  }else{

// y/n, if it's not y or Y then it's assumed to be no
printf("Load data from previous file (if it exists)? [Y/n] ");
char c = getchar();

if(c == 'y' || c == 'Y'){
//allocate memory so avoid buffer overflow, need filename for stat
filename = (char*)malloc(strlen(argv[1])+1);
memset(filename, 0, sizeof(*filename));
strcpy(filename, argv[1]);
fd = open(argv[1], O_RDWR|O_CREAT, S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);
persist(fd, &index, &head, filename);
free(filename);

//if no add O_TRUNC to clear any previous data
}else fd = open(argv[1], O_TRUNC|O_RDWR|O_CREAT, S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);
}

if( fd < 0) die("[ERROR] open failed");



ask_input(fd, index, &head);

//end sequence
close(fd);
print_nodes(&head);
kill(&head);
printf("END\n");
exit(EXIT_SUCCESS);
}
*/


void readp(int fd, int index, Player *play){
  lseek(fd, index*sizeof(Player),0);
  if(read(fd, play, sizeof(Player)) == -1)
    die("[ERROR] read failed");
}

void writep(int fd, int index, Player *play){
  struct player rec = *play;
  lseek(fd, index*sizeof(Player), 0);
  if(write(fd, &rec, sizeof(Player)) == -1)
    die("[ERROR] write failed");
}

void persist(int fd, int *index, Node **head, char *filename){
  int size, i;
  int temp_i = *index;

  Node *temp = *head;
  struct stat st;
  struct player rec;

  //stat file size, convert to index
  stat(filename, &st);
  size = st.st_size/sizeof(Player);
  if (size == 0) return;

  for(i = 0; i < (size); i++){
    Node *newn = calloc(1, sizeof(Node));

    readp(fd, temp_i, &rec);
    newn->userid = rec.userid;
    newn->index = temp_i;
    insert(&temp, newn);
    temp_i++;
  }
  *index = temp_i;
  *head = temp;
}


//print functions
void print_nodes(Node **head){
  Node *node = *head;
  printf("The linked list: ");
  while(node != NULL){
    printf("(%d, %d)", node->userid, node->index);
    node = node->next;
  }
  printf("\n");
}

void print_players(int fd, Node **head){
  Node *temp = *head;
  while(temp != NULL){
    printp(fd, temp->index);
    temp = temp->next;
  }
}

//takes file and index of player
//reads with readp into player struct
//prints it
void printp(int fd, int index){
  struct player play;
  readp(fd, index, &play);

  printf("%d, %s, %d, %d, %d \n", play.userid,
      play.username,
      play.wins,
      play.losses,
      play.ties);
  return;

}


//get first character in input,
//go to method to do what user asks
/*void ask_input(int fd, int index, Node **head){

  Node *temp = *head;
  char c;

//make input look nicer
printf("> ");

do{
c = getchar();

if(c == '+'){
 *head = add(fd, index, &temp);
 index++;
 }
 else if(c == '*')
 update(fd, &temp);
 else if (c == '?')
 query(fd, &temp);
 else if (c == '\n');
//ignore newline chars, reduced 'Error Invalid Input'
else if (c == '#'){
printf("TERMINATE\n");
print_players(fd, &temp);
return;
}
}while(c != '#');

return;
}*/


//creates a new noed (calloc) and writes it to binary file
//inserts it into struct
Node* add(int fd, int index, Node **head, int userid, 
    int wins, int losses, int ties, char* username){

  Node *temp = *head;
  struct player prec;
  //initialize mem to 0
  memset(&prec, 0, sizeof(prec));
  
  //set variables 
  prec.userid = userid;
  strncpy(prec.username, username, 19);
  prec.wins = wins;
  prec.losses = losses;
  prec.ties = ties;

  while(temp != NULL){
    if(temp->userid == prec.userid){
      printf("ERROR - userid exists. Did you mean to update?\n");
      printp(fd, temp->index);
      return *head;
    }else temp = temp -> next;
  }

  //reset temp after iterating through LL
  temp = *head;

  prec.index = index;
  //write node to binary file
  writep(fd, index, &prec);

  printf("%s", "ADD: ");
  printp(fd, prec.index);

  Node *newNode = (Node*)malloc(sizeof(Node));
  memset(newNode, 0, sizeof(*newNode));

  newNode->userid = prec.userid;
  newNode->index = prec.index;

  insert(&temp, newNode);
  *head = temp;
  //make input look nicer

  return *head;
}


//inserts node into LL
void insert(Node **head, Node *newNode){
  Node *temp = *head;

  if(temp == NULL){
    *head = newNode;
    return;
  }

  if(newNode->userid < temp->userid) {
    //if need to move head back must do it outside loop
    newNode->next = *head;
    *head = newNode;
    return;
  }

  //else we can just insert it into the right pos in the list
  Node *curr;
  curr = temp;

  while(temp->next !=NULL) {
    temp = temp -> next;

    if(newNode->userid < temp->userid) {
      curr->next = newNode;
      newNode->next=temp;
      return;
    }
    //if we are at the end of the list
    curr = temp;
  }

  if(curr->next == NULL){
    curr->next = newNode;
    newNode->next = NULL;
    return;
  }
}

Player* update(int fd, Node **head, int userid, int wins, int losses, int ties){

  Node *temp = *head;
  //declare struct, don't need to use memory on the heap
  Player *prec = calloc(1, sizeof(Player));


  //find data for playerid user entered, change info
  while(temp != NULL){
    if(temp->userid == userid) {
      printf("%s", "BEFORE: ");
      printp(fd, temp->index);

      readp(fd, temp->index, prec);
      prec->userid = userid;
      prec->wins += wins;
      prec->losses += losses;
      prec->ties += ties;

      writep(fd, temp->index, prec);

      printf("%s", "AFTER: ");
      printp(fd, temp->index);
      //make input look nicer

      return prec;
    }else{
      temp = temp->next;
    }
  }
  printf("ERROR - player does not exist.");

  //make input look nicer
  return NULL;
}

void query(int fd, Node **head){
  int userid;
  Node *temp = *head;
  scanf("%d", &userid);

  //find data user needs based on given ID
  //print that player data
  while(temp != NULL){
    if(temp->userid == userid) {
      printf("QUERY: ");
      printp(fd, temp->index);
      //make input look nicer
      printf("> ");

      return;
    }else{
      temp = temp->next;
    }
  }
  //ERROR
  printf("%s\n", "ERROR - player does not exist.");

  //make input look nicer
  printf("> ");

  return;
}

//free memory starting from head
void kill(Node **head){
  Node *node = *head;
  Node *temp;
  while(node != NULL) {
    temp = node;
    node = node->next;
    temp->next = NULL;
    free(temp);
  }
  *head = NULL;
}

//throws a meaningful error message if something goes wrong
//useful, especially in file I/O
void die(const char *message){
  if(errno)
    perror(message);
  else
    printf("ERROR: %s\n", message);
  exit(1);
}


int doesPlayerExist(Node **head, int uPID, char *username, int fd){
  Node *temp = *head;
  char t_username[21];
  strncpy(t_username, username, 20);

  //find data user needs based on given ID
  //print that player data
  while(temp != NULL){
    if(temp->userid == uPID) {
    Player play;
    
    readp(fd, temp->index, &play);
    
    if(strncmp(play.username, username, 20) != 0)
      return FALSE;

    }else{
      temp = temp->next;
    }
  }

  return FALSE;
}


int getIndex(int fd){
  int index=0;
  struct player rec;
  int rd = readnp(fd, index, &rec);
  if(rd == 0 || rd == -1) return index;

  while (rd != -1 || rd != 0){
    index++;
    rd = readnp(fd, index, &rec);
  }
  return index;
}

int readnp(int fd, int index, Player *play){
  lseek(fd, index*sizeof(Player), 0);
  return read(fd, play, sizeof(Player));
}

Player* getPlayer(int uPID, int fd, char *username, Node **head){


  Node *temp = *head;
  char t_username[21];
  strncpy(t_username, username, 20);

  //find data user needs based on given ID
  //print that player data
  
  Player *play = calloc(1, sizeof(Player));

  while(temp != NULL){
    if(temp->userid == uPID) {
      readp(fd, temp->index, play);
    }else{
      temp = temp->next;
    }
  }
  return play;

}
