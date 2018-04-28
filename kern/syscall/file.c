#include <types.h>
#include <kern/errno.h>
#include <kern/fcntl.h>
#include <kern/stat.h>
#include <kern/seek.h>
#include <lib.h>
#include <uio.h>
#include <current.h>
#include <synch.h>
#include <vfs.h>
#include <vnode.h>
#include <file.h>
#include <proc.h>

/*
 * Add your file-related functions here ...
 */

/*
 * open()
 *
 * Open the file named by filename and give it a file descriptor. 
 * Returns the file descriptor given.
 */

int 
open(char *filename, int flags, mode_t mode, int *retval) 
{
        int i;
        int result;
        int fd = -1;
        struct open_file *of;

        of = kmalloc(sizeof(struct open_file));
        if (of == NULL) {
                return ENFILE;
        } 

        /* find empty block in file descriptor table */
        for (i = 0; i < OPEN_MAX; i++) {
                if (curproc->fd_table[i] == NULL) {
                        fd = i;
                        break;
                }
        }

        /* empty block not found */
        if (fd == -1) {
                return EMFILE;
        }

        /* open and initialise file */
        result = vfs_open(filename, flags, mode, &of->vn);
        if (result) {
                return result;
        }

        of->offset = 0;
        of->flags = flags;
        of->ref_count = 1;
        of->of_lock = lock_create("of_lock");

        if (of->of_lock == NULL) {
                panic("open: failed to create lock");
        }

        /* add entry to file descriptor table */
        curproc->fd_table[fd] = of;
        *retval = fd;

        return 0;
}

/*
 * read()
 *
 * Read buflen bytes from the file pointed to by the file descriptor
 * into the buffer provided and updates the offset by how many bytes
 * are read. Returns amount of bytes read.
 */

int
read(int fd, void *buf, size_t buflen, ssize_t *retval) 
{
        int result;
        struct iovec iov;
        struct uio myuio;
        struct open_file *curfile; 

        if (fd < 0 || fd >= OPEN_MAX || 
                        (curfile = curproc->fd_table[fd]) == NULL) {
                return EBADF;
        }

        /* check that file is not write only */
        if ((curfile->flags & O_ACCMODE) == O_WRONLY) {
                return EBADF;
        }

        lock_acquire(curfile->of_lock);

        /* read file into buffer */
        uio_kinit(&iov, &myuio, buf, buflen, curfile->offset, UIO_READ);
        result = VOP_READ(curfile->vn, &myuio);
        if (result) {
                lock_release(curfile->of_lock);
                return result;
        }

        /* update offset */
        *retval = myuio.uio_offset - curfile->offset;
        curfile->offset = myuio.uio_offset;

        lock_release(curfile->of_lock);

        return 0;
}

/*
 * write()
 *
 * Writes nbytes from the provided buffer into the file pointed to
 * by the file descriptor and update the offset by how many bytes
 * were written. Returns amount of bytes written.
 */

int
write(int fd, const void *buf, size_t nbytes, ssize_t *retval) 
{
        int result;
        struct iovec iov;
        struct uio myuio;
        struct open_file *curfile; 

        if (fd < 0 || fd >= OPEN_MAX || 
                        (curfile = curproc->fd_table[fd]) == NULL) {
                return EBADF;
        }

        /* check that file is not read only */
        if ((curfile->flags & O_ACCMODE) == O_RDONLY) {
                return EBADF;
        }

        lock_acquire(curfile->of_lock);

        /* write buffer into file */
        uio_kinit(&iov, &myuio, (void *)buf, nbytes, curfile->offset, UIO_WRITE);
        result = VOP_WRITE(curfile->vn, &myuio);
        if (result) {
                lock_release(curfile->of_lock);
                return result;
        }

        /* update offset */
        *retval = myuio.uio_offset - curfile->offset;
        curfile->offset = myuio.uio_offset;

        lock_release(curfile->of_lock);

        return 0;
}

