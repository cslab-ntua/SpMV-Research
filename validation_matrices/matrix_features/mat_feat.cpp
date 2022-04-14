#include "stdio.h"
#include "stdlib.h"
#include "util.h"

#include "matrix_util.h"


CSRArrays csr;
COOArrays coo;


int main(int argc, char **argv)
{
	__attribute__((unused)) int num_threads;
	__attribute__((unused)) double time;
	long i;

	if (argc < 6)
	{
		char * file_in;
		i = 1;
		file_in = argv[i++];
		create_coo_matrix(file_in, &coo);
		COO_to_CSR(&coo, &csr);

		char * buf;
		long buf_n;
		char * title;
		buf = strdup(file_in);
		buf_n = strlen(buf) + 1;
		snprintf(buf, buf_n, "%s", file_in);
		title = buf;
		for (i=0;i<buf_n;i++)
		{
			if (buf[i] == '.')
			{
				buf[i] = 0;
			}
			if (buf[i] == '/')
				title = &buf[i+1];
		}

		csr_matrix_features(title, csr.ia, csr.ja, csr.m, csr.n, csr.nnz);
		free(buf);
	}

	return 0;
}

