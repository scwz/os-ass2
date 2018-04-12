#include <types.h>
#include <kern/errno.h>
#include <kern/fcntl.h>
#include <kern/limits.h>
#include <kern/stat.h>
#include <kern/seek.h>
#include <lib.h>
#include <uio.h>
#include <thread.h>
#include <current.h>
#include <synch.h>
#include <vfs.h>
#include <vnode.h>
#include <file.h>
#include <proc.h>
#include <syscall.h>
#include <copyinout.h>

/*
 * Add your file-related functions here ...
 */

int 
open(char *filename, int flags, int *retval) 
{
        (void) filename;
        (void) flags;
        (void) retval;
        return 0;
}

int
read(int fd, void *buf, size_t buflen, ssize_t *retval) 
{
        (void) fd;
        (void) buf;
        (void) buflen;
        (void) retval;
        return 0;
}

int
write(int fd, const void *buf, size_t nbytes, ssize_t *retval) 
{
        (void) fd;
        (void) buf;
        (void) nbytes;
        (void) retval;
        return 0;
}

int
lseek(int fd, off_t pos, int whence, off_t *retval) 
{
        (void) fd;
        (void) pos;
        (void) whence;
        (void) retval;
        return 0;
}

int 
close(int fd) 
{
        (void) fd;
        return 0;
}

int 
dup2(int oldfd, int newfd) 
{
        (void) oldfd;
        (void) newfd;
        return 0;
}
 
int
open_std_fd(void) {
//        char c1[] = "con:", c2[] = "con:";
        return 0;
}
