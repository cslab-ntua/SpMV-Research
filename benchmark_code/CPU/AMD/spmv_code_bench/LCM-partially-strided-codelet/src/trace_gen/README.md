## Example

```C++
/// Loading matrix A here

int num_thread = 8;
 auto *sm = new SpMVModel(A->n, A->m, A->nnz, A->p, A->i);
 format::timing_measurement t1;
 t1.start_timer();
 auto trs = sm->generate_trace(8);
 auto tgen = t1.measure_elapsed_time();
 auto trace_array = trs[0]->MemAddr(); // This is the array that has traces
 for (int i = 0; i < trs[0]->_num_partitions; ++i) {
  trs[i]->print();
  std::cout<<"\n\n";
 }
 std::cout<<": "<<tgen<<"\n";
```