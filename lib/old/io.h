#ifndef IO_H
#define IO_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdarg.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>

#include "macros/cpp_defines.h"
#include "debug.h"


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
--------------------------------------------------------------------------------------------------------------------------------------------
-                                                                Readlink                                                                  -
--------------------------------------------------------------------------------------------------------------------------------------------
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


int safe_readlink(const char *path, char *buf, size_t bufsiz);
int unsafe_fd_to_path(int fd, char * buf, long buf_n);
int unsafe_fd_to_path_preserve_errno(int fd, char * buf, long buf_n);
int safe_fd_to_path(int fd, char * buf, long buf_n);



/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
--------------------------------------------------------------------------------------------------------------------------------------------
-                                                                  Stat                                                                    -
--------------------------------------------------------------------------------------------------------------------------------------------
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


/* int stat(const char *pathname, struct stat *statbuf);
 * int fstat(int fd, struct stat *statbuf);
 * int lstat(const char *pathname, struct stat *statbuf);
 *
 * lstat() is identical to stat(), except that if pathname is a symbolic link,
 * then it returns information about the link itself, not the file that the link refers to.
 *
 * S_ISREG(m)  is it a regular file?
 * S_ISDIR(m)  directory?
 * S_ISCHR(m)  character device?
 * S_ISBLK(m)  block device?
 * S_ISFIFO(m) FIFO (named pipe)?
 * S_ISLNK(m)  symbolic link?  (Not in POSIX.1-1996.)
 * S_ISSOCK(m) socket?  (Not in POSIX.1-1996.)
 */


void safe_stat(const char * pathname, struct stat *statbuf);
void safe_lstat(const char * pathname, struct stat *statbuf);
void safe_fstat(int fd, struct stat *statbuf);


/* Tests for inode types.
 *
 *     long   stat_is{reg,dir,chr,blk,fifo,lnk,sock}         (const char * pathname);
 *     void   stat_is{reg,dir,chr,blk,fifo,lnk,sock}_assert  (const char * pathname);
 *
 *     long  lstat_is{reg,dir,chr,blk,fifo,lnk,sock}         (const char * pathname);
 *     void  lstat_is{reg,dir,chr,blk,fifo,lnk,sock}_assert  (const char * pathname);
 *
 *     long  fstat_is{reg,dir,chr,blk,fifo,lnk,sock}         (int fd);
 *     void  fstat_is{reg,dir,chr,blk,fifo,lnk,sock}_assert  (int fd);
 */

#undef decl_stat_is
#define decl_stat_is(suffix, test, err_str)                   \
long stat_is ## suffix(const char * pathname);                \
void stat_is ## suffix ## _assert(const char * pathname);     \
long lstat_is ## suffix(const char * pathname);               \
void lstat_is ## suffix ## _assert(const char * pathname);    \
long fstat_is ## suffix(int fd);                              \
void fstat_is ## suffix ## _assert(int fd);

decl_stat_is(reg , S_ISREG , "regular file")
decl_stat_is(dir , S_ISDIR , "directory")
decl_stat_is(chr , S_ISCHR , "character device")
decl_stat_is(blk , S_ISBLK , "block device")
decl_stat_is(fifo, S_ISFIFO, "FIFO (named pipe)")
decl_stat_is(lnk , S_ISLNK , "symbolic link")
decl_stat_is(sock, S_ISSOCK, "socket")

#undef decl_stat_is


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
--------------------------------------------------------------------------------------------------------------------------------------------
-                                                              Read - Write                                                                -
--------------------------------------------------------------------------------------------------------------------------------------------
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


int safe_open_base(const char * pathname, int flags, mode_t mode);

// Default mode for created files: 644 rw-r--r--
#define _safe_open(pathname, flags, mode, ...)  safe_open_base(pathname, flags, mode)
#define safe_open(pathname, flags, ...)  _safe_open(pathname, flags, ##__VA_ARGS__, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)

void safe_close(int fd);

/* size_t : unsigned integer.
 * ssize_t: used to represent the sizes of blocks that can be read or 
 *          written in a single operation.
 *          It is similar to size_t, but must be a signed type.
 */
