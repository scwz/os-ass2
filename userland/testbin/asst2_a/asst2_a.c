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

int
main(int argc, char * argv[])
{
        int fd;
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
		  int fd2 = open("tester.file", O_RDWR);
        printf("* open() got fd %d\n", fd2);
        if (fd < 0 || fd2 < 0) {
                printf("ERROR opening file: %s\n", strerror(errno));
                exit(1);
        } else {
		  	   //printf("SUCCESSFULLY didn't open: %s\n", strerror(errno));
		}
		  
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

	   printf("**********\n* open file normally \"tester.file\"\n");
	   fd = open("tester.file", O_RDWR);
	   printf("* open() got fd %d\n", fd);
	   if (fd < 0) {
	    		 printf("ERROR opening file: %s\n", strerror(errno));
	    		 exit(1);
	   } else {
	    	//printf("SUCCESSFULLY didn't open: %s\n", strerror(errno));
	   }

		printf("* testing lseek\n");
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
      if (r != 2) {
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

        return 0;
}


