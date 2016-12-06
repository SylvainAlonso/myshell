#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>


#define handle_error(msg) \
do { perror(msg); exit(EXIT_FAILURE); } while (0)


int c = -1;
char * strerror(int code);


struct option longopts[] = {
    {"recursive", no_argument, NULL, 'r'},
    {"interactive", no_argument, NULL, 'i'},
    {"help", no_argument, NULL, 'h'},
    {0,0,0,0}
};

int main(int argc, char *argv[]) {
  int r = 0 ;
  int i = 0 ;
  int h = 0;
  errno=0;
  int rem = 0;
  char choice[20];

  while ((c = getopt_long(argc, argv, "rih", longopts, NULL)) != -1) {
    switch (c) {
    case 'r':
      r = 1;
      break;

    case'i':
      i = 1;
      break;

    case'h':
      printf("\nThis command remove files or directories\n\n"
             "  ----- Options -----\n" \
             "  -r\tRemove directories and their contents recursively\n" \
             "  -i\tPrompt before every removal\n\n");
      exit(EXIT_SUCCESS);
      break;

    case '?':
      if (isprint (optopt))
        fprintf (stderr, "Unknown option '-%c'.\n", optopt);
      else
        fprintf (stderr, "Unknown option character! '\\x%x'.\n", optopt);

      return 1;

    default:
      return 1;
    }
  }
  argc -= optind;

  if (r != 1 & i != 1) {
      rem = unlink(argv[optind]);
  }

  if (r != 1 & i ==1) {
    printf("Delete file %s? [yes/no] \n", argv[optind]);
    fgets(choice, sizeof choice, stdin);

    if (strcmp(choice, "yes\n")==0  || strcmp(choice, "y\n")==0   ) {
      rem = unlink(argv[optind]);
    }

    else{
    }
  }

  if (r == 1 & i !=1) {
    remove_directory(argv[optind]);
  }

  if (argc == 0) {
    printf("rm: missing operand\nEnter 'rm -h' for more information.\n");
    exit(EXIT_FAILURE);
  }

  if (rem == -1 && errno != 0) {
    (void)fprintf(stderr, "Error: %s.\n", strerror(errno));
  }

  return 0;
}

int remove_directory(char const *name){

  DIR *directory;
  struct dirent *entry;
  struct stat file_stat;
  char buffer[1024] = {0};
  directory = opendir(name);
  int statbit = 0;

  if ( directory == NULL ) {
    fprintf(stderr, "Cannot open directory %s\n", name);
    return 0;
  }

  while ((entry = readdir(directory)) != NULL) {

    if ( strcmp(entry->d_name, ".") == 0 ||strcmp(entry->d_name, "..") == 0 ) {
      continue;
    }

    snprintf(buffer, 1024, "%s/%s", name, entry->d_name);

    stat(buffer, &file_stat);

    if (statbit == -1 && errno != 0) {
      (void)fprintf(stderr, "Error: %s.\n", strerror(errno));
    }

    if ( S_ISREG(file_stat.st_mode) ) {
      remove(buffer);
    }
    else if ( S_ISDIR(file_stat.st_mode) ) {
      remove_directory(buffer);
    }
  }

  closedir(directory);
  remove(name);
  /* TESTER VALEUR RETOUR*/
  return 1;
}
