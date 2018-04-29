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
        (void) argc;
        (void) argv;

		  printf("* Create tester.file if it doesn't exist\n");
		  fd = open("tester.file", O_RDWR | O_CREAT | O_EXCL);
		  if (fd > 0) {
		     test_valid_write(fd, tester, strlen(tester));
		  }
		  test_valid_close(fd);

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

		test_valid_close(fd);
		test_valid_close(fd2);


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


		printf("**********\n* test: open 2 fd's, close one, do work and reopen another, then close both\n");
		printf("*WARNING: IO BUFFERING may falsify this test\n");

		printf("* create dummy file\n");
		fd = test_valid_open("close2open.file", O_RDWR | O_CREAT);
		test_valid_write(fd, tester, strlen(tester));
		r = lseek(fd, SEEK_SET, 0);

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

		printf("**********\n* Test: Functions on fd's within valid range but unopened\n");
		fd = 10;
		printf("* test read\n");
		r = read(fd, &buf[0], 1);
		if (r != -1) {
			printf("Error reading invalid fd within range\n");
			exit(1);
		}
		printf("* test write\n");
		r = write(fd, newtest, strlen(newtest));
		if (r != -1) {
			printf("Error writing invalid fd within range\n");
			exit(1);
		}
		printf("* test lseek\n");
		r = lseek(fd, SEEK_SET, 0);
		if (r != -1) {
			printf("Error lseeking invalid fd within range\n");
			exit(1);
		}
		printf("* test close\n");
		r = close(fd);
		if (r != -1) {
			printf("Error closing invalid fd within range\n");
			exit(1);
		}
		fd2 = test_valid_open("valid.file", O_RDWR | O_CREAT);
		printf("* test dup2\n");
		r = dup2(fd, fd2);
		if (r != -1) {
			printf("Error dup2 invalid fd within range\n");
			exit(1);
		}

		test_valid_close(fd2);
		fd = -1;
		fd2 = -1;
		

		printf("**********\n* Test: Open two copies and close one - see if second is ok\n");
		fd = test_valid_open("tester.file", O_RDWR);
		fd2 = test_valid_open("tester.file", O_RDWR);

		test_valid_close(fd);

		r = test_valid_read(fd2, &buf[0], 1);
		if (buf[0] != 'A') {
			printf("Error on closing 2nd copy of fd: %c\n", buf[0]);
			exit(1);
		}
		test_valid_close(fd2);


		printf("**********\n* TEST DUP2\n");
		fd = -1;
		fd2 = -1;

		printf("**********\n* dup2: empty old fd, empty new fd\n");

		fd3 = dup2(fd, fd2);
		if (fd3 != -1) {
			printf("ERROR: dup2 worked on empty oldfd, empty newfd: %d\n", fd3);
			exit(1);
		}

		printf("**********\n* dup2: empty oldfd, init newfd\n");
		fd2 = test_valid_open("dup2.file", O_RDWR | O_CREAT);
		fd3 = dup2(fd, fd2);
		if (fd3 != -1) {
			printf("ERROR: dup2 worked on empty oldfd, init newfd: %d, %d\n", fd, fd2);
			exit(1);

		}
		test_valid_close(fd2);
		fd2 = -1;

		printf("**********\n* dup2: init oldfd, empty newfd\n");
		fd = test_valid_open("dup2.file", O_RDWR | O_CREAT);
		fd3 = dup2(fd, fd2);
		if (fd3 != -1) {
			printf("ERROR: dup2 worked on init oldfd, empty newfd: %d\n", fd3);
			exit(1);
		}
		test_valid_close(fd);
		fd = -1;
		
		printf("**********\n* dup2: close newfd if opened\n");
		fd = test_valid_open("dup2.file", O_RDWR | O_CREAT);
		fd2 = test_valid_open("dup2_2.file", O_RDWR | O_CREAT);

		int old_newfd = fd2;
		
		fd3 = dup2(fd, fd2);
		if (fd3 < 0) {
			printf("ERROR: dup2 failed: %d\n", fd3);
			exit(1);
		} else if (fd3 != old_newfd) {
			printf("ERROR: dup2 newfd is a different fd: %c\n", fd3);
			exit(1);
		}
		test_valid_close(fd);
		test_valid_close(fd2);


		printf("**********\n* dup2: old and newfd are same\n");
		fd = test_valid_open("dup2.file", O_RDWR | O_CREAT);

		fd3 = dup2(fd, fd);
		if (fd3 == -1) {
			printf("ERROR: dup2 failed on same: %d\n", fd3);
			exit(1);
		}
		test_valid_close(fd);


		printf("**********\n* dup2: check newfd attributes are same as oldfd\n");
		fd = test_valid_open("tester.file", O_RDONLY);
		fd2 = 4;

		printf("* advance fd's seek pointer by 1\n");
		test_valid_read(fd, &buf[0], 1);	

		r = write(fd, newtest, strlen(newtest));
		if (r != -1) {
			printf("ERROR: writing on rdonly oldfd allowed\n");
			exit(1);
		}
		
		fd3 = test_valid_dup2(fd, fd2);

		printf("* test to see if a new open skips newfd's assigned fd number\n");
		int testfd = test_valid_open("tester.file", O_RDONLY);
		if (testfd != 5) {
			printf("ERROR: dup2 doesn't reserve int newfd from the open function\n");
			exit(1);
		}
		test_valid_close(testfd);
		
		printf("* test to see if fd and cloned fd have same seek pointer AND reference same object\n");
		test_valid_read(fd3, &buf[0], 1);
		test_valid_read(fd, &buf[1], 1);

		if (buf[0] != 'B') {
			printf("ERROR: dup2 cloned fd doesn't have same seek pointer\n");
			exit(1);
		}
		if (buf[1] != 'C') {
			printf("ERROR: dup2 cloned fd doesn't reference same object\n");
			exit(1);
		}

		r = write(fd3, newtest, strlen(newtest));
		if (r != -1) {
			printf("ERROR: dup2 cloned fd permissions not the same\n");
			exit(1);
		}

		printf("* test to see if closing cloned fd affects oldfd\n");
		test_valid_close(fd3);

		r = lseek(fd, 0, SEEK_SET);
		if (r != 0) {
			printf("ERROR: dup2 closing cloned fd closes oldfd?\n");
			exit(1);
		}

		test_valid_close(fd);


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
