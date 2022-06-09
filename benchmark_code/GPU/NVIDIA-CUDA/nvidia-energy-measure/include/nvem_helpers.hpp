#ifndef NVEM_HELP_H
#define NVEM_HELP_H

double csecond(void);

void warning(const char *fmt, ...);
void error(const char *fmt, ...);
void massert(bool condi, const char *fmt, ...);
#endif
