#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <time.h>
#include <sys/time.h>

#define BUFFLEN 256     // Length of the message buffer
#define PORT 3490       // the port users will be connecting to
#define BACKLOG 10      // how many pending connections queue will hold

// FUNCTIONS signatures
void request_options(int);
void get_profile(int, char*, char*);
void send_file(int, char*, char*);
void send_help(int, char*);
char* get_name(char*);
void send_data(int, char*, int);
void receive_file(int, char*, char*);
void get_all_profiles(int, char*);
void get_experience(int, char*, char*);
void add_experience(int, char*, char*);
void habilities_by_city(int, char*, char*);
void names_by_course(int, char*, char*);
char* get_line(FILE*, char*, int);
char* get_path(char*, char*, char);

// Debuggin wrapper for send
int write_d(int socket, char *buffer, int length){
  int i, r_val;

  // Fill message to standard size of buffer
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
      perror("ERROR: send");
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
