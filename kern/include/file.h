/*
 * Declarations for file handle and file table management.
 */

#ifndef _FILE_H_
#define _FILE_H_

/*
 * Contains some file-related maximum length constants
 */
#include <limits.h>

/*
 * Put your function declarations and data types here ...
 */

struct open_file {
        struct vnode *vn;
        struct lock *of_lock;
        off_t offset;
        int flags;
        int ref_count;
};

int open(char *filename, int flags, mode_t mode, int *retval);
int read(int fd, void *buf, size_t buflen, ssize_t *retval);
int write(int fd, const void *buf, size_t nbytes, ssize_t *retval);
int lseek(int fd, off_t pos, int whence, off_t *retval);
int close(int fd);
int dup2(int oldfd, int newfd);
void open_std_fd(void);
void oft_bootstrap(void);

#endif /* _FILE_H_ */
