#ifndef CSR_H
#define CSR_H 

#include "FastHash.h"
#include <iostream>
#include <vector>
using namespace std;

template<class ValueType>
        struct Csr{
            int num_rows;
            int num_cols;
            int nnz;
            int* rows;
            int* cols;
            // Indicates the size of each row.
            // To be set by users.
            // Only used for aligned CSRs.
            int* row_lens;

            // Indicates the first position in a row that has no data.
            ValueType* vals;

            Csr(int num_rows, int num_cols, int nnz) {
                this->num_rows = num_rows;
                this->num_cols = num_cols;
                this->nnz = nnz;
                rows = new int[num_rows + 1];

                // Here we assume the lane width is 64 bytes.
                cols = (int*)_mm_malloc(sizeof(int)*nnz, 64);
                vals = (ValueType*)_mm_malloc(sizeof(ValueType)*nnz, 64);

                row_lens = 0;
            }

            // Construct from hash table array.
            // table->size should be equal to num_rows.
            Csr(vector<FastHash<int, ValueType>* >& table,
                int num_rows, int num_cols) {
                row_lens = 0;
                this->num_rows = num_rows;
                this->num_cols = num_cols;
                this->nnz = 0;

                vector<ValueType> vals_tmp;
                vector<int> cols_tmp;
                vector<int> rows_tmp;
                for (int i = 0; i < table.size(); ++i) {
                    rows_tmp.push_back(vals_tmp.size());
                    // The hash table for this row is non-empty.
                    if (!(table[i]->Empty())) {
                        for (int j = 0; j < table[i]->num_buckets; ++j) {
                            if ((table[i]->keys)[j] != -1 && !((table[i]->values)[j] == 0)) {
                                vals_tmp.push_back((table[i]->values)[j]);
                                cols_tmp.push_back((table[i]->keys)[j]);
                                ++nnz;
                            }
                            // Clean the hash table.
                            (table[i]->keys)[j] = -1;
                        }
                        table[i]->remaining_buckets = table[i]->num_buckets;
                    }
                }

                rows = new int[num_rows+1];
                cols = (int*)_mm_malloc(sizeof(int)*nnz, 64);
                vals = (ValueType*)_mm_malloc(sizeof(ValueType)*nnz, 64);

                // Copy rows.
                for (int i = 0; i < num_rows; ++i) {
                    rows[i] = rows_tmp[i];
                }

                // Copy vals and cols.
                for (int i = 0; i < vals_tmp.size(); ++i) {
                    cols[i] = cols_tmp[i];
                    vals[i] = vals_tmp[i];
                }

                // Indicates the end.
                rows[num_rows] = nnz;
            }

            void print() {
                for (int i = 0; i < num_rows; ++i) {
                    cout << "Row: " << i << ":" << endl;
                    for (int j = rows[i]; j < rows[i+1]; ++j) {
                        cout << "(Col: " << cols[j] << " Val: " << vals[j] << "), ";
                    }
                    cout << endl;
                }
            }

            ~Csr() {
                delete [] rows;
                _mm_free(cols);
                _mm_free(vals);
                if (row_lens) {
                    delete [] row_lens;
                }
            }
        };

#endif /* CSR_H */