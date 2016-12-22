#include <dirent.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <time.h>
#include <getopt.h>
#include <errno.h>
#include <ctype.h>

#define GREEN "\033[32m"
#define YELLOW    "\033[33m"
#define DEFAULT  "\033[0m"
#define BUF_SIZE 1024


int c = -1;
int fd,fc,size, size2;
char buf[BUF_SIZE];
char buf2[BUF_SIZE];
struct linux_dirent *d;
struct linux_dirent *d2;
int bpos;
int bpos2;
char d_type;



void check_directory();

char * strerror(int code);
struct stat sb;
struct linux_dirent {
    unsigned long  d_ino;
    unsigned long  d_off;
    unsigned short d_reclen;
    char           d_name[];
};

//match a long option for each short option
struct option longopts [] = {
  {"all", no_argument , NULL , 'a' },
  {"long", no_argument , NULL , 'l' },
  {"recursive", no_argument , NULL , 'R' },
  {"help", no_argument , NULL , 'h' },
  { 0, 0, 0, 0 }
};

int main(int argc, char *argv[]) {
  int a = 0 ;
  int l = 0 ;
  int r = 0 ;
  int fd = 0 ;
  errno=0;

  //Check options and put the right flags
  while ((c = getopt_long(argc, argv, "alRh", longopts, NULL)) != -1) {
    switch (c) {
    case 'a':
      a = 1;
      break;

    case 'l':
      l = 1;
      break;

    case'R':
      r = 1;
      break;

    case'h':
      printf("\nThis command list information about the files (the current directory by default)\n\n"
             "  ----- Options -----\n" \
             "  -r, --recursive\tList subdirectories recursively.\n" \
             "  -a, --all      \tDo not hide entries starting with.\n" \
             "  -l, --long     \tDisplay more informations about the files.\n\n");
      exit(EXIT_SUCCESS);
      break;

    //if unknow option
    case '?':
      if (isprint (optopt))
        fprintf (stderr, "Unknown option `-%c'.\n", optopt);
      else
        fprintf (stderr, "Unknown option character! `\\x%x'.\n", optopt);

      return 1;

    default:
      return 1;
    }
  }
  char *param = argv[optind];

  //if there are some parameters, check the fd of the selected directory
  if ( param != NULL) {
    char path[50];
    sprintf(path, "%s", argv[optind]);
    fd = open(path , O_RDONLY | O_DIRECTORY);
  }

  //else, check the fd of the current directory
  else
    fd = open(".", O_RDONLY | O_DIRECTORY);

  if (fd == -1)
    (void)fprintf(stderr, "Error: %s.\n", strerror(errno));

  while (1) {

    //place several linux_dirent structures in buf
    size = syscall(SYS_getdents, fd, buf, BUF_SIZE);

    if (size == -1)
      (void)fprintf(stderr, "Error: %s.\n", strerror(errno));

    if (size == 0)
      break;

    //check what's inside the directory
    for (bpos = 0; bpos < size; ) {
      d = (struct linux_dirent *) (buf + bpos);
      d_type = *(buf + bpos + d->d_reclen - 1);

      //if there are no options
      if ((*d->d_name != '.') & (a != 1) & (l != 1) & (r!=1)) {

        //if there is a directory, print in yellow
        if (d_type == DT_DIR){
          printf(YELLOW"%10s"DEFAULT, d->d_name);
        }

        //else, print in green
        else{
          printf(GREEN"%10s"DEFAULT, d->d_name);
        }

      }

      //if ls -all : do not ignore entries starting with '.'
      if (a==1){
        if (d_type == DT_DIR){
          printf(YELLOW"%10s"DEFAULT, d->d_name);
        }
        else{
          printf(GREEN"%10s"DEFAULT, d->d_name);
        }
      }

      //if ls -l : display more informations about the files
      if ((*d->d_name != '.') & (l == 1)) {

        //check the permissions (owner, group, others)
        if (stat(d->d_name, &sb) == -1) {
          (void)fprintf(stderr, "Error: %s.\n", strerror(errno));
          exit(EXIT_FAILURE);
        }
        char info [] = {'-','-','-','-','-','-','-','-','-','-','\0'};

        if (S_IFDIR & sb.st_mode) {
          info[0] = 'd';
        }
        if (S_IRUSR & sb.st_mode) {
          info[1] = 'r';
        }
        if (S_IWUSR & sb.st_mode) {
          info[2] = 'w';
        }
        if (S_IXUSR & sb.st_mode) {
          info[3] = 'x';
        }
        if (S_IRGRP & sb.st_mode) {
          info[4] = 'r';
        }
        if (S_IWGRP & sb.st_mode) {
          info[5] = 'w';
        }
        if (S_IXGRP & sb.st_mode) {
          info[6] = 'x';
        }
        if (S_IROTH & sb.st_mode) {
          info[7] = 'r';
        }
        if (S_IWOTH & sb.st_mode) {
          info[8] = 'w';
        }
        if (S_IXOTH & sb.st_mode) {
          info[9] = 'x';
        }

        if (d_type == DT_DIR){
          printf(YELLOW"%10s"DEFAULT, d->d_name);
        }
        else{
          printf(GREEN"%10s"DEFAULT, d->d_name);
        }

        printf ("%12s", info);
        //print user
        printf("     %ld   %ld",(long) sb.st_uid, (long) sb.st_gid);
        // print size (bytes)
        printf("%10lld",(long long) sb.st_size);
        //print date of last modification
        printf("%30s", ctime(&sb.st_mtime));

      }

      //if ls -r : list subdirectories recursively
      if ((*d->d_name != '.') & (r == 1)) {
        if (d_type != DT_DIR){
          printf(GREEN"%10s\n"DEFAULT, d->d_name);
        }
        else{
          printf(YELLOW"    %s\n"DEFAULT, d->d_name);
          printf("-----------\n");
          //if directory in directory: call check_directory function
          check_directory();
        }
      }
      //incrementation of bpos to check the next file
      bpos += d->d_reclen;
    }
    printf("\n" );
    exit(EXIT_SUCCESS);
    close(fd);
  }
  return(1);
}
//same concept as previously
void check_directory(int fd2, int size2,int bpos2) {

  fd2 = open(d->d_name, O_RDONLY | O_DIRECTORY);

  if (fd2 == -1)
    (void)fprintf(stderr, "Error: %s.\n", strerror(errno));

  size2 = syscall(SYS_getdents, fd2, buf2, BUF_SIZE);

  if (size2 == -1)
    (void)fprintf(stderr, "Error: %s.\n", strerror(errno));

  for (bpos2 = 0; bpos2 < size2; ) {

    d2 = (struct linux_dirent *) (buf2 + bpos2);
    d_type = *(buf2 + bpos2 + d2->d_reclen-1);
    if (*d2->d_name != '.'){

      if (d_type == DT_DIR){
        printf(YELLOW"%s\n"DEFAULT, d2->d_name);
      }
      else{
        printf(GREEN"%s\n"DEFAULT, d2->d_name);
      }
    }
    bpos2 += d2->d_reclen;
  }
  close(fd2);
  printf("-----------\n");
}
