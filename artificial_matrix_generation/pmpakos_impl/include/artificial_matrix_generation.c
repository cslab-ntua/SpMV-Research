#include "artificial_matrix_generation.h"

const ValueType sample_values[14] = {4.1792, 2.1257, 7.2853, 4.3276, 4.6612, 1.184, 2.9755, 7.0970, 8.7629, 4.4306, 9.5388, 3.70473, 7.6795, 7.3303};

double getTimestamp(){
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return tv.tv_usec + tv.tv_sec*1e6;
}

/*
	Allocate memory for csr matrix
*/
csr_matrix *create_csr_matrix(unsigned int nr_rows, unsigned int nr_cols, unsigned int nr_nzeros,
							  double density, char mem_range[128],
							  double avg_nnz_per_row, double std_nnz_per_row,
							  int seed, char *distribution, char *placement, double diagonal_factor,
							  double avg_bw, double std_bw, double avg_sc, double std_sc,
							  double time1, double time2)
{	
	csr_matrix *matrix;
    matrix = (csr_matrix*)malloc(sizeof(csr_matrix));

    matrix->nr_rows   = nr_rows;
    matrix->nr_cols   = nr_cols;
    matrix->nr_nzeros = nr_nzeros;
    
    matrix->density       = density;
    matrix->mem_footprint = ((sizeof(ValueType)+sizeof(IndexType))*nr_nzeros + sizeof(IndexType)*(nr_rows+1))/(1.0*1024*1024);
    matrix->precision     = VALUE_TYPE_BIT_WIDTH;
    strcpy(matrix->mem_range,mem_range);
    
    matrix->avg_nnz_per_row = avg_nnz_per_row;
    matrix->std_nnz_per_row = std_nnz_per_row;

    matrix->seed = seed;
    strcpy(matrix->distribution,distribution);
    strcpy(matrix->placement,placement);
    matrix->diagonal_factor = diagonal_factor;

    matrix->avg_bw = avg_bw;
    matrix->std_bw = std_bw;
    matrix->avg_sc = avg_sc;
    matrix->std_sc = std_sc;

    matrix->time1 = time1;
    matrix->time2 = time2;

    matrix->row_ptr   = (IndexType*)malloc((nr_rows + 1)*sizeof(IndexType));
    matrix->col_ind   = (IndexType*)malloc(nr_nzeros*sizeof(IndexType));
    matrix->values    = (ValueType*)malloc(nr_nzeros*sizeof(ValueType));
    return matrix;
}

/*
	Free memory allocated for csr matrix
*/
void delete_csr_matrix(csr_matrix *matrix)
{
	if (!matrix)
        return;

    free(matrix->values);
    free(matrix->col_ind);
    free(matrix->row_ptr);
    free(matrix);
    return;
}

