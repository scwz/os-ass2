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
sys_open(userptr_t user_filename, int flags, int *retval) 
{
        int result;
        char *filename;
        size_t got;

        filename = kmalloc(sizeof(char *) * PATH_MAX);
        if (filename == NULL) {
                return ENOSPC;
        }

        result = copyinstr(user_filename, filename, PATH_MAX, &got);
        if (result) {
                return result;
        }

        result = open(filename, flags, retval);
        if (result) {
                return result;
        }

        kfree(filename);

        return 0;
}

int
sys_read(int fd, userptr_t user_buf, size_t buflen, ssize_t *retval) 
{
        (void) fd;
        (void) user_buf;
        (void) buflen;
        (void) retval;
        return 0;
}

int
sys_write(int fd, const_userptr_t user_buf, size_t nbytes, ssize_t *retval) 
{
        int result;
        void *buf;

        buf = kmalloc(sizeof(void *) * nbytes);
        if (buf == NULL) {
                return ENOSPC;
        }

        result = copyin(user_buf, buf, nbytes);
        if (result) {
                return result;
        }

        result = write(fd, buf, nbytes, retval);
        if (result) {
                return result;
        }

        kfree(buf);

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
        return close(fd);
}

int 
sys_dup2(int oldfd, int newfd) 
{
        (void) oldfd;
        (void) newfd;
        return 0;
}

