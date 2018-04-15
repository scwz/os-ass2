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
        int i;
        int result;
        int fd = -1, entry_no = -1;
        struct open_file *of;

        of = kmalloc(sizeof(struct open_file *));
        if (of == NULL) {
                return ENFILE;
        } 

        /* find empty block in open file table */
        for (i = 0; i < OPEN_MAX; i++) {
                if (oft.table[i] == NULL) {
                        entry_no = i;
                        break;
                }
        }

        if (entry_no == -1) {
                return ENFILE;
        }

        /* find empty block in file descriptor table */
        for (i = 0; i < OPEN_MAX; i++) {
                if (curproc->fd_table[i] == NULL) {
                        fd = i;
                        break;
                }
        }

        if (fd == -1) {
                return EMFILE;
        }

        of->index = entry_no;
        of->offset = 0;
        of->flags = flags;
        of->ref_count = 1;
        of->of_lock = lock_create("of_lock");

        result = vfs_open(filename, flags, 0, &of->vn);
        if (result) {
                return result;
        }

        oft.table[entry_no] = curproc->fd_table[fd] = of;
        *retval = fd;

        return 0;
}

int
read(int fd, void *buf, size_t buflen, ssize_t *retval) 
{
        int result;
        struct iovec iov;
        struct uio myuio;
        struct open_file *curfile; 

        if (fd < 0 || fd >= OPEN_MAX || (curfile = curproc->fd_table[fd]) == NULL) {
                return EBADF;
        }

        lock_acquire(curfile->of_lock);

        uio_kinit(&iov, &myuio, buf, buflen, curfile->offset, UIO_READ);
        result = VOP_READ(curfile->vn, &myuio);
        if (result) {
                return result;
        }

        *retval = myuio.uio_offset - curfile->offset;
        curfile->offset = myuio.uio_offset;

        lock_release(curfile->of_lock);
        return 0;
}

int
write(int fd, const void *buf, size_t nbytes, ssize_t *retval) 
{
        int result;
        struct iovec iov;
        struct uio myuio;
        struct open_file *curfile; 

        if (fd < 0 || fd >= OPEN_MAX || (curfile = curproc->fd_table[fd]) == NULL) {
                return EBADF;
        }

        lock_acquire(curfile->of_lock);

        uio_kinit(&iov, &myuio, (void *)buf, nbytes, curfile->offset, UIO_WRITE);
        result = VOP_WRITE(curfile->vn, &myuio);
        if (result) {
                return result;
        }

        *retval = myuio.uio_offset - curfile->offset;
        curfile->offset = myuio.uio_offset;

        lock_release(curfile->of_lock);

        return 0;
}

int
lseek(int fd, off_t pos, int whence, off_t *retval) 
{
        int new_pos;
        struct stat file_stats;
        struct open_file *curfile;

        if (fd < 0 || fd >= OPEN_MAX || (curfile = curproc->fd_table[fd]) == NULL) {
                return EBADF;
        }

        lock_acquire(curfile->of_lock);

        switch (whence) {
                case SEEK_SET:
                        new_pos = pos;
                        break;
                case SEEK_CUR:
                        new_pos = curfile->offset + pos;
                        break;
                case SEEK_END:
                        VOP_STAT(curfile->vn, &file_stats);
                        new_pos = file_stats.st_size + pos;
                        break;
                default:
                        return EINVAL;
        }

        if (new_pos < 0) {
                return EINVAL;
        }

        curfile->offset = new_pos;
        *retval = curfile->offset;

        lock_release(curfile->of_lock);

        return 0;
}

int 
close(int fd) 
{
        int index;
        struct open_file *curfile;

        if (fd < 0 || fd >= OPEN_MAX || (curfile = curproc->fd_table[fd]) == NULL) {
                return EBADF;
        }

        curproc->fd_table[fd] = NULL;
        curfile->ref_count--;

        if (curfile->ref_count == 0) {
                index = curfile->index;

                vfs_close(curfile->vn);
                lock_destroy(curfile->of_lock);
                kfree(curfile);

                oft.table[index] = NULL;
        }

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
        int dummy;
        char c0[] = "con:", c1[] = "con:", c2[] = "con:";

        // open stdin, stdout, and stderr first so that they
        // get fds 0, 1, 2 respectively
        open(c0, O_RDONLY, &dummy);
        open(c1, O_WRONLY, &dummy);
        open(c2, O_WRONLY, &dummy);
        return 0;
}
