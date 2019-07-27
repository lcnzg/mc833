#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <time.h>
#include <sys/time.h>

#define BUFFLEN 256 // Length of the message buffer
#define TCP_PORT "3490" // the port client will be connecting to
#define UDP_PORT "8080"

typedef struct sockaddr* sap;

// Funcions signatures
int test_server(char, int, sap);
void receive_file(int, char*, char*);
void make_request(int, int, sap);
char* get_name(char*);
void send_file(int, char*, char*);
void receive_data(int, char*);
char* get_path(char*);

// TCP WRAPPERS ////////////////////////////////////////////////////////////////
// Debuggin wrapper for send
int write_d(int socket, char *buffer, int length){
  int i, r_val;

  // Fiil message to standard size of buffer
  for (i = length; i < BUFFLEN; ++i) buffer[i] = '\0';

  if ((r_val = send(socket, buffer, BUFFLEN, 0)) == -1) {
    perror("ERROR: send");
    exit(1);
  } else if (r_val == 0) {
    printf("ERROR: pairing socket is closed\n");
    exit(1);
  }

  return r_val;
}

// Debuggin wrapper for recv
int read_d(int socket, char *buffer) {
  int r_val, total = 0;

  while (total != BUFFLEN) {
    if ((r_val = recv(socket, &buffer[total], (BUFFLEN - total), 0)) == -1) {
      perror("ERROR: recv");
      exit(1);
    } else if (r_val == 0) { // if client not responding
      printf("ERROR: pairing socket is closed\n");
      exit(1);
    } else {
      total += r_val;
    }
  }

  return total;
}

// UDP SEND AND RECEIVE WRAPPERS ///////////////////////////////////////////////
// Debuggin wrapper for sendto
int write_udp(int socket, char *buffer, int length, sap target){
  int i, r_val;

  // Fill message to standard size of buffer
  for (i = length; i < BUFFLEN; ++i) buffer[i] = '\0';

  if ((r_val = sendto(socket, (const char*)buffer, BUFFLEN, MSG_CONFIRM, target, sizeof(struct sockaddr))) == -1) {
    perror("ERROR: sendto");
    exit(1);
  } else if (r_val == 0) {
    printf("ERROR: pairing socket is closed\n");
    exit(1);
  }

  return r_val;
}

// Debuggin wrapper for recvfrom
int read_udp(int socket, char *buffer, sap sender, int* sender_len) {
  int r_val, total = 0;

  while (total != BUFFLEN) {
    if ((r_val = recvfrom(socket, &buffer[total], (BUFFLEN - total), MSG_WAITALL,
                          sender, sender_len)) == -1) {
      if (errno != 11){
        printf("ERROR: message might be lost/corrupted.\n");
        return -1;
      } else {
        printf("client: reached timeout (package might have been lost).\n");
        return -1;
      }
    } else if (r_val == 0) { // if client not responding
      printf("ERROR: pairing socket is closed\n");
      return -1;
    } else {
      total += r_val;
    }
  }

  return total;
}

// UDP||TCP SELECTOR WRAPPER ////////////////////////////////////////////////////
int transfer(char prot, char op, int sock, char *buff, int len, sap pair, int* pair_len) {
  int r_val = -1;
  if      (prot == 't' && op == 'w') r_val = write_d(sock, buff, len);
  else if (prot == 'u' && op == 'w') r_val = write_udp(sock, buff, len, pair);
  else if (prot == 't' && op == 'r') r_val = read_d(sock, buff);
  else if (prot == 'u' && op == 'r') r_val = read_udp(sock, buff, pair, pair_len);
  else    printf("Unknown protocol/operation!\n");
  return r_val;
}

// This function verifies if both servers are correctly set up ////////////////
int test_server(char prot, int socket, struct sockaddr *servaddr){
  int len;
  char buffer[BUFFLEN];

  strcpy(buffer,"t");
  transfer(prot, 'w', socket, buffer, strlen(buffer), servaddr, &len);
  if (prot == 'u') strcpy(buffer,"Testing UDP.");
  else strcpy(buffer,"Testing TCP.");
  transfer(prot, 'w', socket, buffer, strlen(buffer), servaddr, &len);
  transfer(prot, 'r', socket, buffer, strlen(buffer), servaddr, &len);

  return 1;
}
