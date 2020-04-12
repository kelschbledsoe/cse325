// Gabriella Kelsch Bledsoe
// Project 3
// Code to copy contents of a file to another

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <errno.h>
#include <limits.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
int main( int argc, char* argv[] )
{
  // Citing gnu.org tutorial on getopt, in section 25.2.2, as a resource for the
  // assignment. Argument stored in variable optarg. Gets option character not
  // included in options -> returns ? and sets optopt to the character
  int aflag = 0;
  int bflag = 0;
  int tflag = 0;
  int buf_size = 256;
  bool found_buf = false;
  char *fileA = NULL;
  char *fileB = NULL;
  char last_flag;
  // Loop over arguments to process
  for (int i=1; i<argc; i++)
  {
    if (argv[i][0] == '-')
    {
      if (argv[i][1] == 'b')
      {
        bflag++;
        // Convert input to an int
        char *p;
        errno = 0;
        long conv = strtol(argv[i+1], &p, 10);
        // Error checking
        if (errno != 0 || *p != '\0' || conv > INT_MAX)
        {
          printf("Invalid input! b flag detected but not followed by an integer.\n");
          exit(0);
        } 
        else
        {
          buf_size = conv;
          found_buf = true;
          if (buf_size < 1)
          {
            printf("Invalid buffer size of %i\n",buf_size);
            exit(0);
          }
          if (bflag > 1)
          {
            printf("Multiple b flags specified. Using the last buffer size specified which was %i\n", buf_size);
          }
        }
      }
      else if (argv[i][1] == 't')
      {
        tflag++;
        last_flag = 't';
      }
      else if (argv[i][1] == 'a')
      {
        aflag++;
        last_flag = 'a';
      }
      else
        printf("Error, not a valid flag \n");
    }
    else if (found_buf == true)
    {
      found_buf = false;
    }
    else 
    {
      if (fileA == NULL)
      {
        fileA = argv[i];
      }
      else if (fileA != NULL && fileB == NULL)
      {
        fileB = argv[i];
      }
      else
      {
        printf("Error, only specify two files!\n");
        exit(0);
      }
    }
  } 
  // Now working on actually copying the file
  // Read returns the number of characters which were copied into the buffer
  // need loop so can keep reading and writing until finish file
  // buffer is how many characters transfer at a time
  
  // Testing if the source file exists. If it doesn't, exit
  int fileAexists = open(fileA, O_EXCL|O_CREAT, S_IRUSR|S_IWUSR);
  if (fileAexists != -1)
  {
    printf("Source file does not exist!\n"); 
    exit(0);
  }
  
  // Handling if given multiple a and t flags. Use the last flag read.
  if (aflag > 1 || tflag > 1)
  {
    if (last_flag == 't')
    {
      tflag = 1;
      aflag = 0;
    }
    else if (last_flag == 'a')
    {
      aflag = 1;
      tflag = 0;
    }
    printf("Multiple flags specified. Using the last flag specified which was %c\n", last_flag);
  }
  char buf[buf_size];
  int size = 1;
  // This open will fail if fileB already exists
  // Open returns -1 when it fails
  int fileBexists = open(fileB, O_EXCL|O_CREAT, S_IRUSR|S_IWUSR);
  // fileB exists
  if (fileBexists == -1)
  {
    close(fileBexists);
    // These two if statements are for if fileB already exists  
    // Code to make fileB an exact copy of fileA, the examples with flag -t
    if (tflag != 0)
    {
      int fda = open(fileA, O_RDONLY, S_IRUSR|S_IWUSR);
      int fdb = open(fileB, O_WRONLY|O_TRUNC, S_IRUSR|S_IWUSR);
      while ((size = read(fda, buf, 256))>0)
      {
        write(fdb, buf, size);
      }
      close(fda);
      close(fdb);
    }

    // Code to make fileB a concatenation of fileB and fileA, the examples with
    // flag -a
    else if (aflag != 0)
    {
      int fda = open(fileA, O_RDONLY, S_IRUSR|S_IWUSR);
      int fdb = open(fileB, O_WRONLY|O_APPEND, S_IRUSR|S_IWUSR);
      while ((size = read(fda, buf, 256))>0)
      {
        write(fdb, buf, size);
      }
      close(fda);
      close(fdb);
    }
    // No t or a flag, but the destination file already exists
    else if (aflag == 0 && tflag == 0)
    {
      printf("No -a or -t option specified. Program will not modify an already existing destination file!\n");
      exit(0);
    }
  }
  // fileB doesn't exist
  else
  {
    int fda = open(fileA, O_RDONLY, S_IRUSR|S_IWUSR);
    int fdb = open(fileB, O_WRONLY|O_TRUNC|O_CREAT, S_IRUSR|S_IWUSR);
    while ((size = read(fda, buf, 256))>0)
      {
        write(fdb, buf, size);
      }
    close(fda);
    close(fdb);
  }
  return 0;
}
