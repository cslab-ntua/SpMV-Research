#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>

#include "debug.h"
#include "io.h"

#include "rapl_arm_grace.h"


void
rapl_register_init(struct RAPL_Register * reg)
{
	reg->fd = 0;
	reg->dir_name = NULL;
	reg->type = NULL;
	reg->w_cnt = 0;
	reg->w_accum = 0;
}


void
rapl_register_clean(struct RAPL_Register * reg)
{
	if (reg == NULL)
		return;
	free(reg->dir_name);
	reg->dir_name = NULL;
	free(reg->type);
	reg->type = NULL;
}


void
rapl_register_destroy(struct RAPL_Register ** reg_ptr)
{
	if (reg_ptr == NULL)
		return;
	rapl_register_clean(*reg_ptr);
	free(*reg_ptr);
	*reg_ptr = NULL;
}


static
int
rapl_open_filter(const struct dirent * file)
{
	long buf_n = 1000;
	char buf[buf_n];
	if (!strcmp(file->d_name, "hwmon1") || !strcmp(file->d_name, "hwmon2") || !strcmp(file->d_name, "hwmon3") || !strcmp(file->d_name, "hwmon4") || !strcmp(file->d_name, "hwmon5") || !strcmp(file->d_name, "hwmon7") || !strcmp(file->d_name, ".") || !strcmp(file->d_name, ".."))
		return 0;
	snprintf(buf, buf_n, "/sys/class/hwmon/%s/device/power1_average", file->d_name);
	if (access(buf, R_OK))
		return 0;
	printf("%s\n", file->d_name);
	return 1;
}

void
rapl_open(char * register_ids, struct RAPL_Register ** regs_out, long * n_out)
{
	struct RAPL_Register * regs;
	long buf_n = 1000;
	char buf[buf_n];
	char buf_name[buf_n];
	int fd;
	long i, n, len;

	len = (register_ids != NULL) ? strlen(register_ids) : 0;

	if (len == 0)
	{
		n = 0;
		struct dirent ** namelist;
		n = scandir("/sys/class/hwmon", &namelist, rapl_open_filter, alphasort);
		if (n < 0)
			error("scandir()");
		i = 0;

		regs = (typeof(regs)) malloc(n * sizeof(*regs));
		for (i=0;i<n;i++)
		{
			rapl_register_init(&regs[i]);
			regs[i].dir_name = strdup(namelist[i]->d_name);
			free(namelist[i]);
		}
		free(namelist);
	}
	else
	{
		char * str = strdup(register_ids);
		n = 1;
		for (i=0;i<len;i++)
			if (str[i] == ',')
			{
				str[i] = '\0';
				n++;
			}
		regs = (typeof(regs)) malloc(n * sizeof(*regs));
		n = 0;
		i = 0;
		while (i < len)
		{
			i += snprintf(buf_name, buf_n, "hwmon%s", str + i) - strlen("hwmon") + 1;
			snprintf(buf, buf_n, "/sys/class/hwmon/%s/device/power1_average", buf_name);
			if (access(buf, R_OK))
				continue;
			rapl_register_init(&regs[n]);
			regs[n].dir_name = strdup(buf_name);
			n++;
		}
		free(str);
	}

	for (i=0;i<n;i++)
	{
		snprintf(buf, buf_n, "/sys/class/hwmon/%s/device/power1_average", regs[i].dir_name);
		regs[i].fd = safe_open(buf, O_RDONLY);

		snprintf(buf, buf_n, "/sys/class/hwmon/%s/device/power1_oem_info", regs[i].dir_name);
		fd = safe_open(buf, O_RDONLY);
		len = safe_read(fd, buf, buf_n);
		if (len >= buf_n)
			error("rapl_open(): read overflow");
		if (len > 0 && buf[len-1] == '\n')
			buf[len-1] = '\0';
		buf[len] = '\0';
		regs[i].type = strdup(buf);

		// printf("regs[%ld].dir_name = %s\n", i, regs[i].dir_name);
		// printf("regs[%ld].type     = %s\n", i, regs[i].type);
		safe_close(fd);
	}

	*regs_out = regs;
	*n_out = n;
}


void
rapl_close(struct RAPL_Register * regs, long n)
{
	long i;
	for (i=0;i<n;i++)
	{
		safe_close(regs[i].fd);
		rapl_register_clean(&regs[i]);
	}
}


// Can only give valid results if the register has overflown at most one time.
static inline
void
read_register(struct RAPL_Register * reg)
{
	long buf_n = 1000;
	char buf[buf_n];
	long len;
	len = safe_read(reg->fd, buf, buf_n);
	if (len >= buf_n)
		error("read_register(): read overflow");
	if (lseek(reg->fd, 0, SEEK_SET) < 0)
		error("read_register(): lseek()");
	buf[len] = '\0';

	reg->w_cnt++;
	reg->w_accum += atol(buf);
	//printf("reg[%ld]->w_accum = %ld (watt_live = %.2lf W)\n", reg->w_cnt, reg->w_accum, (double)reg->w_accum / reg->w_cnt / 1e6);
}


void
rapl_read_start(struct RAPL_Register * regs, long n)
{
	long i;
	for (i=0;i<n;i++)
		read_register(&regs[i]);
}

void
rapl_read_end(struct RAPL_Register * regs, long n)
{
	long i;
	for (i=0;i<n;i++)
		read_register(&regs[i]);
}

