#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <sys/stat.h>
#include <dirent.h>



int c = -1;
char * strerror(int code);
int remove_directory(char const *name);

//match a long option for each short option
struct option longopts[] = {
    {"recursive", no_argument, NULL, 'r'},
    {"interactive", no_argument, NULL, 'i'},
    {"help", no_argument, NULL, 'h'},
    {0,0,0,0}
};

int main(int argc, char *argv[]) {
  int r = 0 ;
  int i = 0 ;
  errno=0;
  int rem = 0;
  char choice[20];

  //Check options and put the right flags
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
             "  -r, --recursive  \tRemove directories and their contents recursively\n" \
             "  -i, --interactive\tPrompt before every removal\n\n");
      exit(EXIT_SUCCESS);
      break;

    //if unknow option
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

  //if there are no options
  if ((r != 1) & (i != 1)) {
      rem = unlink(argv[optind]);
  }

  //if ls -i : interactive mode
  if ((r != 1) & (i ==1)) {
    //ask the user if really want to delete the file
    printf("Delete file %s? [yes/no] \n", argv[optind]);
    fgets(choice, sizeof choice, stdin);

    //if choice = yes
    if (strcmp(choice, "yes\n")==0  || strcmp(choice, "y\n")==0   ) {
      rem = unlink(argv[optind]);
    }
  }

  //print an error message if no file specified
  if (argc == 0) {
    printf("rm: missing operand\nEnter 'rm -h' for more information.\n");
    exit(EXIT_FAILURE);
  }

  //if rm -r : call remove_directory function
  if ((r == 1) & (i !=1)) {
    remove_directory(argv[optind]);
    exit(EXIT_SUCCESS);
  }

  if (rem == -1 && errno != 0) {
    (void)fprintf(stderr, "Error: %s.\n", strerror(errno));
  }

  return 0;
}

//function to remove directories and their contents recursively
int remove_directory(char const *name){

  DIR *directory;
  struct dirent *entry;
  struct stat file_stat;
  int statbit = 0;
  char buffer[1024] = {0};

  directory = opendir(name);


  if ( directory == NULL ) {
    fprintf(stderr, "Cannot open directory %s\n", name);
    return 0;
  }

  //loop on file entries
  while ((entry = readdir(directory)) != NULL) {

    //don't check directories "." and ".."
    if ( strcmp(entry->d_name, ".") == 0 ||strcmp(entry->d_name, "..") == 0 ) {
      continue;
    }

    //file path =  parent folder name/folder name
    snprintf(buffer, 1024, "%s/%s", name, entry->d_name);

    //check informations about the file
    stat(buffer, &file_stat);

    if (statbit == -1 && errno != 0) {
      (void)fprintf(stderr, "Error: %s.\n", strerror(errno));
    }

    //if file : remove
    if ( S_ISREG(file_stat.st_mode) ) {
      remove(buffer);
    }

    //if directory : call remove_directory again
    else if ( S_ISDIR(file_stat.st_mode) ) {
      remove_directory(buffer);
    }
  }
  //close the directory
  closedir(directory);
  //now, the directory is empty, i can remove it
  remove(name);
  /* TESTER VALEUR RETOUR*/
  return 1;
}
