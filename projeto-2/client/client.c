#include "client.h"

clock_t start, end;
double cpu_time_used;
int time_count = 0;
char* time_path;
FILE* time_output;

struct timeval t1, t2;
double elapsed;

char prot;
struct sockaddr *servaddr;
int len;

int main(int argc, char *argv[])
{
    int sock_udp, sock_tcp, rv;
    struct addrinfo hints, *p, *servers;
    struct timeval timeout;

    if (argc < 2) {
        fprintf(stderr,"usage: client hostname\n");
        exit(1);
    }

    if (argc == 3) {
      time_path = argv[2];
      time_output = fopen(time_path, "w");
    }

    // Alocate search parameters
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;

    // Search for TCP server
    hints.ai_socktype = SOCK_STREAM;
    if ((rv = getaddrinfo(argv[1], TCP_PORT, &hints, &servers)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // loop through all the TCP results and connect to the first we can
    for(p = servers; p != NULL; p = p->ai_next) {
        sock_tcp = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        // Check if socket was successfully created
        if (sock_tcp == -1) {
            perror("client: socket");
            continue;
        }
        // Check if TCP connection was successfully stablished
        if (connect(sock_tcp, p->ai_addr, p->ai_addrlen) == -1) {
            perror("client: connect");
            continue;
        }
        break;
    }
    // If no server within the specifications was found, end.
    if (p == NULL) {
        fprintf(stderr, "client: failed to connect\n");
        return 2;
    }
    freeaddrinfo(servers); // all done with this structure

    // Search for UDP servers
    hints.ai_socktype = SOCK_DGRAM;
    if ((rv = getaddrinfo(argv[1], UDP_PORT, &hints, &servers)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // loop through all the UDP results and use the first we can
    for(p = servers; p != NULL; p = p->ai_next) {
        if ((sock_udp = socket(p->ai_family, p->ai_socktype, 0)) == -1) {
            perror("client: socket");
            continue;
        } else break;
    }

    // If no server was found within the specidications, end.
    if (p == NULL) {
        fprintf(stderr, "client: failed to connect\n");
        return 2;
    }
    // Save destination server for UDP
    servaddr = p->ai_addr;

    timeout.tv_sec = 0;
    timeout.tv_usec = 100000;
    setsockopt(sock_udp, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

    // Test connections
    test_server('u', sock_udp, servaddr);
    test_server('t', sock_tcp, servaddr);

    // Make requests to udp and tcp servers
    make_request(sock_tcp, sock_udp, servaddr);

    freeaddrinfo(servers); // all done with this structure
    close(sock_tcp);
    close(sock_udp);

    return 0;
}

////////////////////////////////////////////////////////////////////////////////

void make_request(int sock_tcp, int sock_udp, struct sockaddr *servaddr) {
  char buffer[BUFFLEN];
  int i, socket;

  while(1) {
    // Scan and send user request
    printf("awaiting input:\n");
    scanf(" %[^\n]", buffer);

    // Set delay and starting time
    if (time_path)
      while(cpu_time_used = ((double) (clock() - end)) / CLOCKS_PER_SEC < 1);
    gettimeofday(&t1, NULL);

    // Select socket by protocoltest_server('t',sock_tcp, servaddr);
    prot = strtok(buffer, " ")[0];
    if      (prot == 't') socket = sock_tcp;
    else if (prot == 'u') {
      socket = sock_udp;
    } else if (prot == 'e' || prot == 'q') {
      strcpy(buffer,"e");
      transfer('t', 'w', sock_tcp, buffer, strlen(buffer), servaddr, &len);
      return;
    }

    // Remove protocol from message and send to server
    strcpy(buffer, &buffer[2]);
    transfer(prot, 'w', socket, buffer, strlen(buffer), servaddr, &len);

    // Await server commands
    switch (strtok(buffer, " ")[0]) {
      case '1':
        printf("awating profile...\n");
        receive_file(socket, buffer, strtok(NULL, " "));
        receive_data(socket, buffer);
        printf("\nprofile received\n");
        break;
      case 'f':
        transfer('u', 'r', socket, buffer, strlen(buffer), servaddr, &len);
        break;
      case 'e':
        return;
      default:
        printf("invalid option\n");
    }

    gettimeofday(&t2, NULL);
    elapsed = (t2.tv_sec - t1.tv_sec) + ((t2.tv_usec - t1.tv_usec)/1000000.0);
    printf("Real time: %lf\n", elapsed);
    end = clock();

    if (time_path) {
      if (errno == 11) {
        fprintf(time_output,"package lost.\n");
        errno = 0;
      } else {
        fprintf(time_output,"%lf\n", elapsed);
      }
    }
  }

  return;
}

// MESSAGE FUNCTIONS ///////////////////////////////////////////////////////////

// Receive N amount os messages to be printed in the terminal
void receive_data(int socket, char *buffer) {

  buffer[0] = 'x';
  while (buffer[0] != '\0') {  // print all messages
      if(transfer(prot, 'r', socket, buffer, BUFFLEN, servaddr, &len) < 0) return;
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

  if(transfer(prot, 'r', socket, buffer, BUFFLEN, servaddr, &len) < 0) return; // Read size
  size = strtol(buffer, NULL, 10);  // Cast size to long int

  while (i < size) {
    if(transfer(prot, 'r', socket, buffer, BUFFLEN, servaddr, &len) < 0) return; // Read a full buffer.
    for (base = i; (i < base + BUFFLEN) && (i < size); ++i)       // Write elements
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