// https://github.com/edthrn/python3-in-c
csr_matrix *artificial_matrix_generation(int argc, char *argv[], int starting_point, int verbose)
{
	if(verbose)
		printf(">>> Artificial Matrix Generation starts now...\n");
	PyObject *pName, *pModule, *pFunc;
	PyObject *pArgs, *pValue;
	int i;

	csr_matrix *matrix;
	matrix = NULL;
	unsigned int py_nnz = 0;
	double py_density=0, py_avg_nnz_per_row=0, py_std_nnz_per_row=0, py_avg_bw=0, py_std_bw=0, py_avg_sc=0, py_std_sc = 0, py_time1=0, py_time2=0;
	char py_mem_range[128];

	if (argc < 8) {
		fprintf(stderr,"Usage: call pythonfile funcname [args]\n");
		return NULL;
	}
	
	Py_Initialize();
	pName = PyUnicode_DecodeFSDefault("artificial_matrix_generation"); 
	/* Error checking of pName left out */
	// https://stackoverflow.com/a/35582046 (!!!)
	PyRun_SimpleString("import sys");
	PyRun_SimpleString("sys.path.append(\"/various/pmpakos/SpMV-Research/artificial_matrix_generation/pmpakos_impl/include/\")");

	// no need to error-check these two, they are core of python
	/*PyImport_ImportModule("random");
	PyImport_ImportModule("time");

	if ( (PyImport_ImportModule("numpy")) == NULL){
		PyErr_Print();
		fprintf(stderr, "Failed to load numpy. pip3 install it and then come back again. Thank you\n");
		return NULL;
	}
	if ( (PyImport_ImportModule("multiprocessing")) == NULL){
		PyErr_Print();
		fprintf(stderr, "Failed to load multiprocessing. pip3 install it and then come back again. Thank you\n");
		return NULL;
	}
	if ( (PyImport_ImportModule("scipy")) == NULL){
		PyErr_Print();
		fprintf(stderr, "Failed to load scipy. pip3 install it and then come back again. Thank you\n");
		return NULL;
	}
	if ( (PyImport_ImportModule("h5py")) == NULL){
		PyErr_Print();
		fprintf(stderr, "Failed to load h5py. pip3 install it and then come back again. Thank you\n");
		return NULL;
	}*/
	
	pModule = PyImport_Import(pName);
	Py_DECREF(pName);

	if(verbose)
		fprintf(stderr,">>> Imported 'artificial_matrix_generation.py' file...\n");

	if (pModule != NULL) {
		pFunc = PyObject_GetAttrString(pModule, "sparse_matrix_generator_wrapper");

		/* pFunc is a new reference */
		if (pFunc && PyCallable_Check(pFunc)) {
			int matrix_generation_args = 7;
			// I need 7 arguments (+2 for precision and verbosity), do not use argc - 1, not safe (used to be PyTuple_New(argc-1)
			pArgs = PyTuple_New(matrix_generation_args+2); 
			int cnt = starting_point;
			for (i = 0; i < matrix_generation_args+2; ++i) {
				if(i==0 || i==6) // 0 : nr_rows, 6 : seed
					pValue = PyLong_FromLong(atoi(argv[cnt++]));
				else if(i==7) // 7 : precision
					pValue = PyLong_FromLong((long)VALUE_TYPE_BIT_WIDTH);
				else if(i==8) // 8 : verbosity
					pValue = PyLong_FromLong(verbose);
				else if(i==1 || i==2 || i==5) // 1 : avg_nnz_per_row, 2 : std_nnz_per_row, 5 : d_f
					pValue = PyFloat_FromDouble(strtod(argv[cnt++],NULL)); //  atof(argv[cnt++])
				else // 3 : distribution, 4 : placement
					pValue = Py_BuildValue("s",argv[cnt++]);

				if (!pValue) {
					Py_DECREF(pArgs);
					Py_DECREF(pModule);
					fprintf(stderr, "Cannot convert argument\n");
					return NULL;
				}
				/* pValue reference stolen here: */
				PyTuple_SetItem(pArgs, i, pValue);
			}
			if(verbose){
				fprintf(stderr,">>> Passed arguments to 'sparse_matrix_generator_wrapper' function...\n");
				fprintf(stderr,">>> Proceeding to execution of Python function...\n");
			}

			double py_s, py_f, py_exec;
			PyObject *ret;
			py_s = getTimestamp();
			ret = PyObject_CallObject(pFunc, pArgs);
			py_f = getTimestamp();
			py_exec = (py_f - py_s)/(1000);
			if(verbose)
				fprintf(stderr,">>> >>> Execution of Python function : %.3f ms elapsed\n", py_exec);

			PyObject *obj_row_ptr, *obj_col_ind, *obj_nnz, *obj_density, *obj_mem_range, *obj_avg_nnz_per_row, *obj_std_nnz_per_row, *obj_avg_bw, *obj_std_bw, *obj_avg_sc, *obj_std_sc, *obj_time1, *obj_time2;
			PyArg_ParseTuple(ret,"O|O|O|O|O|O|O|O|O|O|O|O|O:ref", &obj_row_ptr, &obj_col_ind, &obj_nnz, &obj_density, &obj_mem_range, &obj_avg_nnz_per_row, &obj_std_nnz_per_row, &obj_avg_bw, &obj_std_bw, &obj_avg_sc, &obj_std_sc, &obj_time1, &obj_time2);

			if(verbose)
				fprintf(stderr,">>> Returned from function. Time to parse return variables...\n");

			Py_DECREF(pArgs);
			if (ret != NULL) {
				py_nnz = PyLong_AsLong(obj_nnz);
				py_density = PyFloat_AsDouble(obj_density);
				strcpy(py_mem_range, PyUnicode_AsUTF8(obj_mem_range));
				py_avg_nnz_per_row = PyFloat_AsDouble(obj_avg_nnz_per_row);
				py_std_nnz_per_row = PyFloat_AsDouble(obj_std_nnz_per_row);
				py_avg_bw = PyFloat_AsDouble(obj_avg_bw);
				py_std_bw = PyFloat_AsDouble(obj_std_bw);
				py_avg_sc = PyFloat_AsDouble(obj_avg_sc);
				py_std_sc = PyFloat_AsDouble(obj_std_sc);
				py_time1 = PyFloat_AsDouble(obj_time1);
				py_time2 = PyFloat_AsDouble(obj_time2);

				Py_DECREF(obj_nnz);
				Py_DECREF(obj_density);
				// Py_DECREF(obj_mem_range); // if i decref the "string" arg it causes segmentation fault later in Py_FinalizeEx
				Py_DECREF(obj_avg_nnz_per_row);
				Py_DECREF(obj_std_nnz_per_row);
				Py_DECREF(obj_avg_bw);
				Py_DECREF(obj_std_bw);
				Py_DECREF(obj_avg_sc);
				Py_DECREF(obj_std_sc);
				Py_DECREF(obj_time1);
				Py_DECREF(obj_time2);
			}
			else {
				Py_DECREF(pFunc);
				Py_DECREF(pModule);
				PyErr_Print();
				fprintf(stderr,"Call failed\n");
				return NULL;
			}
			if(verbose)
				fprintf(stderr,">>> Parsed scalar return variables successfully.\n");
			if(py_nnz != 0){
				unsigned int nr_rows = atoi(argv[starting_point]), nr_cols = atoi(argv[starting_point]), nr_nzeros = py_nnz;
				double density = py_density, diagonal_factor = strtod(argv[starting_point+5],NULL);
				char mem_range[128];
				int seed = atoi(argv[starting_point+6]);
				double avg_nnz_per_row = py_avg_nnz_per_row, std_nnz_per_row = py_std_nnz_per_row, avg_bw = py_avg_bw, std_bw = py_std_bw, avg_sc = py_avg_sc, std_sc = py_std_sc;
				double time1 = py_time1, time2 = py_time2;

				strcpy(mem_range, py_mem_range);

				if(verbose)
					fprintf(stderr,">>> Creating csr_matrix struct...\n");
				matrix = create_csr_matrix(nr_rows, nr_cols, nr_nzeros,
										   density, mem_range,
										   avg_nnz_per_row, std_nnz_per_row,
										   seed, argv[starting_point+3], argv[starting_point+4], diagonal_factor,
										   avg_bw, std_bw, avg_sc, std_sc,
										   time1, time2);
				if(verbose)
					fprintf(stderr,">>> Time to fill row_ptr, col_ind with values returned from function. Fill values too with random values.\n");

				double arr_s, arr_t1, arr_t2, arr_f, arr_row, arr_col, arr_val;
				arr_s = getTimestamp();

				// need to convert it to unsigned int (long is 8 bytes, unsigned int is 4 bytes)
				for(unsigned int k=0; k<matrix->nr_rows+1; k++)
					matrix->row_ptr[k] = (IndexType)PyLong_AsLong(PyList_GetItem(obj_row_ptr, (Py_ssize_t)k)); 
				arr_t1 = getTimestamp();
				for(unsigned int k=0; k<matrix->nr_nzeros; k++)
					matrix->col_ind[k] = (IndexType)PyLong_AsLong(PyList_GetItem(obj_col_ind, (Py_ssize_t)k));
				arr_t2 = getTimestamp();

				for(unsigned int k=0; k<matrix->nr_nzeros; k++)
					matrix->values[k] = sample_values[k%14];
				arr_f = getTimestamp();
				
				arr_row = (arr_t1 - arr_s)/(1000);
				arr_col = (arr_t2 - arr_t1)/(1000);
				arr_val = (arr_f - arr_t2)/(1000);

				if(verbose){
					fprintf(stderr,">>> >>> Filling values completed : %.3f + %.3f + %.3f  = %.3f ms elapsed\n", arr_row, arr_col, arr_val, arr_row+arr_col+arr_val);
					fprintf(stderr,">>> Completed artificial matrix generation successfully! Returning to main function.\n");				
				}
				Py_DECREF(ret);
				Py_DECREF(obj_row_ptr);
				Py_DECREF(obj_col_ind);
			}
			else{
				fprintf(stderr, ">>> sparse_matrix_generator did not produce any result. The given matrix features produce a matrix with memory footprint outside of the given range (4MB - 4096MB).\n");
				fprintf(stderr, ">>> Returning to main function with NULL.\n");
				Py_DECREF(ret);
				Py_DECREF(obj_row_ptr);
				Py_DECREF(obj_col_ind);
				return NULL;
			}
		}
		else {
			if (PyErr_Occurred())
				PyErr_Print();
			fprintf(stderr, "Cannot find function \"%s\"\n", argv[2]);
		}
		Py_XDECREF(pFunc);
		Py_DECREF(pModule);
	}
	else {
		PyErr_Print();
		fprintf(stderr, "Failed to load \"%s\"\n", argv[1]);
		return NULL;
	}
	if (Py_FinalizeEx() < 0)
		return NULL;
	return matrix;
}