/*
 * lseek()
 *
 * Update seek offset depending on the value of whence. Returns the
 * new seek offset.
 */

int
lseek(int fd, off_t pos, int whence, off_t *retval) 
{
        int new_pos;
        struct stat file_stats;
        struct open_file *curfile;

        if (fd < 0 || fd >= OPEN_MAX || 
                        (curfile = curproc->fd_table[fd]) == NULL) {
                return EBADF;
        }

        /* check if you can actually change the seek offset */
        if (!VOP_ISSEEKABLE(curfile->vn)) {
                return ESPIPE;
        }

        lock_acquire(curfile->of_lock);

        /* get new pos */
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
                        lock_release(curfile->of_lock);
                        return EINVAL;
        }

        /* invalid pos */
        if (new_pos < 0) {
                lock_release(curfile->of_lock);
                return EINVAL;
        }

        /* update offset */
        curfile->offset = new_pos;
        *retval = curfile->offset;

        lock_release(curfile->of_lock);

        return 0;
}

/*
 * close()
 *
 * Close the file pointed to by the file descriptor.
 */

int 
close(int fd) 
{
        struct open_file *curfile;

        if (fd < 0 || fd >= OPEN_MAX || 
                        (curfile = curproc->fd_table[fd]) == NULL) {
                return EBADF;
        }

        lock_acquire(curfile->of_lock);

        /* remove entry from file descriptor table */
        curproc->fd_table[fd] = NULL;
        curfile->ref_count--;

        if (curfile->ref_count == 0) {
                /* free resources if no references left */
                vfs_close(curfile->vn);
                lock_release(curfile->of_lock);
                lock_destroy(curfile->of_lock);
                kfree(curfile);
        }
        else {
                lock_release(curfile->of_lock);
        }

        return 0;
}

/*
 * dup2()
 *
 * Clone a file descriptor onto another file descriptor. Returns the
 * file descriptor which was cloned onto.
 */

int 
dup2(int oldfd, int newfd) 
{
        struct open_file *curfile;

        if (oldfd < 0 || oldfd >= OPEN_MAX || 
                        newfd < 0 || newfd >= OPEN_MAX) {
                return EBADF;
        }

        if ((curfile = curproc->fd_table[oldfd]) == NULL) {
                return EBADF;
        }

        /* no effect */
        if (oldfd == newfd) {
                return 0;
        }

        /* close newfd if it contains a file */
        if (curproc->fd_table[newfd] != NULL) {
                close(newfd);
        }

        lock_acquire(curfile->of_lock);

        /* clone file descriptors */
        curfile->ref_count++;
        curproc->fd_table[newfd] = curfile;

        lock_release(curfile->of_lock);

        return 0;
}
 
/*
 * open_std_fd()
 * 
 * Open the standard file descriptors and attach them to 
 * file descriptors 0, 1, and 2.
 */

void
open_std_fd(void) 
{
        int dummy;
        char c0[] = "con:", c1[] = "con:", c2[] = "con:";

        open(c0, O_RDONLY, 0, &dummy);
        open(c1, O_WRONLY, 0, &dummy);
        open(c2, O_WRONLY, 0, &dummy);
}

/*
 * fdt_create()
 *
 * Initialise the file descriptor table and set all its entries
 * to null.
 */

struct open_file **
fdt_create(void) 
{
        struct open_file **fdt;

        fdt = kmalloc(sizeof(struct open_file *) * OPEN_MAX);
        if (fdt == NULL) {
                return NULL;
        }

        for (int i = 0; i < OPEN_MAX; i++) {
                fdt[i] = NULL;
        }

        return fdt;
}

/*
 * fdt_destroy()
 *
 * Close any currently open files in the process and then free
 * the file descriptor table.
 */

void 
fdt_destroy(struct open_file **fdt) 
{
        for (int i = 0; i < OPEN_MAX; i++) {
                close(i);
        }

        kfree(fdt);
        fdt = NULL;
}
