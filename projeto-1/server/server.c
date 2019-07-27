#include "server.h"

clock_t start, end;
double cpu_time_used;
int time_count = 0;
char* time_path;
FILE* time_output;

struct timeval t1, t2;
double elapsed;

int main(int argc, char *argv[]){
  int sockfd, new_fd, pid;  // listen on sock_fd, new connection on new_fd
  struct sockaddr_in server, client;
  int sin_size;

  if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
    perror("gateway socket");
    exit(1);
  }

  if (argc == 2) {
    time_path = argv[1];
    time_output = fopen(time_path, "w");
  }

  memset(&server, 0, sizeof server);
  server.sin_family = AF_INET;
  server.sin_port = htons(PORT);
  server.sin_addr.s_addr = htonl(INADDR_ANY);

  if (bind(sockfd, (struct sockaddr *)&server, sizeof(server)) == -1) {
      perror("server: bind");
      exit(1);
  }

  if (listen(sockfd, BACKLOG) == -1) {
    perror("listen");
    exit(1);
  }

  printf("server: waiting for connections...\n");
  while(1) {  // main accept() loop
      sin_size = sizeof client;
      new_fd = accept(sockfd, (struct sockaddr *)&client, &sin_size);
      if(new_fd == -1) {
          perror("accept");
          exit(1);
      }

      printf("server: got connection\n");
      if (!fork()) { // this is the child process
          close(sockfd); // child doesn't need the listener
          request_options(new_fd); // Communication function
          close(new_fd);
          exit(0);
      }
      close(new_fd);  // parent doesn't need this
  }

  return 0;
}

////////////////////////////////////////////////////////////////////////////////

void request_options(int socket) {
  char buffer[BUFFLEN];

  // notify connections is set
  strcpy(buffer, "connection is set...\n");
  write_d(socket, buffer, strlen(buffer));

  // notify connections is set
  strcpy(buffer, "Type help for instructions");
  write_d(socket, buffer, strlen(buffer));

  while(1){
    // Await new message from client
    printf("server awaiting new message...\n");
    read_d(socket, buffer);
    gettimeofday(&t1, NULL);

    // Test which request the client aksed for
    switch (strtok(buffer, " ")[0]) {
      case '#':
        printf("sending file...\n");
        send_file(socket, buffer, strtok(NULL, " "));
        break;
      case '1':
        printf("retrieving name by course...\n");
        names_by_course(socket, buffer, &buffer[strlen(buffer)+1]);
        printf("names retrieved\n");
        break;
      case '2':
        printf("retrieving habilities by city...\n");
        habilities_by_city(socket, buffer, strtok(NULL, " "));
        printf("habilities retrieved\n");
        break;
      case '3':
        printf("adding experience...\n");
        add_experience(socket, buffer, strtok(NULL, " "));
        break;
      case '4':
        printf("retrieving experiences...\n");
        get_experience(socket, buffer, strtok(NULL, " "));
        printf("experiences retrieved\n");
        break;
      case '5':
        printf("sending all profiles...\n");
        get_all_profiles(socket, buffer);
        printf("all profiles sent\n");
        break;
      case '6': // Get full profile
        printf("retrieving profile...\n");
        get_profile(socket, buffer, strtok(NULL, " "));
        printf("profile sent.\n");
        break;
      case 'h':
        printf("sending help info...\n");
        send_help(socket, buffer);
        break;
      case 'e':
        return;
      default:
        printf("invalid option\n");
    }

    double elapsed = (t2.tv_sec - t1.tv_sec) + ((t2.tv_usec - t1.tv_usec)/1000000.0);
    printf("Real time: %lf\n", elapsed);

    if (time_path) {
      fprintf(time_output,"%lf\n", elapsed);
    }
    // End connection if requested by client
    if (!strcmp(buffer, "exit")) break;
  }

  return;
}

// OPTIONS FUNCTIONS ///////////////////////////////////////////////////////////

void names_by_course(int socket, char* buffer, char* course_b) {
  FILE *index, *profile;
  char course[BUFFLEN], email[BUFFLEN];

  strcpy(course, course_b);
  index = fopen(get_path(buffer, "index", 't'), "r");

  while (fgets(email, BUFFLEN, index)) {
    email[strlen(email)-1] = '\0';
    profile = fopen(get_path(buffer, email, 't'), "r");
    get_line(profile, buffer, 4);
    printf("%s graduated in |%s|%s|\n", email, buffer, course);

    if (!strcmp(buffer, course)) {
      sprintf(buffer, "\"%s\" name:\n", email);
      write_d(socket, buffer, strlen(buffer));
      get_line(profile, buffer, 1);
      strcat(buffer, " ");
      get_line(profile, &buffer[strlen(buffer)], 2);
      strcat(buffer, "\n");
      write_d(socket, buffer, strlen(buffer));
    }

    fclose(profile);
  }

  gettimeofday(&t2, NULL);
  write_d(socket, buffer, 0); // Send empty buffer to signal eof

  fclose(index);
  return;
}

