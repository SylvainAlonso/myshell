#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <ctype.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>

char line[4096];

void show_invite(){
  char* username = getenv("USER");
  printf("%s@shell > ", username);
}

void check_line(){
  if (line == NULL)
    exit(0);

  fgets(line, sizeof(line)-1, stdin);
}

int execute_line() {
  pid_t pid, w;
  int status;

  pid = fork();

  if (strchr(line, '\n'))
    *strchr(line, '\n') = 0;

  if (pid < 0) {
   printf("Fork failed(%s)\n", strerror(errno));
   return;
  }


  if (pid == 0) {
    int i = 1;
    char **arguments;
    char path[50];

    arguments[0] = strtok(line, " ");
    while ((arguments[i] = strtok(NULL," ")) != NULL) {
      i= i+1;
    }

    sprintf(path, "./%s", arguments[0]);
    execvp(path, arguments);

    printf("Imposible to execute \"%s\" (%s)\n", line,strerror(errno));
    exit(1);
  }

  else{
    do {
      w = waitpid(pid, &status, WUNTRACED | WCONTINUED);
      if (w == -1) {
        perror("waitpid");
        exit(EXIT_FAILURE);
      }

      if (WIFEXITED(status)) {
        //printf("exited, status=%d\n", WEXITSTATUS(status));
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
}


int main(int argc, char const *argv[]) {

  if (argc != 1) {

    exit(EXIT_SUCCESS);
  }

  else{

    while(1) {
      show_invite();
      check_line();
      execute_line();
    }
  }
  return 0;
}
