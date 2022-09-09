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


/* ssize_t readlink(const char *path, char *buf, size_t bufsiz);
 * Notes:
 *     - readlink() does not append a null byte to buf!!!
 *     - It will (silently) truncate the contents (to a length of bufsiz characters),
 *       in case the buffer is too small to hold all of the contents.
 */


static inline
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


static inline
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


static inline
int
unsafe_keep_errno_fd_to_path(int fd, char * buf, long buf_n)
{
	int errno_buf;
	int ret;
	errno_buf = errno;
	ret = unsafe_fd_to_path(fd, buf, buf_n);
	errno = errno_buf;
	return ret;
}


static inline
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
 *
 * S_ISREG(m)  is it a regular file?
 * S_ISDIR(m)  directory?
 * S_ISCHR(m)  character device?
 * S_ISBLK(m)  block device?
 * S_ISFIFO(m) FIFO (named pipe)?
 * S_ISLNK(m)  symbolic link?  (Not in POSIX.1-1996.)
 * S_ISSOCK(m) socket?  (Not in POSIX.1-1996.)
 */


static inline
void
safe_stat(const char * pathname, struct stat *statbuf)
{
	int ret = stat(pathname, statbuf);
	if (ret < 0)
		error("stat(): path='%s'", (pathname != NULL ? pathname : ""));
}


static inline
void
safe_stat_isreg(const char * pathname)
{
	struct stat statbuf;
	safe_stat(pathname, &statbuf);
	if (!S_ISREG(statbuf.st_mode))
		error("not a regular file: path='%s'\n", (pathname != NULL ? pathname : ""));
}


static inline
void
safe_fstat(int fd, struct stat *statbuf)
{
	int ret = fstat(fd, statbuf);
	if (ret < 0)
		error("fstat(): fd=%d", fd);
}


static inline
void
safe_fstat_isreg(int fd)
{
	struct stat statbuf;
	safe_fstat(fd, &statbuf);
	if (!S_ISREG(statbuf.st_mode))
		error("not a regular file: fd=%d\n", fd);
}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
--------------------------------------------------------------------------------------------------------------------------------------------
-                                                              Read - Write                                                                -
--------------------------------------------------------------------------------------------------------------------------------------------
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


/* static inline
int
safe_open(const char * pathname, int flags, ...)
{
	mode_t mode = 0;
	va_list arg;
	int fd;
	if ((flags & O_CREAT) != 0
			#ifdef _GNU_SOURCE
				|| (flags & O_TMPFILE) != 0
			#endif
	   )
	{
		va_start(arg, flags);
		mode = va_arg(arg, int);
		va_end(arg);
	}
	fd = open(pathname, flags, mode);
	if (fd < 0)
		error("open(): path='%s'", (pathname != NULL ? pathname : ""));
	return fd;
} */


static inline
int
safe_open_base(const char * pathname, int flags, mode_t mode)
{
	int fd = open(pathname, flags, mode);
	if (fd < 0)
		error("open(): path='%s'", (pathname != NULL ? pathname : ""));
	return fd;
}


// Default mode for created files: 664 rw-rw-r--
#define _safe_open(pathname, flags, mode, ...)  safe_open_base(pathname, flags, mode)
#define safe_open(pathname, flags, ...)  _safe_open(pathname, flags, ##__VA_ARGS__, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH)


static inline
void
safe_close(int fd)
{
	int ret = close(fd);
	if (ret < 0)
	{
		long error_fd_path_buf_n = 10000;
		char error_fd_path_buf[error_fd_path_buf_n];
		unsafe_keep_errno_fd_to_path(fd, error_fd_path_buf, error_fd_path_buf_n);
		error("close(): fd=%d , path='%s'", fd, error_fd_path_buf);
	}
}


/* size_t : unsigned integer.
 * ssize_t: used to represent the sizes of blocks that can be read or 
 *          written in a single operation.
 *          It is similar to size_t, but must be a signed type.
 */
static inline
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
			unsafe_keep_errno_fd_to_path(fd, error_fd_path_buf, error_fd_path_buf_n);
			error("write(): fd=%d , path='%s'", fd, error_fd_path_buf);
		}
		if (num == 0)
			break;
		total += num;
	} while (total < count);

	return (ssize_t) total;
}


static inline
ssize_t
safe_read(int fd, void * buf, size_t count)
{
	ssize_t num;
	num = read(fd,  buf,  count);
	if (num < 0)
	{
		long error_fd_path_buf_n = 10000;
		char error_fd_path_buf[error_fd_path_buf_n];
		unsafe_keep_errno_fd_to_path(fd, error_fd_path_buf, error_fd_path_buf_n);
		error("read(): fd=%d , path='%s'", fd, error_fd_path_buf);
	}
	return num;
}


/* 'read' returns 0 on EOF no matter how many times it's called,
 * but no EOF is signaled until the connection/pipe/file is closed.
 */
static inline
ssize_t
read_until_EOF(int fd, char **ret)
{
	size_t total = 0;
	ssize_t num;
	size_t len = 1;
	char * buf, * tmp;

	buf = (typeof(buf)) malloc(len);
	while (1)
	{
		do {
			num = read(fd,  buf + total,  len - total);
			if (num < 0)
			{
				long error_fd_path_buf_n = 10000;
				char error_fd_path_buf[error_fd_path_buf_n];
				unsafe_keep_errno_fd_to_path(fd, error_fd_path_buf, error_fd_path_buf_n);
				error("read(): fd=%d , path='%s'", fd, error_fd_path_buf);
			}
			if (num == 0)
			{
				*ret = buf;
				return (ssize_t) total;
			}
			total += num;
		} while (total < len);
		len *= 2;
		tmp = buf;
		buf = (typeof(buf)) malloc(len);
		memcpy(buf, tmp, total);
		free(tmp);
	}
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


static inline
void
safe_munmap(void *addr, size_t length)
{
	int ret = munmap(addr, length);
	if (ret < 0)
		error("munmap()");
}


static inline
void *
safe_mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset)
{
	void * ret = mmap(addr, length, prot, flags, fd, offset);
	if (ret == MAP_FAILED)
		error("mmap()");
	return ret;
}


static inline
void *
safe_mmap_annon(size_t bytes)
{
	return safe_mmap(NULL, bytes, (PROT_READ | PROT_WRITE), (MAP_PRIVATE | MAP_ANONYMOUS), -1, 0);
}


#ifdef _GNU_SOURCE


#define safe_mremap(old_address, old_size, new_size, flags, ... /* new_address */)      \
({                                                                                      \
	void * _ret = mremap(old_address, old_size, new_size, flags, ##__VA_ARGS__);    \
	if (_ret == MAP_FAILED)                                                         \
		error("mremap()");                                                      \
	_ret;                                                                           \
})


static inline
void *
safe_mremap_fixed(void * old_address, size_t old_size, size_t new_size, void * new_address)
{
	return safe_mremap(old_address,  old_size,  new_size,  (MREMAP_MAYMOVE | MREMAP_FIXED),  new_address);
}

#endif


#endif /* IO_H */

