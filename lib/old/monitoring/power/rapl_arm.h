#ifndef RAPL_H
#define RAPL_H

#include "macros/cpp_defines.h"


/* Altra HWMON Framework
 *
 * https://github.com/AmpereComputing/ampere-lts-kernel/issues/7
 *
 * Monitoring attributes
 *     energy<ID>_input (r)
 *         Current energy counter in micro joules.
 *     energy<ID>_label (r)
 *         Name of this power zone.
 *     temp1_input (r)
 *         Current temperature in mC.
 *     temp1_label (r)
 *         Name of this temperature zone.
*/

struct RAPL_Register {
	int fd;
	char * dir_name;
	char * type;
	long max_uj;
	long uj;
	long uj_prev;
	long uj_diff;
	long uj_accum;
};


void rapl_register_init(struct RAPL_Register * reg);
void rapl_register_clean(struct RAPL_Register * reg);
void rapl_register_destroy(struct RAPL_Register ** reg_ptr);

void rapl_open(char * register_ids, struct RAPL_Register ** regs_out, long * n_out);
void rapl_close(struct RAPL_Register * regs, long n);

void rapl_read_start(struct RAPL_Register * regs, long n);
void rapl_read_end(struct RAPL_Register * regs, long n);


#endif /* RAPL_H */

