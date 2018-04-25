#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <err.h>
#include <errno.h>

#define MAX_BUF 500
char teststr[] = "The quick brown fox jumped over the lazy dog.";
char buf[MAX_BUF];
char tester[] = "ABC";
char newtest[] = "XYZ";

int
main(int argc, char * argv[])
{
        int fd, fd2;
		  int r;
		  //int r i, j , k;
        (void) argc;
        (void) argv;

        printf("\n**********\n* Test Open\n");

        printf("**********\n* open file normally \"tester.file\"\n");
        fd = open("tester.file", O_RDWR);
        printf("* open() got fd %d\n", fd);
        if (fd < 0) {
                printf("ERROR opening file: %s\n", strerror(errno));
                exit(1);
        } else {
		  	   //printf("SUCCESSFULLY didn't open: %s\n", strerror(errno));
        }

        
        printf("* close fd\n");
        close(fd);


        printf("**********\n* opening file with EXCL \"tester.file\"\n");
        fd = open("tester.file", O_RDWR | O_CREAT | O_EXCL );
        printf("* open() got fd %d\n", fd);
        if (fd != -1) {
                printf("ERROR opening file: %s\n", strerror(errno));
                exit(1);
        } else {
		  	   //printf("SUCCESSFULLY didn't open: %s\n", strerror(errno));
		}

        printf("**********\n* opening non-existent file without perms\n");
        fd = open("blah", O_RDWR);
        printf("* open() got fd %d\n", fd);
        if (fd != -1) {
                printf("ERROR opening file: %s\n", strerror(errno));
                exit(1);
        } else {
		  	   //printf("SUCCESSFULLY didn't open: %s\n", strerror(errno));
		}

        printf("**********\n* opening same file twice\n");
        fd = open("tester.file", O_RDWR);
        printf("* open() got fd %d\n", fd);
		  fd2 = open("tester.file", O_RDWR);
        printf("* open() got fd %d\n", fd2);
        if (fd < 0 || fd2 < 0) {
                printf("ERROR opening file: %s\n", strerror(errno));
                exit(1);
        } else {
		  	   //printf("SUCCESSFULLY didn't open: %s\n", strerror(errno));
		}

        printf("* close fd and fd2\n");
	     close(fd);	
	     close(fd2);	
		  
        printf("**********\n* opening old file normally\n");
        fd = open("tester.file", O_RDWR);
        printf("* open() got fd %d\n", fd);
        if (fd < 0) {
                printf("ERROR opening file: %s\n", strerror(errno));
                exit(1);
        } else {
		  	   //printf("SUCCESSFULLY didn't open: %s\n", strerror(errno));
		}
     
	   printf("**********\n test return val of read\n");
		printf("* attempting read of %d bytes\n", 2);
		r = read(fd, &buf[0], 2);
		printf("read %d bytes\n", r);
		if (r != 2) {
			printf("ERROR: only %d bytes read\n", r);
			exit(1);
		} else if (buf[0] != 'A' || buf[1] != 'B') {
			printf("ERROR: wrong bytes read: %c%c \n", buf[0], buf[1]);
      }

		printf("* attempting read of additional %d bytes\n", 1);
      r = read(fd, &buf[2], 1);
		printf("read %d bytes\n", r);
      if (r != 1) {
         printf("ERROR: only %d bytes read\n", r);
         exit(1);
		} else if (buf[2] != 'C') {
			printf("ERROR: wrong bytes read: %c\n", buf[2]);
      }
		
	  printf("* close fd\n");
	  close(fd);
		

	   printf("**********\n test read of invalid fd\n");
		printf("* attempting read of %d bytes\n", 2);
		r = read(fd, &buf[0], 2);
		printf("read %d bytes\n", r);
		if (r != -1) {
			printf("ERROR: %d bytes read\n", r);
			exit(1);
		}

		printf("**********\n test write to invalid fd\n");
		printf("* attempting write of %d bytes\n", 2);
		r = write(fd, teststr, strlen(teststr));
		printf("wrote %d bytes\n", r);
		if (r != -1) {
			printf("ERROR: %d bytes written\n", r);
			exit(1);
		}

		printf("**********\n test write lock\n");
		printf("*WARNING: IO BUFFERING may falsify this test\n");
	   printf("* open new tests file\"wrlock.file\"\n");
	   fd = open("wrlock.file", O_RDWR | O_CREAT);
	   printf("* open() got fd %d\n", fd);
	   if (fd < 0) {
	    		 printf("ERROR opening file: %s\n", strerror(errno));
	    		 exit(1);
	   }
      printf("* write test stuff to new file\n");
		r = write(fd, tester, strlen(tester));
		printf("wrote %d bytes\n", r);
		if (r != 3) {
			printf("ERROR: %d bytes written\n", r);
			exit(1);
      }

      printf("* close fd\n");
      close(fd);

      printf("* reopen two copies\n");
	   fd = open("wrlock.file", O_RDWR);
	   fd2 = open("wrlock.file", O_RDONLY);

      printf("* write to first copy\n");
      r = write(fd, newtest, strlen(newtest));
		printf("wrote %d bytes\n", r);
		if (r != 3) {
			printf("ERROR: %d bytes written\n", r);
			exit(1);
      }

      printf("* read from second copy\n");
      r = read(fd2, &buf[0], 3);
		printf("read %d bytes\n", r);
		if (r != 3) {
			printf("ERROR: %d bytes read\n", r);
			exit(1);
      }
      if (buf[0] != 'X' || buf[1] != 'Y' || buf[2] != 'Z') {
			printf("ERROR: %c%c%c instead of 'XYZ'\n", buf[0], buf[1], buf[2]);
			exit(1);
      }

      printf("close fd and fd2\n");
      close(fd);
      close(fd2);

		printf("**********\n* test double write\n");
		printf("*WARNING: IO BUFFERING may falsify this test\n");
	   printf("* open new test file \"doublewrite.file\"\n");
	   fd = open("doublewrite.file", O_RDWR | O_CREAT);
	   printf("* open() got fd %d\n", fd);
	   if (fd < 0) {
	    		 printf("ERROR opening file: %s\n", strerror(errno));
	    		 exit(1);
	   }

      printf("* write test stuff to new file\n");
		r = write(fd, teststr, strlen(teststr));
		printf("wrote %d bytes\n", r);
		if (r != 45) {
			printf("ERROR: %d bytes written\n", r);
			exit(1);
      }

      printf("* close fd\n");
      close(fd);

      printf("* reopen two copies\n");
	   fd = open("doublewrite.file", O_RDWR);
	   fd2 = open("doublewrite.file", O_RDWR);

      printf("* write to first copy\n");
      r = write(fd, tester, strlen(tester));
		printf("wrote %d bytes\n", r);
		if (r != 3) {
			printf("ERROR: %d bytes written\n", r);
			exit(1);
      }

      printf("* write to second copy\n");
      r = write(fd2, newtest, strlen(newtest));
		printf("wrote %d bytes\n", r);
		if (r != 3) {
			printf("ERROR: %d bytes written\n", r);
			exit(1);
      }
      if (buf[0] != 'X' || buf[1] != 'Y' || buf[2] != 'Z') {
			printf("ERROR: overwrite didn't work: %c %c %c\n", buf[0], buf[1], buf[2]);
			exit(1);
      }

      printf("close fd and fd2\n");
      close(fd);
      close(fd2);
     

		printf("**********\n* testing lseek whence types\n");
	   printf("* open file normally \"tester.file\"\n");
	   fd = open("tester.file", O_RDWR);
	   printf("* open() got fd %d\n", fd);
	   if (fd < 0) {
	    		 printf("ERROR opening file: %s\n", strerror(errno));
	    		 exit(1);
	   }

      r = lseek(fd, 1, SEEK_SET);
      if (r != 1) {
              printf("ERROR lseek SEEK_SET: %s\n", strerror(errno));
              exit(1);
      }
      r = lseek(fd, -1, SEEK_CUR);
      if (r != 0) {
              printf("ERROR lseek SEEK_CUR: %s\n", strerror(errno));
              exit(1);
      }
      r = lseek(fd, -1, SEEK_END);
      if (r != 3) {
              printf("ERROR lseek SEEK_END: %s\n", strerror(errno));
              exit(1);
		}
      r = lseek(fd, -30, SEEK_CUR);
      if (r != -1) {
              printf("ERROR lseek SEEK NEG: %s\n", strerror(errno));
              exit(1);
		}

      printf("* closing file\n");
      close(fd);

		printf("**********\n* testing lseek on multiple fd's of the same file\n");
      printf("* open file normally \"tester.file\"\n");
	   fd = open("tester.file", O_RDWR);
	   printf("* open() got fd %d\n", fd);
	   if (fd < 0) {
	    		 printf("ERROR opening file: %s\n", strerror(errno));
	    		 exit(1);
	   }

	   printf("* open second copy of file normally \"tester.file\"\n");
	   fd2 = open("tester.file", O_RDWR);
	   printf("* open() got fd %d\n", fd2);
	   if (fd2 < 0) {
	    		 printf("ERROR opening file: %s\n", strerror(errno));
	    		 exit(1);
	   }

      r = lseek(fd, 1, SEEK_SET);
      if (r != 1) {
              printf("ERROR lseek SEEK_SET: %s\n", strerror(errno));
              exit(1);
      } 
      read(fd, &buf[0], 1);
      read(fd2, &buf[1], 1);
      if (buf[0] != 'B' || buf[1] != 'A') {
          printf("ERROR lseek fd's multiple: %s\n", strerror(errno));
      }

      printf("* closing fd and fd2\n");
      close(fd);
      close(fd2);

		printf("**********\n* same test as above but open and read before 2nd open\n");
      printf("* open file normally \"tester.file\"\n");
	   fd = open("tester.file", O_RDWR);
	   printf("* open() got fd %d\n", fd);
	   if (fd < 0) {
	    		 printf("ERROR opening file: %s\n", strerror(errno));
	    		 exit(1);
	   }
      r = lseek(fd, 1, SEEK_SET);
      if (r != 1) {
              printf("ERROR lseek SEEK_SET: %s\n", strerror(errno));
              exit(1);
      } 
      read(fd, &buf[0], 1);

	   printf("* open second copy of file normally \"tester.file\"\n");
	   fd2 = open("tester.file", O_RDWR);
	   printf("* open() got fd %d\n", fd2);
	   if (fd2 < 0) {
	    		 printf("ERROR opening file: %s\n", strerror(errno));
	    		 exit(1);
	   }

      read(fd2, &buf[1], 1);
      if (buf[0] != 'B' || buf[1] != 'A') {
          printf("ERROR lseek fd's multiple: %s\n", strerror(errno));
      }

		printf("**********\n* Test CLOSE\n");
      printf("* open file normally \"tester.file\"\n");
	   fd = open("tester.file", O_RDWR);
	   printf("* open() got fd %d\n", fd);
	   if (fd < 0) {
	    		 printf("ERROR opening file: %s\n", strerror(errno));
	    		 exit(1);
	   }
      printf("* test return val\n");
      r = close(fd);
      if (r != 0) {
          printf("ERROR closing file: %s\n", strerror(errno));
          exit(1);
      }
      printf("* test invalid return val\n");
      r = close(fd);
      if (r == 0) {
          printf("ERROR closing invalid file: %s\n", strerror(errno));
          exit(1);
      }

//TODO
/*
      printf("* test: open 2 fd's, close one, do work and reopen another, thenclose both\n");
      printf("* open new file \"blah.file\"\n");
	   fd = open("blah.file", O_RDWR | O_CREAT);
	   printf("* open() got fd %d\n", fd);
	   if (fd < 0) {
	    		 printf("ERROR opening file: %s\n", strerror(errno));
	    		 exit(1);
	   }
      printf("* open duplicate file \"blah.file\"\n");
	   fd = open("blah.file", O_RDWR | O_CREAT);
	   printf("* open() got fd %d\n", fd);
	   if (fd < 0) {
	    		 printf("ERROR opening file: %s\n", strerror(errno));
	    		 exit(1);
	   }
*/

      printf("*********\n* SUCCESS: TESTS COMPLETE\n");
      return 0;
}