void habilities_by_city(int socket, char* buffer, char* city_b) {
  FILE *index, *profile;
  char city[BUFFLEN], email[BUFFLEN];

  strcpy(city, city_b);

  index = fopen(get_path(buffer, "index", 't'), "r");

  while (fgets(email, BUFFLEN, index)) {
    email[strlen(email)-1] = '\0';
    profile = fopen(get_path(buffer, email, 't'), "r");
    get_line(profile, buffer, 3);
    printf("%s lives in %s\n", email, buffer);

    if (!strcmp(buffer, city)) {
      sprintf(buffer, "\"%s\" habilities:\n", email);
      write_d(socket, buffer, strlen(buffer));
      get_line(profile, buffer, 5);
      strcat(buffer, "\n");
      write_d(socket, buffer, strlen(buffer));
    }

    fclose(profile);
  }

  gettimeofday(&t2, NULL);
  write_d(socket, buffer, 0); // Send empty buffer to signal eof

  fclose(index);
  return;
}

void add_experience(int socket, char* buffer, char* email_b) {
  FILE *profile;
  char email[BUFFLEN], exp[BUFFLEN];
  int i = 0, start = 5;

  strcpy(email, email_b);
  strcpy(exp, &email_b[strlen(email_b)+1]);
  profile = fopen(get_path(buffer, email, 't'), "a+");
  printf("adding \"%s\" to profile \"%s\"\n", exp, email);

  while(get_line(profile, buffer, (++i)+start));
  fprintf(profile, "(%d)%s\n", i, exp);

  gettimeofday(&t2, NULL);
  write_d(socket, buffer, 0);

  fclose(profile);

  return;
}

void get_experience(int socket, char* buffer, char* email) {
  FILE *profile;
  char path[BUFFLEN];
  int i = 6;

  profile = fopen(get_path(path, email, 't'), "r");


  while (get_line(profile, buffer, i++))
    write_d(socket, strcat(buffer, "\n"), strlen(buffer)+1);

  gettimeofday(&t2, NULL);
  write_d(socket, buffer, 0); // Send empty buffer to sinal eof

  fclose(profile);
  return;
}

void get_all_profiles(int socket, char *buffer) {
  FILE *index;

  get_path(buffer, "index", 't');
  index = fopen(buffer, "r");

  while (fgets(buffer, BUFFLEN, index)) {
    buffer[strlen(buffer)-1] = '\0';
    printf("sending profile: %s\n", buffer);
    write_d(socket, buffer, strlen(buffer)); // send profile email
    get_profile(socket, buffer, buffer);     // send profile
  }

  gettimeofday(&t2, NULL);
  write_d(socket, buffer, 0); // Send empty buffer to signal eof


  return;
}

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
    write_d(socket, tag, strlen(tag));
    if (line < 6) ++line;
  }

  gettimeofday(&t2, NULL);
  write_d(socket, buffer, 0); // Send empty buffer to sinal eof

  return;
}

void send_help(int socket, char *buffer) {
  FILE *help;

  get_path(buffer, "help", 't');
  help = fopen(buffer, "r");

  while (fgets(buffer, BUFFLEN, help)) {
    buffer[strlen(buffer)-1] = '\0';
    printf("sending: %s\n", buffer);
    write_d(socket, buffer, strlen(buffer));
  }

  gettimeofday(&t2, NULL);
  write_d(socket, buffer, 0); // Send empty buffer to signal eof

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
  write_d(socket, buffer, strlen(buffer));  // Send file size to client

  while (i < size) { // reads char by char filling buffer until eof
    buffer[(i++)%BUFFLEN] = fgetc(input); // Add char to buffer, then incremente i
    if (i%BUFFLEN == 0 || i == size)      // i buffer full or EOF send data
      write_d(socket, buffer, BUFFLEN);   // sends entire buffer to avoid border issues
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

// FILE SEARCH FUNCTIONS //////////////////////////////////////////////////////

// Retrieves a specific entry from the profile
char* get_line(FILE* profile, char* buffer, int line) {
  int i, position = ftell(profile);

  fseek(profile, 0, SEEK_SET);
  for (i = 1; i < line; ++i) fgets(buffer, BUFFLEN, profile);
  buffer = fgets(buffer, BUFFLEN, profile);
  fseek(profile, position, SEEK_SET);

  if (buffer && buffer[strlen(buffer)-1] == '\n')
    buffer[strlen(buffer)-1] = '\0';

  return buffer;
}
