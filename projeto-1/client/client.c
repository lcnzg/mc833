#include "client.h"

clock_t start, end;
double cpu_time_used;
int time_count = 0;
char* time_path;
FILE* time_output;

struct timeval t1, t2;
double elapsed;

int main(int argc, char *argv[])
{
    int sockfd, rv;
    struct addrinfo hints, *p, *servers;
    if (argc < 2) {
        fprintf(stderr,"usage: client hostname\n");
        exit(1);
    }

    if (argc == 3) {
      time_path = argv[2];
      time_output = fopen(time_path, "w");
    }

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    if ((rv = getaddrinfo(argv[1], PORT, &hints, &servers)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // loop through all the results and connect to the first we can
    for(p = servers; p != NULL; p = p->ai_next) {
        sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (sockfd == -1) {
            perror("client: socket");
            continue;
        }

        if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            perror("client: connect");
            continue;
        }

        break;
    }

    if (p == NULL) {
        fprintf(stderr, "client: failed to connect\n");
        return 2;
    }

    printf("client: connecting...\n");
    freeaddrinfo(servers); // all done with this structure
    make_request(sockfd);
    close(sockfd);

    return 0;
}

////////////////////////////////////////////////////////////////////////////////

void make_request(int socket) {
  char buffer[BUFFLEN];
  int i;

  // receive server connection set confirmation
  read_d(socket, buffer);
  printf("%s\n", buffer);

  // receive help
  read_d(socket, buffer);
  printf("%s\n", buffer);


  while(1) {
    // Scan and send user request
    printf("awaiting input:\n");
    scanf(" %[^\n]", buffer);
    if (!strlen(buffer))
      exit(1);
    else if (time_path)
      while(cpu_time_used = ((double) (clock() - end)) / CLOCKS_PER_SEC < 1);

    gettimeofday(&t1, NULL);
    write_d(socket, buffer, strlen(buffer));

    // Await server commands
    switch (strtok(buffer, " ")[0]) {
      case '#':
        printf("awaiting file...\n");
        receive_file(socket, buffer, strtok(NULL, " "));
        break;
      case '1':
        printf("awaiting names by course...\n");
        while (buffer[0])
          receive_data(socket, buffer);
        printf("received names\n");
        break;
      case '2':
        printf("getting habilities by city...\n");
        while (buffer[0])
          receive_data(socket, buffer);
        printf("habilities received\n");
        break;
      case '3':
        printf("adding experience...\n");
        read_d(socket, buffer);
        printf("experience added\n");
        break;
      case '4':
        printf("awating \"%s\" experiences...\n", strtok(NULL, " "));
        while (buffer[0])
          receive_data(socket, buffer);
        printf("experiences received\n");
        break;
      case '5':
        printf("awating all profiles...\n");
        read_d(socket, buffer);
        while (buffer[0]) {
          receive_file(socket, buffer, buffer);
          receive_data(socket, buffer);
          read_d(socket, buffer);
        }
        printf("\nall profiles received\n");
        break;
      case '6':
        printf("awating profile...\n");
        receive_file(socket, buffer, strtok(NULL, " "));
        receive_data(socket, buffer);
        printf("\nprofile received\n");
        break;
      case 'h':
        while (buffer[0])
        receive_data(socket, buffer);
        break;
      case 'e':
        return;
      default:
        printf("invalid option\n");
    }

    gettimeofday(&t2, NULL);
    double elapsed = (t2.tv_sec - t1.tv_sec) + ((t2.tv_usec - t1.tv_usec)/1000000.0);
    printf("Real time: %lf\n", elapsed);
    end = clock();

    if (time_path) {
      fprintf(time_output,"%lf\n", elapsed);
    }
  }

  return;
}

// MESSAGE FUNCTIONS ///////////////////////////////////////////////////////////

// Receive N amount os messages to be printed in the terminal
void receive_data(int socket, char *buffer) {

  buffer[0] = 'x';
  while (buffer[0] != '\0') {  // print all messages
      read_d(socket, buffer);
      printf("%s\n", buffer);
  }

  return;
}

// FILE TRANSFER FUNCTIONS /////////////////////////////////////////////////////

// This function receives a file from the socket
void receive_file(int socket, char *buffer, char *path) {
  FILE *output;
  long int i = 0, base, size;
  char file_name[BUFFLEN];

  strcat(strcat(strcat(get_path(buffer), "data/"), strcpy(file_name, path)),".jpg");
  printf("\nreceving profile image: \"%s\"...\n", buffer);
  output = fopen(buffer, "wb"); // create/erase file to write

  read_d(socket, buffer);           // Read size
  size = strtol(buffer, NULL, 10);  // Cast size to long int

  while (i < size) {
    read_d(socket, buffer); // Read a full buffer.
    for (base = i; (i < base + BUFFLEN) && (i < size); ++i) // Write elements
      fputc(buffer[i%BUFFLEN], output);
  }

  printf("image received\n");
  fclose(output);
  return;
}

// Gets the full path of the file to be sent
char* get_path(char *path) {
  char szTmp[32];
  int bytes;

  sprintf(szTmp, "/proc/%d/exe", getpid()); // get this process origin file path
  bytes = readlink(szTmp, path, BUFFLEN);   // save full path

  for (bytes ; path[bytes] != '/'; --bytes); // removes the process name
  path[bytes+1] = '\0'; // add eof

  return path; // return path size and full path
}

// This was create solely for the purpose of testing the # option
char* get_name(char *path) {
  int i;

  for(i = strlen(path); i >= 0; --i)
    if (path[i] == '/') return &(path[i+1]);

  return path;
}
