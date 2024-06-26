//设置计算类型为双精度
#include <iomanip>
#include <vector>

using namespace std;
typedef double dtype;

#ifdef gpu
#include <hipsparse.h> // cusparseSpMV
#include <rocsparse.h>
#endif

//定义矩阵和稠密向量指针
dtype **mat = NULL;              //稀疏矩阵
dtype *hX = NULL;                //密度向量
dtype *hhY = NULL;               //主机结果
dtype *hY = NULL;                //设备最终结果
dtype *temphY = NULL;            // Y向量的初始值，hy和hyy拷贝于此
dtype *value = NULL;             // csr存储格式稀疏值
int *colindex = NULL;            // csr列标识
int *rowptr = NULL;              // csr行标识
int a = 0;                       //稀疏矩阵非零值个数
int n = 0;                       //矩阵维度
int m = 0;                       //矩阵维度
int A_nnz = 0;                   //同a
int A_num_cols = 0;              //同m
int A_num_rows = 0;              //同n
int *dA_csrOffsets, *dA_columns; //设备
double *dA_values, *dX, *dY;
double alpha = 1; //稀疏矩阵的标量系数
double beta = 1;  //标量系数

//新demo程序从外部读取csr数据，使用vector装载程序
vector<double> csr_data;
vector<int> csr_indices;
vector<int> csr_indptr;
vector<double> dense_vector;

template <typename T> void init_csr_dense_vector(char *buf, vector<T> &vec) {
  char *p;
  p = strtok(buf, " ");
  while (p != NULL) {
    // printf("%s\n", p);
    bool isInt = std::is_same<T, int>::value;
    if (isInt == true)
      vec.push_back(atoi(p));
    else
      vec.push_back(atof(p));
    p = strtok(NULL, " ");
  }
}

template <typename T> void print_vector(vector<T> &vecTest) {
  for (auto it : vecTest) {
    cout << setprecision(10) << it << " ";
  }
  cout << endl << endl;
}

void read_file(char *path) {
  FILE *in = fopen(path, "r");
  char *buf = new char[1024L * 1024L * 1024L * 5L]; // 5g buff
  int line_num = 0;
  while (fgets(buf, sizeof(buf), in) != NULL) {
    // printf("%s\n\n", buf);

    if (line_num == 1)
      init_csr_dense_vector<double>(buf, csr_data);
    if (line_num == 2)
      init_csr_dense_vector<int>(buf, csr_indices);
    if (line_num == 3)
      init_csr_dense_vector<int>(buf, csr_indptr);
    if (line_num == 4)
      init_csr_dense_vector<double>(buf, dense_vector);
    line_num++;
  }
  delete[] buf;
  fclose(in);
}

void spmv(int alpha, int beta, dtype *value, int *rowptr, int *colindex, int m, int n, int a, dtype *x, dtype *y) {
  // calculate the matrix-vector multiply where matrix is stored in the form of CSR
  for (int i = 0; i < m; i++) {
    dtype y0 = 0;
    for (int j = rowptr[i]; j < rowptr[i + 1]; j++)
      y0 += value[j] * x[colindex[j]];
    // printf("%d,%d,%f,%f\n",alpha,beta,y0,y[i]);
    y[i] = alpha * y0 + beta * y[i];
  }
  return;
}

double rand_double(double min, double max) {
  double temp = min + (max - min) * double(rand() % 100) / double((101));
  // cout<<temp<<endl;
  return temp;
}

void generate_vector(int n, dtype *&x) {
  x = new dtype[n];
  for (int i = 0; i < n; i++)
    x[i] = rand_double(-1, 1);
  return;
}

void print_vector(int n, dtype *x) {
  for (int i = 0; i < n; i++)
    cout << x[i] << " ";
  cout << endl;
  return;
}

void print_vector(int n, int *x) {
  for (int i = 0; i < n; i++)
    cout << x[i] << " ";
  cout << endl;
  return;
}

#define HIP_CHECK(stat)                                                                                                \
  {                                                                                                                    \
    if (stat != hipSuccess) {                                                                                          \
      std::cerr << "Error: hip error in line " << __LINE__ << std::endl;                                               \
      exit(-1);                                                                                                        \
    }                                                                                                                  \
  }

#define ROCSPARSE_CHECK(stat)                                                                                          \
  {                                                                                                                    \
    if (stat != rocsparse_status_success) {                                                                            \
      std::cerr << "Error: rocsparse error in line " << __LINE__ << std::endl;                                         \
      exit(-1);                                                                                                        \
    }                                                                                                                  \
  }

