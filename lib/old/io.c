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
#include "lock/spinlock.h"

#include "io.h"


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
--------------------------------------------------------------------------------------------------------------------------------------------
-                                                                Readlink                                                                  -
--------------------------------------------------------------------------------------------------------------------------------------------
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


/* ssize_t readlink(const char *path, char *buf, size_t bufsiz);
 * Notes:
 *     - readlink() does not append a null byte to buf!!!
 *     - It will (silently) truncate the contents (to a length of bufsiz characters),
 *       in case the buffer is too small to hold all of the contents.
 */


inline
int
safe_readlink(const char *path, char *buf, size_t bufsiz)
{
	int ret;
	ret = readlink(path, buf, bufsiz - 1);  // -1 for adding the null byte.
	if (ret < 0)
		error("readlink(): path=%s", (path != NULL ? path : ""));
	if (buf != NULL)
		buf[ret] = '\0';
	return ret;
}


inline
int
unsafe_fd_to_path(int fd, char * buf, long buf_n)
{
	long proc_path_n = 1000;
	char proc_path[proc_path_n];
	int ret;
	snprintf(proc_path, proc_path_n, "/proc/self/fd/%d", fd);
	ret = readlink(proc_path, buf, buf_n - 1);  // -1 for adding the null byte.
	if (ret < 0)
	{
		if (buf != NULL && buf_n > 0)
			buf[0] = '\0';
	}
	else
	{
		if (buf != NULL)
			buf[ret] = '\0';
	}
	return ret;
}


inline
int
unsafe_fd_to_path_preserve_errno(int fd, char * buf, long buf_n)
{
	int errno_buf;
	int ret;
	errno_buf = errno;
	ret = unsafe_fd_to_path(fd, buf, buf_n);
	errno = errno_buf;
	return ret;
}


inline
int
safe_fd_to_path(int fd, char * buf, long buf_n)
{
	int ret;
	ret = unsafe_fd_to_path(fd, buf, buf_n);
	if (ret < 0)
		error("readlink(): fd=%d", fd);
	return ret;
}


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


inline
void
safe_stat(const char * pathname, struct stat *statbuf)
{
	int ret = stat(pathname, statbuf);
	if (ret < 0)
		error("stat(): path='%s'", (pathname != NULL ? pathname : ""));
}


inline
void
safe_lstat(const char * pathname, struct stat *statbuf)
{
	int ret = lstat(pathname, statbuf);
	if (ret < 0)
		error("lstat(): path='%s'", (pathname != NULL ? pathname : ""));
}


