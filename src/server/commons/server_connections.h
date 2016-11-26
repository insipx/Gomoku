//
// Created by insi on 11/24/16.
//

#ifndef SERVER_SERVER_CONNECTIONS_H
#define SERVER_SERVER_CONNECTIONS_H
typedef struct connectionList {
  int sockfd;
  bool isPlaying;
  struct connectionList *next;
} cList;

//the head of the Linked List stores different information pertaining to the linked list
//like the size
//this is cool, because it can tell us how many connections
//we have 
//more information can be added in the future
//(this was more complicated than i thought to implement)

typedef struct headCList {
  int sockfd, size;
  bool isPlaying;
  struct connectionList *next;
} c_head;

void add(c_head **head, int sockfd);
void update(c_head **head, int sockfd);
void del(c_head **head, int sockfd);


#endif //SERVER_SERVER_CONNECTIONS_H
