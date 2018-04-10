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
#include <syscall.h>
#include <copyinout.h>

/*
 * Add your file-related functions here ...
 */

int 
sys_open(userptr_t user_filename, int flags, int *retval) {
        (void) user_filename;
        (void) flags;
        (void) retval;
        return 0;
}

int
sys_read(int fd, userptr_t user_buf, size_t buflen, ssize_t *retval) {
        (void) fd;
        (void) user_buf;
        (void) buflen;
        (void) retval;
        return 0;
}

int
sys_write(int fd, const_userptr_t user_buf, size_t nbytes, ssize_t *retval) 
{
        (void) fd;
        (void) user_buf;
        (void) nbytes;
        (void) retval;
        return 0;
}

int
sys_lseek(int fd, off_t pos, int whence, off_t *retval) 
{
        (void) fd;
        (void) pos;
        (void) whence;
        (void) retval;
        return 0;
}

int 
sys_close(int fd) 
{
        (void) fd;
        return 0;
}

int 
sys_dup2(int oldfd, int newfd) 
{
        (void) oldfd;
        (void) newfd;
        return 0;
}