ssize_t safe_write(int fd, char *buf, size_t count);

ssize_t safe_read(int fd, void * buf, size_t count);

/* 'read' returns 0 on EOF no matter how many times it's called,
 * but no EOF is signaled until the connection/pipe/file is closed.
 */
ssize_t read_until_EOF_base(int fd, char **ret, int strict_size_test);

ssize_t read_until_EOF_strict_size(int fd, char **ret);

ssize_t read_until_EOF(int fd, char **ret);

/* sysfs files are always read with one call to 'read()', subsequent calls always return 0.
 * Their size is always PAGE_SIZE bytes in length.
 * https://www.kernel.org/doc/Documentation/filesystems/sysfs.txt  --> (Reading/Writing Attribute Data)
 */
ssize_t read_sysfs_file(const char * filename, char ** buf_out);


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//------------------------------------------------------------------------------------------------------------------------------------------
//-                                                            Redirection                                                                 -
//------------------------------------------------------------------------------------------------------------------------------------------
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


int safe_dup(int oldfd);
void safe_dup2(int oldfd, int newfd);
void redirection_fd_to_file(int fd, char * path);

/* fdatasync() is similar to fsync(), but does not flush modified metadata
 * unless that metadata is needed in order to allow a subsequent data retrieval
 * to be correctly handled.
 */

#define redirect_fd(__fd, __file, ...)         \
do {                                           \
	int _fd = __fd;                        \
	char * _file = __file;                 \
	int _fd_dup = safe_dup(_fd);           \
	redirection_fd_to_file(_fd, _file);    \
	__VA_ARGS__                            \
	fdatasync(_fd);                        \
	safe_dup2(_fd_dup, _fd);               \
	close(_fd_dup);                        \
} while (0)


#define redirect_stdout(__file, ...)                           \
do {                                                           \
	redirect_fd(fileno(stdout), __file, ##__VA_ARGS__);    \
} while (0)


#define redirect_stderr(__file, ...)                           \
do {                                                           \
	redirect_fd(fileno(stderr), __file, ##__VA_ARGS__);    \
} while (0)


#define redirect_stdout_stderr(__file_out, __file_err, ...)    \
do {                                                           \
	int _fd_out = fileno(stdout);                          \
	int _fd_err = fileno(stderr);                          \
	char * _file_out = __file_out;                         \
	char * _file_err = __file_err;                         \
	int _fd_dup_out = safe_dup(_fd_out);                   \
	int _fd_dup_err = safe_dup(_fd_err);                   \
                                                               \
	redirection_fd_to_file(_fd_out, _file_out);            \
	redirection_fd_to_file(_fd_err, _file_err);            \
                                                               \
	__VA_ARGS__                                            \
                                                               \
	fdatasync(_fd_out);                                    \
	fdatasync(_fd_err);                                    \
                                                               \
	safe_dup2(_fd_dup_out, _fd_out);                       \
	safe_dup2(_fd_dup_err, _fd_err);                       \
                                                               \
	close(_fd_dup_out);                                    \
	close(_fd_dup_err);                                    \
} while (0)


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//------------------------------------------------------------------------------------------------------------------------------------------
//-                                                                MMap                                                                    -
//------------------------------------------------------------------------------------------------------------------------------------------
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


/* int munmap(void *addr, size_t length);
 * void * mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset);
 * void * mremap(void *old_address, size_t old_size, size_t new_size, int flags, ... void *new_address );
 */


void safe_munmap(void *addr, size_t length);
void * safe_mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset);
void * safe_mmap_annon(size_t bytes);


#ifdef _GNU_SOURCE

#define safe_mremap(old_address, old_size, new_size, flags, ... /* new_address */)      \
({                                                                                      \
	void * _ret = mremap(old_address, old_size, new_size, flags, ##__VA_ARGS__);    \
	if (_ret == MAP_FAILED)                                                         \
		error("mremap()");                                                      \
	_ret;                                                                           \
})

void * safe_mremap_fixed(void * old_address, size_t old_size, size_t new_size, void * new_address);

#endif


#endif /* IO_H */

