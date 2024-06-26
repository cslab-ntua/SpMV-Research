//
// Created by genshen on 2021/7/5.
//

#ifndef SPMV_ACC_MEM_BANDWIDTH_H
#define SPMV_ACC_MEM_BANDWIDTH_H

void create_hip_event(hipEvent_t &start, hipEvent_t &stop) {
  hipEventCreate(&start);
  hipEventCreate(&stop);
}

inline int _bytes(const int nnz, const int rows, const int size_of_index, const int size_of_value) {
  //  bytes  = y[] read + y[] write, offset index (start and end), matrix values, column index, x[].
  return size_of_value * rows * 2 + size_of_index * rows * 2 + nnz * size_of_value + nnz * size_of_index +
         nnz * size_of_value;
}

#define WITH_MEM_BANDWIDTH_START                                                                                       \
  {                                                                                                                    \
    hipEvent_t start, stop;                                                                                            \
    create_hip_event(start, stop);                                                                                     \
                                                                                                                       \
    hipEventRecord(start, NULL);

#define WITH_MEM_BANDWIDTH_END(nnz, rows, size_of_index, size_of_value)                                                \
  hipEventRecord(stop, NULL);                                                                                          \
  hipEventSynchronize(stop);                                                                                           \
                                                                                                                       \
  float event_ms = 0.0f;                                                                                               \
  /* Time is computed in ms, with a resolution of approximately 1 us. */                                               \
  hipEventElapsedTime(&event_ms, start, stop);                                                                         \
                                                                                                                       \
  printf("elapsed time:%f (us)\n", event_ms * 1000);                                                                   \
  int bytes = _bytes(nnz, rows, size_of_index, size_of_value);                                                         \
  double bandwidth = (bytes + 0.0) / 1024 / 1024 / 1024 / (event_ms / 1000);                                           \
  printf("bandwidth: %f (GB/S)\n", bandwidth);                                                                         \
  }

#endif // SPMV_ACC_MEM_BANDWIDTH_H
