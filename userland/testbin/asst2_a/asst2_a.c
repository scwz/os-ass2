#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <err.h>
#include <errno.h>

#define MAX_BUF 500
#define OPEN_MAX 32

char teststr[] = "The quick brown fox jumped over the lazy dog.";
char buf[MAX_BUF];
char tester[] = "ABC";
char newtest[] = "XYZ";


/* Func prototypes */
int test_valid_open(const char *file, int flags);
int test_valid_read(int fd, char buf[], int n);
int test_valid_write(int fd, char string[], int testVal);
void test_valid_close(int fd);
int test_valid_dup2(int fd1, int fd2);

int
main(int argc, char * argv[])
{
        int fd, fd2, fd3;
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
        }

        
        printf("* close fd\n");
        close(fd);


        printf("**********\n* opening file with EXCL \"tester.file\"\n");
        fd = open("tester.file", O_RDWR | O_CREAT | O_EXCL );
        printf("* open() got fd %d\n", fd);
        if (fd != -1) {
                printf("ERROR opening file: %s\n", strerror(errno));
                exit(1);
        }

        printf("**********\n* opening non-existent file without perms\n");
        fd = open("blah", O_RDWR);
        printf("* open() got fd %d\n", fd);
        if (fd != -1) {
                printf("ERROR opening file: %s\n", strerror(errno));
                exit(1);
        }

        printf("**********\n* opening same file twice\n");
        fd = open("tester.file", O_RDWR);
        printf("* open() got fd %d\n", fd);
		  fd2 = open("tester.file", O_RDWR);
        printf("* open() got fd %d\n", fd2);
        if (fd < 0 || fd2 < 0) {
                printf("ERROR opening file: %s\n", strerror(errno));
                exit(1);
        }

        printf("* close fd and fd2\n");
	     close(fd);	
	     close(fd2);	
		  

		// NB: I hardcoded OPEN_MAX to 32 cos I couldn't figure out how to #include kern/limits.h here
	   printf("**********\n* test OPEN_MAX limit\n");
		int volatile i = 0; 
		int length = OPEN_MAX - 3;
		int fdtable[length]; //OPEN_MAX - std fd's = 29

		printf("* create OPEN_MAX num of fd's\n");
		while (i < length) {
			fdtable[i] = test_valid_open("openmax.file", O_RDWR | O_CREAT);
			i++;
		} 

		printf("* try and open one more\n");
		fd = open("openmax.file", O_RDWR | O_CREAT);
		if (fd != -1) {
			printf("ERROR, opened more than OPEN_MAX\n");
			exit(1);
		}

		printf("* close all fd's\n");
		i = 0;
		while (i < length) {
			test_valid_close(fdtable[i]);
			i++;
		}


      printf("**********\n* opening old file normally\n");
      fd = open("tester.file", O_RDWR);
      printf("* open() got fd %d\n", fd);
      if (fd < 0) {
              printf("ERROR opening file: %s\n", strerror(errno));
              exit(1);
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
		

	   printf("**********\n test read of wrong fd\n");
		printf("* attempting read of %d bytes\n", 2);
		r = read(fd, &buf[0], 2);
		printf("read %d bytes\n", r);
		if (r != -1) {
			printf("ERROR: %d bytes read\n", r);
			exit(1);
		}

		printf("**********\n test write to wrong fd\n");
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

/*
		// TODO : Fix implementation
		// READ - WRITE ONLY TESTS: BROKEN

		printf("**********\n* testing read on write only\n");
		fd = test_valid_open("tester.file", O_WRONLY);

		r = read(fd, &buf[0], 1);
		if (r != -1) {
			printf("ERROR invalid read on write only file: %d\n", r);
			exit(1);
		}

		test_valid_close(fd);

		printf("**********\n* testing write on read only\n");
		printf("* open test file\n");
		fd = test_valid_open("wrreadonly.file", O_RDWR | O_CREAT);

		printf("* write to test file\n");
		test_valid_write(fd, tester, strlen(tester));
		test_valid_close(fd);

		printf("* reopen test file as read only\n");
		fd = test_valid_open("wrreadonly.file", O_RDONLY);
		r = write(fd, newtest, strlen(newtest));
		if (r != -1) {
			printf("ERROR invalid write on read only file: %d\n", r);
			exit(1);
		}
		
		test_valid_close(fd);
*/

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
		
		printf("* TEST SEEK_END\n");
		r = lseek(fd, -1, SEEK_END);
      if (r != 3) {
              printf("ERROR lseek SEEK_END, r is: %d\n", r);
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
      printf("* test closing already closed fd\n");
      r = close(fd);
      if (r == 0) {
          printf("ERROR closing invalid file: %s\n", strerror(errno));
          exit(1);
      }

/*
		TODO: Investigate issue - is it a buffering issue?

		printf("**********\n* test: open 2 fd's, close one, do work and reopen another, then close both\n");
		printf("*WARNING: IO BUFFERING may falsify this test\n");

		printf("* create dummy file\n");
		fd = test_valid_open("close2open.file", O_RDWR | O_CREAT);
		//test_valid_write(fd, tester, strlen(tester));
		r = write(fd, tester, strlen(tester));
		if (r < 0) {
			printf("ERROR: didn't write\n");
			exit(1);
		}
		// N.B. Closing the file and reopening forces the write for some reason
		//test_valid_close(fd);
		//fd = test_valid_open("close2open.file", O_RDWR | O_CREAT);
		r = test_valid_read(fd, &buf[0], strlen(tester));

		if (buf[0] != 'A' || buf[1] != 'B' || buf[2] != 'C') {
			 printf("ERROR writing dummy file: %c%c%c\n", buf[0], buf[1], buf[2]);
			 exit(1);
		}
		test_valid_close(fd);

		printf("* open 2 fd's\n");
		fd = test_valid_open("close2open.file", O_RDWR);
		fd2 = test_valid_open("close2open.file", O_RDWR);

		printf("* close second\n");
		test_valid_close(fd2);

		printf("* write to first\n");
		test_valid_write(fd, newtest, strlen(newtest));

		printf("* reopen another copy\n");
		fd2 = test_valid_open("close2open.file", O_RDWR);

		printf("* close both\n");
		test_valid_close(fd);
		test_valid_close(fd2);
*/

		printf("**********\n* TEST DUP2\n");

		printf("**********\n* dup2: try and duplicate uninit fd's\n");

		fd3 = dup2(fd, fd2);
		if (fd3 != -1) {
			printf("ERROR: dup2 worked on empty oldfd, empty newfd: %d\n", fd3);
			exit(1);
		}
//TODO: see sheet
//TODO: invalid oldfd, valid newfd
//      valid oldfd, invalid newfd


//TODO: Should dup2 work if the newfd argument is invalid (ie. -1)? No must provide a valid file handle to clone onto. If open, close it.
		
      printf("*********\n* SUCCESS: TESTS COMPLETE\n");
      return 0;
}

int test_valid_open(const char *file, int flags) {
	printf("* open file normally \"%s\"\n", file);
	int fd = open(file, flags);
	printf("* open() got fd %d\n", fd);
	if (fd < 0) {
			 printf("ERROR opening file: %s\n", strerror(errno));
			 exit(1);
	}
	return fd;
}

int test_valid_read(int fd, char buf[], int n) {
	printf("* attempting read of %d bytes from fd: %d\n", n, fd);
	int r = read(fd, &buf[0], n);
	printf("read %d bytes\n", r);
	if (r != n) {
		printf("ERROR: only %d bytes read\n", r);
		exit(1);
	}
	return r;
}

int test_valid_write(int fd, char string[], int testVal) {
	printf("* write to fd: %d\n", fd);
	int r = write(fd, string, strlen(string));
	printf("wrote %d bytes\n", r);
	if (r != testVal) {
		printf("ERROR: %d bytes written\n", r);
		exit(1);
	}
	return r;
}

void test_valid_close(int fd) {
	printf("* close fd: %d\n", fd);	
	close(fd);
}

int test_valid_dup2(int fd1, int fd2) {
	int r = dup2(fd1, fd2);

	if (r != fd2) {
		printf("ERROR: dup2 failed\n");
		exit(1);
	}
	return r;
}
