#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>


char * strerror(int code);
int c = -1;
void show();

//match a long option for each short option
struct option longopts[] = {
    {"number", no_argument, NULL, 'n'},
    {"show-ends", no_argument, NULL, 'e'},
    {"help", no_argument, NULL, 'h'},
    {0,0,0,0}
};

int main(int argc , char *argv[]){

  int n = 0 ;
  int e = 0 ;
  errno=0;
  int i=1;
  int nbr = 1 ;
  char *ffile, *sfile;
  ffile= argv[optind];


  //Check options and put the right flags
  while ((c = getopt_long(argc, argv, "neh", longopts, NULL)) != -1) {
    switch (c) {
    case 'n':
      n = 1;
      break;

    case'e':
      e = 1;
      break;

    case'h':
      printf("\nThis command concatenate files and print on the standard output\n\n"
             "  ----- Options -----\n" \
             "  -n, --number   \tNumber all output lines\n" \
             "  -e, --show-ends\tDisplay $ at end of each line\n\n");
      printf("argv:  %s\n",argv[1]);
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

  //print the first file on the standard output
  show(ffile,n,e,&nbr);

  //if there are other files: concatenate files and print on the standard output
  if ((argc-optind) > 1) {
    for (; i < (argc-optind); i++) {
      sfile= argv[optind+i];
      show(sfile,n,e,&nbr);
    }
  }
  return(0);
}

//print the content of the files
void show(char *f, int nbit, int ebit, int *nbr) {

  FILE *file;
  char line[2400];

  //open file and associates a stream with it
  file = fopen(f,"r");

  if (file == 0)
    (void)fprintf(stderr, "Error: %s.\n", strerror(errno));
     exit(EXIT_FAILURE);
     
  // reading file line by line entering to buffer line
  while(fscanf(file,"%[^\n]\n",line)!=EOF)
  {
    //if cat -n -e : display "&" and the number of the lines
    if (nbit == 1 && ebit == 1) {
      printf("$   %d.%s\n",*nbr, line);
      *nbr = *nbr +1;
    }

    //if cat -n
    else if (nbit == 1 && ebit != 1) {
      printf("%d.%s\n",*nbr, line);
      *nbr = *nbr +1;
    }

    //if cat -e
    else if (nbit !=1 && ebit == 1) {
      printf("$   %s\n", line);
    }

    //if there are no options
    else{
      printf("%s\n", line);
    }

  }
  fclose(file);
}
