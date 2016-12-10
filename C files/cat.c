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

char * strerror(int code);
  int c = -1;

  void test();

struct option longopts[] = {
    {"number", no_argument, NULL, 'n'},
    {"show-ends", no_argument, NULL, 'e'},
    {"help", no_argument, NULL, 'h'},
    {0,0,0,0}
};

int main(int argc , char *argv[]){



  errno=0;
  int n = 0 ;
  int e = 0 ;
  errno=0;

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



  char *ffile, *sfile;
  ffile= argv[optind];
  int i=1;



  int nbr = 1 ;

  test(ffile,n,e,&nbr);


  if ((argc-optind) > 1) {
    for (i; i < (argc-optind); i++) {
      sfile= argv[optind+i];
      test(sfile,n,e,&nbr);
    }

  }


  return(0);
}

void test(char *f, int nbit, int ebit, int *nbr) {

  FILE *file;
  char line[2400];

  file = fopen(f,"r");

  while(fscanf(file,"%[^\n]\n",line)!=EOF)
  {
    if (nbit == 1 && ebit == 1) {
      printf("$   %d.%s\n",*nbr, line);
      *nbr = *nbr +1;
    }

    else if (nbit == 1 && ebit != 1) {
      printf("%d.%s\n",*nbr, line);
      *nbr = *nbr +1;
    }

    else if (nbit !=1 && ebit == 1) {
      printf("$   %s\n", line);
    }

    else{
      printf("%s\n", line);
    }

  }
  fclose(file);
}