void create_host_data() {
  generate_vector(n, hX); //外部读取数据覆盖hX
  generate_vector(m, temphY);
  generate_vector(m, hY); //实际数据依然来自随机生成
  generate_vector(m, hhY);

  //统一主机端和设备端的向量值
  memcpy(hY, temphY, m * sizeof(double));
  memcpy(hhY, temphY, m * sizeof(double));

  //重新初始化，将读到的数据拷贝到之前定义的数组减少改动量
  value = new dtype[csr_data.size()];
  colindex = new int[csr_indices.size()];
  rowptr = new int[csr_indptr.size()];
  a = csr_data.size();
  memcpy(hX, &dense_vector[0], n * sizeof(double));
  memcpy(colindex, &csr_indices[0], csr_indices.size() * sizeof(int));
  memcpy(rowptr, &csr_indptr[0], csr_indptr.size() * sizeof(int));
  memcpy(value, &csr_data[0], csr_data.size() * sizeof(double));
  // debug
  //  print_vector(n,hX);
}

void create_deivce_data() {
  A_nnz = a;
  A_num_cols = n;
  A_num_rows = m;

  // printf("a is %d value rowptr colindex hX hy as flows\n",a);
  // print_vector(a,value);
  // print_vector(n+1,rowptr);
  // print_vector(a,colindex);
  //  print_vector(n,temphY);
  //  print_vector(n,hY);
  ///////////////////////////////////////////

  HIP_CHECK(hipMalloc((void **)&dA_csrOffsets, (A_num_rows + 1) * sizeof(int)))
  HIP_CHECK(hipMalloc((void **)&dA_columns, A_nnz * sizeof(int)))
  HIP_CHECK(hipMalloc((void **)&dA_values, A_nnz * sizeof(double)))
  HIP_CHECK(hipMalloc((void **)&dX, A_num_cols * sizeof(double)))
  HIP_CHECK(hipMalloc((void **)&dY, A_num_rows * sizeof(double)))

  HIP_CHECK(hipMemcpy(dA_csrOffsets, rowptr, (A_num_rows + 1) * sizeof(int), hipMemcpyHostToDevice))
  HIP_CHECK(hipMemcpy(dA_columns, colindex, A_nnz * sizeof(int), hipMemcpyHostToDevice))
  HIP_CHECK(hipMemcpy(dA_values, value, A_nnz * sizeof(double), hipMemcpyHostToDevice))
  HIP_CHECK(hipMemcpy(dX, hX, A_num_cols * sizeof(double), hipMemcpyHostToDevice))
  HIP_CHECK(hipMemcpy(dY, temphY, A_num_rows * sizeof(double), hipMemcpyHostToDevice))
}

void verify(double *dy, double *hy, int n) {
  int total_validation = 0;
  for (int i = 0; i < n; i++) {
    if (fabs(dy[i] - hy[i]) / fabs(hy[i]) >= 1e-7) {
      cout << fabs(dy[i] - hy[i]) << " i:" << i << " dy[i]:" << dy[i] << " hy[i]:" << hy[i] << endl;
      cout << "Failed verification,please check your code\n" << endl;
      return;
    }
    total_validation = i;
    //  cout<<"i:"<<i<<" dy[i]:"<<dy[i]<<" hy[i]:"<<hy[i]<<endl;
  }
  cout << "Congratulation,pass " << total_validation + 1 << " validation!\n" << endl;
}

enum sparse_operation operation = operation_none;

#ifdef gpu
void rocsparse() {
  rocsparse_handle handle = NULL;
  rocsparse_spmat_descr matA;
  rocsparse_dnvec_descr vecX, vecY;
  void *dBuffer = NULL;
  size_t bufferSize = 0;
  ROCSPARSE_CHECK(rocsparse_create_handle(&handle));
  // Create sparse matrix A in CSR format
  ROCSPARSE_CHECK(rocsparse_create_csr_descr(&matA, A_num_rows, A_num_cols, A_nnz, dA_csrOffsets, dA_columns, dA_values,
                                             rocsparse_indextype_i32, rocsparse_indextype_i32,
                                             rocsparse_index_base_zero, rocsparse_datatype_f64_r));
  ROCSPARSE_CHECK(rocsparse_create_dnvec_descr(&vecX, A_num_cols, dX, rocsparse_datatype_f64_r));
  ROCSPARSE_CHECK(rocsparse_create_dnvec_descr(&vecY, A_num_rows, dY, rocsparse_datatype_f64_r));
  // allocate an external buffer if needed

  ROCSPARSE_CHECK(rocsparse_spmv(handle, rocsparse_operation_none, &alpha, matA, vecX, &beta, vecY,
                                 rocsparse_datatype_f64_r, rocsparse_spmv_alg_default, &bufferSize, nullptr));
  HIP_CHECK(hipMalloc(&dBuffer, bufferSize));
  ROCSPARSE_CHECK(rocsparse_spmv(handle, rocsparse_operation_none, &alpha, matA, vecX, &beta, vecY,
                                 rocsparse_datatype_f64_r, rocsparse_spmv_alg_default, &bufferSize, dBuffer));
}
#endif
