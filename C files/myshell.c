#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <ctype.h>
#include <string.h>
#include <errno.h>

char line[4096];
int flag = 0;

//show the prompt with username and number of the group
void show_invite(){
  char* username = getenv("USER");
  printf("%s@sh07 > ", username);
}

//put the user input (=command) in the global variable line
void check_line(){
  fgets(line, sizeof(line)-1, stdin);
}

//execute the command enter by the user
int execute_line() {
  pid_t pid, w;
  int status;

  //delete de final "/n" in line
  if (strchr(line, '\n'))
    *strchr(line, '\n') = 0;

  //close the shell if the user types "quit"
  if(!strcmp(line, "quit"))
    exit(1);

  //creation of a new processus (=son)
  pid = fork();

  if (pid < 0) {
   printf("Fork failed(%s)\n", strerror(errno));
   return 0;
  }

  //if pid ==0, i'm in the son
  if (pid == 0) {
    int i = 1;
    int ret;
    char **arguments;
    char path[50];

    arguments = malloc(100 * sizeof(char*));

    //stock all the arguments in a array
    arguments[0] = strtok(line, " ");
    while ((arguments[i] = strtok(NULL," ")) != NULL) {
      i= i+1;
    }

    //if flag: batch mode -> add "\0" at the end of the array
    if (flag==1) {
        arguments[i-1][strlen(arguments[i-1])-1] = '\0';
    }

    sprintf(path, "./%s", arguments[0]);

    //execute the corresponding executable + print an error and exit if it fail
    if ((ret = execvp(path, arguments)) == -1) {
      printf("Imposible to execute \"%s\" (%s)\n", line,strerror(errno));
      exit(1);
    }
  }
  //else, i'm in the father
  else{
    //wait the end of the son
    do {
      w = waitpid(pid, &status, WUNTRACED | WCONTINUED);
      if (w == -1) {
        perror("waitpid");
        exit(EXIT_FAILURE);
      }
      if (WIFEXITED(status)) {
        return 0;
      }
      else if (WIFSIGNALED(status)) {
        printf("Killed by signal %d\n", WTERMSIG(status));
      }
      else if (WIFSTOPPED(status)) {
        printf("Stopped by signal %d\n", WSTOPSIG(status));
      }
      else if (WIFCONTINUED(status)) {
        printf("Continued\n");
      }
    }
    while (!WIFEXITED(status) && !WIFSIGNALED(status));
    return 0;
	}
  return(1);
}

int main(int argc, char const *argv[]) {
  //if there are some arguments, i'm in batch mode
  if (argc != 1) {
    FILE *file;
    char line2[4096];
    errno=0;

    //open the file .sh wich contains some commands and associates a stream with it
    file = fopen(argv[optind], "r");

    if (file == 0)
      (void)fprintf(stderr, "Error: %s.\n", strerror(errno));
      exit(EXIT_FAILURE);

    //check line by line and execute the corresponding commands
    while(fscanf(file,"%[^\n]\n",line2)!=EOF){
      printf("\n --> %s\n", line2);
      strcpy(line, line2);
      flag = 1;
      execute_line();
    }

    fclose(file);
    exit(EXIT_SUCCESS);
  }

  //else, i'm in interactive modr
  else{

    while(1) {
      show_invite();
      check_line();
      execute_line();
    }
  }
  return 0;
}
