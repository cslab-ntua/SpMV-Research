#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <list>

typedef struct csr_elem_s {
	long long unsigned int col;
	char val[50];
	char val_im[50];
} csr_elem_t;

#define MAX_LINE_LENGTH 1000

bool compare_csr_elem_t (const csr_elem_t& first, const csr_elem_t& second)
{
	return first.col <= second.col;
}

void print_list (std::list<csr_elem_t> list)
{
	std::list<csr_elem_t>::iterator it;
	csr_elem_t elem;

	for (it=list.begin(); it!=list.end(); ++it) {
		elem = (csr_elem_t) *it;
		//std::cout  << " -> " << elem.col << "," << elem.val;
		printf(" -> %llu,%s", elem.col, elem.val);
	}
	std::cout << std::endl;
}

void print_usage(char *progname)
{
	printf("usage: %s <mmf_file> <output_file>\n", progname);
}

int main(int argc, char **argv)
{
	
	FILE *infile, *outfile;
	int ret;
	long long unsigned int nrows, ncols, nnz;
	long long unsigned int row, col;
	char val[50];
	char val_im[50];
	std::list<csr_elem_t> *elems_list;
	std::list<csr_elem_t> list;
	std::list<csr_elem_t>::iterator it;
	csr_elem_t csr_elem;

	if (argc < 3) {
		print_usage(argv[0]);
		exit(1);
	}

	infile = fopen(argv[1], "r");
	outfile = fopen(argv[2], "w");
	
	/*************************************/
	// code segment added to accept % as input too!
	int flag=0, cnt = 0, matrix_type=0;
	char buf[MAX_LINE_LENGTH];
	do{
		fgets(buf, MAX_LINE_LENGTH, infile);
		fprintf(outfile, "%s", buf);
		if(flag==0){
			// first line, need to find out if matrix is "real", "pattern" or "complex"
			flag=1;
			char delim[]=" ";
			char *ptr = strtok(buf, delim);
			int cnt = 0;
			while(ptr!=NULL){
				// printf("part %d = %s\n", cnt, ptr);
				if(cnt == 3){
					// means that we will now obtain the matrix type
					char type_real[] = "real", type_integer[] = "integer", type_pattern[] = "pattern",  type_complex[] = "complex";
					int ret_real = strcmp(ptr,type_real);
					int ret_integer = strcmp(ptr,type_integer);
					int ret_pattern = strcmp(ptr,type_pattern);
					int ret_complex = strcmp(ptr,type_complex);
					// printf("%d %d %d\n", ret_real, ret_pattern, ret_complex);
					if(ret_real == 0){
						matrix_type = 0;
					}
					if(ret_integer == 0){
						matrix_type = 0;
					}
					else if(ret_pattern == 0){
						matrix_type = 1;
					}
					else if(ret_complex == 0){
						matrix_type = 2;
					}
				}
				ptr = strtok(NULL, delim);
				cnt++;
			}
		}
	}while((buf[0]=='%'));

	// last entry in above do-while contains the buf that we want to convert to matrix info
	sscanf(buf, "%llu %llu %llu\n", &nrows, &ncols, &nnz);

	/*************************************/

	// fscanf(infile, "%llu %llu %llu\n", &nrows, &ncols, &nnz);
	elems_list = new std::list<csr_elem_t>[nrows + 1];

	/* Read from MMF file to the lists. */
	printf("Reading `%s` MMF file... ", argv[1]);
	fflush(stdout);
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// when "real" or "integer" -> simply read row, col, val as usual ////////////////////////////////
	if(matrix_type == 0){
		while ((ret = fscanf(infile, "%llu %llu %s\n", &row, &col, val)) == 3) {
			csr_elem.col = col;
			memcpy(csr_elem.val, val, 50);
			elems_list[row].push_back(csr_elem);
		}
	}
	// when "pattern" instead of real -> no values exist, read row and column only ////////////////////////////////
	if(matrix_type == 1){
		while ((ret = fscanf(infile, "%llu %llu\n", &row, &col)) == 2) {
			csr_elem.col = col;
			char def_val[50] = "1.0";
			memcpy(csr_elem.val, def_val, 50);
			elems_list[row].push_back(csr_elem);
		}
	}
	// when "complex" instead of real -> two values exist, read both of them //////////////////////////////////////
	if(matrix_type == 2){
		while ((ret = fscanf(infile, "%llu %llu %s %s\n", &row, &col, val, val_im)) == 4) {
			csr_elem.col = col;
			memcpy(csr_elem.val, val, 50);
			memcpy(csr_elem.val_im, val_im, 50);
			elems_list[row].push_back(csr_elem);
		}
	}
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	printf("[OK]\n");

	/* Sort each list. */
	printf("Sorting lists... ");
	fflush(stdout);
	for (unsigned int i=1; i < nrows + 1; i++)
		elems_list[i].sort(compare_csr_elem_t);
	printf("[OK]\n");
	
	/* Write lists to new MMF file. */
	printf("Writing sorted MMF file to `%s`... ", argv[2]);
	fflush(stdout);

	for (long long unsigned int r=1; r < nrows + 1; r++) {
		list = elems_list[r];
		for (it=list.begin(); it!=list.end(); ++it) {
			csr_elem = (csr_elem_t) *it;
			///////////////////////////////////////////////////////////////////////////////////////////////////////////////
			fprintf(outfile, "%llu %llu %s\n", r, csr_elem.col, csr_elem.val);
			
			// when "pattern" instead of real -> no values exist, write row and column only ///////////////////////////////
			// fprintf(outfile, "%llu %llu\n", r, csr_elem.col);
			
			// when "complex" instead of real -> two values exist, write both of them /////////////////////////////////////
			// fprintf(outfile, "%llu %llu %s %s\n", r, csr_elem.col, csr_elem.val, csr_elem.val_im);
			///////////////////////////////////////////////////////////////////////////////////////////////////////////////
		}
	}
	printf("[OK]\n");

	/* Cleanup */
	// delete(elems_list);
	fclose(infile);
	fclose(outfile);

	return 0;
}

