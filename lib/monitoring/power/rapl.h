#ifndef RAPL_H
#define RAPL_H

#include "macros/cpp_defines.h"


/* Power Capping Framework - RAPL
 *
 * https://docs.kernel.org/power/powercap/powercap.html
 *
 * Monitoring attributes
 *     energy_uj (rw)
 *         Current energy counter in micro joules. Write “0” to reset. If the counter can not be reset, then this attribute is read only.
 *     max_energy_range_uj (ro)
 *         Range of the above energy counter in micro-joules.
 *     power_uw (ro)
 *         Current power in micro watts.
 *     max_power_range_uw (ro)
 *         Range of the above power value in micro-watts.
 *     name (ro)
 *         Name of this power zone.
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