inline
void
safe_fstat(int fd, struct stat *statbuf)
{
	int ret = fstat(fd, statbuf);
	if (ret < 0)
		error("fstat(): fd=%d", fd);
}


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
#define decl_stat_is(suffix, test, err_str)                                                   \
inline                                                                                        \
long                                                                                          \
stat_is ## suffix(const char * pathname)                                                      \
{                                                                                             \
	struct stat statbuf;                                                                  \
	long ret = stat(pathname, &statbuf);                                                  \
	return (ret >= 0) && test(statbuf.st_mode);                                           \
}                                                                                             \
inline                                                                                        \
void                                                                                          \
stat_is ## suffix ## _assert(const char * pathname)                                           \
{                                                                                             \
	if (! stat_is ## suffix(pathname))                                                    \
		error("not a " err_str ": path='%s'", (pathname != NULL ? pathname : ""));    \
}                                                                                             \
inline                                                                                        \
long                                                                                          \
lstat_is ## suffix(const char * pathname)                                                     \
{                                                                                             \
	struct stat statbuf;                                                                  \
	long ret = lstat(pathname, &statbuf);                                                 \
	return (ret >= 0) && test(statbuf.st_mode);                                           \
}                                                                                             \
inline                                                                                        \
void                                                                                          \
lstat_is ## suffix ## _assert(const char * pathname)                                          \
{                                                                                             \
	if (! lstat_is ## suffix(pathname))                                                   \
		error("not a " err_str ": path='%s'", (pathname != NULL ? pathname : ""));    \
}                                                                                             \
inline                                                                                        \
long                                                                                          \
fstat_is ## suffix(int fd)                                                                    \
{                                                                                             \
	struct stat statbuf;                                                                  \
	long ret = fstat(fd, &statbuf);                                                       \
	return (ret >= 0) && test(statbuf.st_mode);                                           \
}                                                                                             \
inline                                                                                        \
void                                                                                          \
fstat_is ## suffix ## _assert(int fd)                                                         \
{                                                                                             \
	if (! fstat_is ## suffix(fd))                                                         \
		error("not a " err_str ": fd='%d'", fd);                                      \
}                                                                                             \

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


inline
int
safe_open_base(const char * pathname, int flags, mode_t mode)
{
	int fd = open(pathname, flags, mode);
	if (fd < 0)
		error("open(): path='%s'", (pathname != NULL ? pathname : ""));
	return fd;
}


// Default mode for created files: 644 rw-r--r--
#define _safe_open(pathname, flags, mode, ...)  safe_open_base(pathname, flags, mode)
#define safe_open(pathname, flags, ...)  _safe_open(pathname, flags, ##__VA_ARGS__, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)


inline
void
safe_close(int fd)
{
	int ret = close(fd);
	if (ret < 0)
	{
		long error_fd_path_buf_n = 10000;
		char error_fd_path_buf[error_fd_path_buf_n];
		unsafe_fd_to_path_preserve_errno(fd, error_fd_path_buf, error_fd_path_buf_n);
		error("close(): fd=%d , path='%s'", fd, error_fd_path_buf);
	}
}


/* size_t : unsigned integer.
 * ssize_t: used to represent the sizes of blocks that can be read or 
 *          written in a single operation.
 *          It is similar to size_t, but must be a signed type.
 */
inline
ssize_t
safe_write(int fd, char *buf, size_t count)
{
	size_t total = 0;
	ssize_t num;

	do {
		num = write(fd,  buf + total,  count - total);
		if (num < 0)
		{
			long error_fd_path_buf_n = 10000;
			char error_fd_path_buf[error_fd_path_buf_n];
			unsafe_fd_to_path_preserve_errno(fd, error_fd_path_buf, error_fd_path_buf_n);
			error("write(): fd=%d , path='%s'", fd, error_fd_path_buf);
		}
		if (num == 0)
			break;
		total += num;
	} while (total < count);

	return (ssize_t) total;
}


inline
ssize_t
safe_read(int fd, void * buf, size_t count)
{
	ssize_t num;
	num = read(fd,  buf,  count);
	if (num < 0)
	{
		long error_fd_path_buf_n = 10000;
		char error_fd_path_buf[error_fd_path_buf_n];
		unsafe_fd_to_path_preserve_errno(fd, error_fd_path_buf, error_fd_path_buf_n);
		error("read(): fd=%d , path='%s'", fd, error_fd_path_buf);
	}
	return num;
}


/* 'read' returns 0 on EOF no matter how many times it's called,
 * but no EOF is signaled until the connection/pipe/file is closed.
 */
inline
ssize_t
read_until_EOF_base(int fd, char **ret, int strict_size_test)
{
	struct stat sb;
	size_t total = 0;
	ssize_t num;
	size_t file_size;
	char * buf;

	safe_fstat(fd, &sb);
	if (!S_ISREG(sb.st_mode))
		error("not a regular file: fd=%d", fd);
	file_size = sb.st_size + 1;
	buf = (typeof(buf)) malloc(file_size + 1);     // Add a NULL byte at the end for extra safety and so that it can be used as a string.
	while (1)
	{
		do {
			num = read(fd,  buf + total,  file_size - total);
			if (num < 0)
			{
				long error_fd_path_buf_n = 10000;
				char error_fd_path_buf[error_fd_path_buf_n];
				unsafe_fd_to_path_preserve_errno(fd, error_fd_path_buf, error_fd_path_buf_n);
				error("read(): fd=%d , path='%s'", fd, error_fd_path_buf);
			}
			if (num == 0)
			{
				// Some files report false size, e.g. sysfs files are always of size 'PAGE_SIZE', but 'read()' returns the actual size and always with one call.
				if (strict_size_test && (total != file_size))
					error("read(): EOF before reading the whole file (based on size returned by 'fstat') file size = %d, bytes read = %d", file_size, total);
				buf[total] = '\0';
				*ret = buf;
				return (ssize_t) total;
			}
			total += num;
		} while (total < file_size);
		error("read(): no EOF after reading the whole file (based on size returned by 'fstat')");
	}
}


inline
ssize_t
read_until_EOF_strict_size(int fd, char **ret)
{
	return read_until_EOF_base(fd, ret, 1);
}


inline
ssize_t
read_until_EOF(int fd, char **ret)
{
	return read_until_EOF_base(fd, ret, 0);
}


/* sysfs files are always read with one call to 'read()', subsequent calls always return 0.
 * Their size is always PAGE_SIZE bytes in length.
 * https://www.kernel.org/doc/Documentation/filesystems/sysfs.txt  --> (Reading/Writing Attribute Data)
 */
inline
ssize_t
read_sysfs_file(const char * filename, char ** buf_out)
{
	long buf_n = sysconf(_SC_PAGESIZE) + 1;
	char buf[buf_n];
	int fd;
	ssize_t len;
	fd = safe_open(filename, O_RDONLY);
	len = safe_read(fd, buf, buf_n - 1);
	buf[(int) len] = 0;  // Stupid c++ warnings when 'len' is long.
	safe_close(fd);
	*buf_out = strdup(buf);
	return len;
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//------------------------------------------------------------------------------------------------------------------------------------------
//-                                                            Redirection                                                                 -
//------------------------------------------------------------------------------------------------------------------------------------------
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


/* The returned file descriptor is made to reference the same open file description as 'oldfd'.
 * After a successful return, the old and new file descriptors may be used interchangeably.
 *
 * The new file descriptor number is guaranteed to be the lowest-numbered file descriptor that was unused in the calling process.
 */
inline
int
safe_dup(int oldfd)
{
	int newfd = dup(oldfd);
	if (newfd < 0) 
		error("dup()");
	return newfd;
}


/* 'newfd' is made to reference the same open file description as 'oldfd'.
 * After a successful return, the old and new file descriptors may be used interchangeably.
 */
inline
void
safe_dup2(int oldfd, int newfd)
{
	if (dup2(oldfd, newfd) < 0) 
		error("dup2()");
}


static int32_t redirection_lock = LOCK_UNLOCKED;
static long redirection_files_size = 0;
static long redirection_files_n = 0;
static char ** redirection_files = NULL;
static int * redirection_fds = NULL;


static
int
redirect_fd_to_file_open(char * path)
{
	int fd_redir;
	long i;
	if (redirection_files_n + 1 >= redirection_files_size)
	{
		redirection_files_size = 2 * redirection_files_size + 1;
		char ** tmp_paths = (typeof(tmp_paths)) malloc(redirection_files_size * sizeof(*tmp_paths));
		int * tmp_fds = (typeof(tmp_fds)) malloc(redirection_files_size * sizeof(*tmp_fds));
		for (i=0;i<redirection_files_n;i++)
		{
			tmp_paths[i] = redirection_files[i];
			tmp_fds[i] = redirection_fds[i];
		}
		free(redirection_files);
		free(redirection_fds);
		redirection_files = tmp_paths;
		redirection_fds = tmp_fds;
	}
	fd_redir = safe_open(path, O_RDWR|O_CREAT|O_TRUNC);
	redirection_files[redirection_files_n] = strdup(path);
	redirection_fds[redirection_files_n] = fd_redir;
	__atomic_store_n(&redirection_files_n, redirection_files_n + 1, __ATOMIC_RELEASE);
	return fd_redir;
}


static
int
redir_search(char * path)
{
	long i;
	for (i=0;i<redirection_files_n;i++)
	{
		if (!strcmp(redirection_files[i], path))
		{
			return redirection_fds[i];
		}
	}
	return -1;
}


void
redirection_fd_to_file(int fd, char * path)
{
	int fd_redir;

	fd_redir = redir_search(path);
	if (fd_redir < 0)
	{
		ttaslock_lock(&redirection_lock);
		fd_redir = redir_search(path);
		if (fd_redir < 0)
			fd_redir = redirect_fd_to_file_open(path);
		ttaslock_unlock(&redirection_lock);
	}

	safe_dup2(fd_redir, fd);
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//------------------------------------------------------------------------------------------------------------------------------------------
//-                                                                MMap                                                                    -
//------------------------------------------------------------------------------------------------------------------------------------------
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


/* int munmap(void *addr, size_t length);
 * void * mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset);
 * void * mremap(void *old_address, size_t old_size, size_t new_size, int flags, ... void *new_address );
 */


inline
void
safe_munmap(void *addr, size_t length)
{
	int ret = munmap(addr, length);
	if (ret < 0)
		error("munmap()");
}


inline
void *
safe_mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset)
{
	void * ret = mmap(addr, length, prot, flags, fd, offset);
	if (ret == MAP_FAILED)
		error("mmap()");
	return ret;
}


inline
void *
safe_mmap_annon(size_t bytes)
{
	return safe_mmap(NULL, bytes, (PROT_READ | PROT_WRITE), (MAP_PRIVATE | MAP_ANONYMOUS), -1, 0);
}


#ifdef _GNU_SOURCE

inline
void *
safe_mremap_fixed(void * old_address, size_t old_size, size_t new_size, void * new_address)
{
	return safe_mremap(old_address,  old_size,  new_size,  (MREMAP_MAYMOVE | MREMAP_FIXED),  new_address);
}

#endif

