#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>

#include "macros/cpp_defines.h"
#include "time_it.h"
#include "monitoring/power/rapl.h"


// Time resolution at this scale seems to have a +-1% error on it's own.
int
main()
{
	struct RAPL_Register * regs;
	long regs_n;
	long i, j;
	long reps = 1000;
	char * reg_ids;
	struct timespec ts;
	double time_base, time_test;

	reg_ids = NULL;
	// reg_ids = (char *) "0";
	// reg_ids = (char *) "0:0";
	// reg_ids = (char *) "0,0:0";

	ts.tv_sec = 0;
	ts.tv_nsec = 1000000000 / 1000; // 0.001 sec

	rapl_open(reg_ids, &regs, &regs_n);

	time_base = 0;
	for (j=0;j<reps;j++)
	{
		rapl_read_start(regs, regs_n);
		time_base += time_it(1,
			nanosleep(&ts, NULL);
		);
		rapl_read_end(regs, regs_n);
	}

	time_test = 0;
	for (j=0;j<reps;j++)
	{
		rapl_read_start(regs, regs_n);
		time_test += time_it(1,
			nanosleep(&ts, NULL);
		);
		rapl_read_end(regs, regs_n);
	}

	printf("time_base = %lf , time_test = %lf , diff%% = %lf%%\n", time_base, time_test, (time_test - time_base) / time_base * 100);

	printf("\n");
	for (i=0;i<regs_n;i++)
	{
		printf("%s, name=%s, s:%ld e:%ld diff:%ld accum:%ld max_val:%ld\n",
				regs[i].dir_name, regs[i].type, regs[i].uj_prev, regs[i].uj,
				regs[i].uj_diff, regs[i].uj_accum, regs[i].max_uj);
	}
	printf("\n");
	for (i=0;i<regs_n;i++)
		printf("'%s' total joule = %g\n", regs[i].type, ((double) regs[i].uj_accum) / 1000000);

	rapl_close(regs, regs_n);
	free(regs);

	return 0;
}


