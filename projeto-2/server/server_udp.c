#include "server.h"

clock_t start, end;
double cpu_time_used;
int time_count = 0;
char* time_path;
FILE* time_output;

struct timeval t1, t2;
double elapsed;

int len;
struct sockaddr_in cliaddr; // Client on current turn

int main(int argc, char *argv[]) {
    int sockfd;
    struct sockaddr_in servaddr;

    // Creating socket file descriptor
    if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    if (argc == 2) {
      time_path = argv[1];
      time_output = fopen(time_path, "w");
    }

    memset(&servaddr, 0, sizeof(servaddr));
    memset(&cliaddr, 0, sizeof(cliaddr));

    // Filling server information
    servaddr.sin_family    = AF_INET; // IPv4
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(UDP_PORT);

    // Bind the socket with the server address
    if ( bind(sockfd, (const struct sockaddr *)&servaddr,
            sizeof(servaddr)) < 0 )
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    request_options(sockfd);
    close(sockfd);

    if (time_path) fclose(time_output);

    return 0;
}

////////////////////////////////////////////////////////////////////////////////

void request_options(int socket) {
  char buffer[BUFFLEN];
  int n;

  while(1){
    // Await new message from client
    printf("server awaiting UDP message...\n");
    read_udp(socket, buffer, &cliaddr, &len);
    gettimeofday(&t1, NULL);

    // Test which request the client aksed for
    switch (strtok(buffer, " ")[0]) {
      case '1': // Get full profile
        printf("retrieving profile...\n");
        get_profile(socket, buffer, strtok(NULL, " "));
        printf("profile sent.\n");
        break;
      case 't':
        read_udp(socket, buffer, &cliaddr, &len);
        gettimeofday(&t2, NULL);
        write_udp(socket, buffer, strlen(buffer), cliaddr, len);
        break;
      default:
        gettimeofday(&t2, NULL);
        printf("invalid option\n");
    }

    elapsed = (t2.tv_sec - t1.tv_sec) + ((t2.tv_usec - t1.tv_usec)/1000000.0);
    printf("Real time: %lf\n", elapsed);

    if (time_path) {
      if (errno == 11) {
        fprintf(time_output,"package lost.\n");
        errno = 0;
      } else {
        fprintf(time_output,"%lf\n", elapsed);
      }
      fflush(time_output);
    }
  }

  return;
}

// OPTIONS FUNCTIONS ///////////////////////////////////////////////////////////

void get_profile(int socket, char* buffer, char *buff_email) {
  FILE *fptr;
  int line = 0;
  char email[BUFFLEN], tag[BUFFLEN];
  char* tags[] = {"Nome: \0","Sobrenome: \0","Residência: \0", "Formação acadêmica: \0",
                  "Habilidades: \0","Experiências: \0", "              \0"};


  strcpy(email, buff_email); // Copy email key from buffer

  // Gets image from server
  get_path(buffer, email, 'i');
  send_file(socket, buffer, buffer);

  // Gets the values in the txt file
  get_path(buffer, email, 't');

  if ((fptr = fopen(buffer,"r")) == NULL){
      printf("Error! opening file: %s\n", buffer);
      exit(1); // Exits if failed to open file
  }

  // Send contents from file
  while (fgets(buffer, BUFFLEN, fptr)) {
    strcpy(tag, tags[line]);
    strcat(tag, buffer);
    if (write_udp(socket, tag, strlen(tag), cliaddr, len) < 0)
      return;
    if (line < 6) ++line;
  }

  gettimeofday(&t2, NULL);
  write_udp(socket, buffer, 0, cliaddr, len);

  return;
}

// FILE TRANSFER FUNCTIONS /////////////////////////////////////////////////////

// This function splits files from /data/ and send then to the client
void send_file(int socket, char *buffer, char *full_path) {
  FILE *input;           // File to be sent
  long int i = 0, size;  // Size of the file to be sent

  input = fopen(full_path, "rb");
  printf("sending file \"%s\"\n", get_name(full_path));

  // Get size (amount of char in the file)
  fseek(input, 0, SEEK_END);
  size = ftell(input);
  fseek(input, 0, SEEK_SET);

  sprintf(buffer, "%ld", size);             // Cast size to string
  if (write_udp(socket, buffer, strlen(buffer), cliaddr, len) < 0) return;

  while (i < size) { // reads char by char filling buffer until eof
    buffer[(i++)%BUFFLEN] = fgetc(input); // Add char to buffer, then incremente i
    if (i%BUFFLEN == 0 || i == size)      // i buffer full or EOF send data
      if (write_udp(socket, buffer, BUFFLEN, cliaddr, len) < 0) return;
  }

  gettimeofday(&t2, NULL);
  printf("file sent\n");
  fclose(input);
  return;
}

// PATH HANDLING FUNCTIONS ////////////////////////////////////////////////////

// Gets the full path of the file to be sent
char* get_path(char* path, char* file_name_buff, char id) {
  char szTmp[32], file_name[BUFFLEN];
  int bytes;

  strcpy(file_name, file_name_buff);        // make sure file_name wont be overwritten
  sprintf(szTmp, "/proc/%d/exe", getpid()); // get this process origin file path
  bytes = readlink(szTmp, path, BUFFLEN);   // save path

  for (bytes ; path[bytes] != '/'; --bytes); // removes the process name
  path[bytes+1] = '\0'; // add eof

  if (id == 'i')
    strcat(strcat(strcat(path, "data/images/"), file_name), ".jpg");
  else if (id == 't')
    strcat(strcat(strcat(path, "data/"), file_name), ".txt");

  return path; // return path size and full path
}

// This was create solely for the purpose of testing the # option
char* get_name(char *path) {
  int i;

  for(i = strlen(path); i >= 0; --i)
    if (path[i] == '/') return &(path[i+1]);

  return path;
}
