#ifndef FAST_HASH_H
#define FAST_HASH_H
#include <cstdlib>
#include <iostream>
using namespace std;

#include <mm_malloc.h>

// Default hash.
template<class Key>
        struct default_hash {
            size_t operator()(const Key& key) {
                size_t hs = 5381;
                char *str = (char*)&key;
                for (int i = 0; i < sizeof(key); ++i) {
                    hs = ((hs << 5) + hs) + ((int)str[i]);  // hs * 33 + c
                }
                return hs;
            }
        };

// Default equal.
template<class Key>
        struct default_equal {
            bool operator()(const Key& key1, const Key& key2) {
                return key1 == key2;
            }
        };

template <class ValueType>
        ValueType sum(const ValueType& v) {
            return v;
        }

        template<class Key,
                class Value,
                        class Hash = default_hash<Key>,
                                class Equal = default_equal<Key> >
                                        class FastHash {
        public:
            int num_buckets;
            int remaining_buckets;
            Value* values; // Values are aligned.
            Key* keys; // Keys are used for comparison.
            Hash hash;
            Equal equal;

            FastHash() {
                values = 0;
                keys = 0;
            }

            FastHash(int num_buckets) {
                this->num_buckets = num_buckets;
                this->remaining_buckets = num_buckets;
                // Assume that the SIMD lane width is 64.
                this->values = (Value*)_mm_malloc(sizeof(Value)*num_buckets, 64);
                // this->values = (Value*)malloc(sizeof(Value)*num_buckets);
                this->keys = new Key[num_buckets];
                for (int i = 0; i < num_buckets; ++i) {
                    this->keys[i] = -1;  // Indicates it is empty.
                }
            }

            bool Empty() const {
                return num_buckets == remaining_buckets;
            }

            // Tells if a key is in the table.
            bool Exists(const Key& k) const {
                int i = hash(k)%num_buckets;
                while (1) {
                    if (keys[i] == -1) {
                        return false;
                    } else if (equal(keys[i], k)) {
                        return true;
                    }
                    i = (i + 1)%num_buckets;  // Sequential rehash.
                }
            }

            // Get the reference of the value from
            // the table according to the provided key.
            Value& operator[](Key& k) {
                int i = hash(k)%num_buckets;
                while (1) {
                    if (equal(keys[i], k)) {
                        return values[i];
                    }
                    i = (i + 1)%num_buckets;  // Sequential rehash.
                }
            }

            // Customized functionality for our use case:
            // Reduction.
            // Reduces (k, v) to the hash table, using
            // summation operation as the reduction operation.
            void Reduce(const Key& k, const Value& v) {
                int i = hash(k)%num_buckets;
                while (1) {

                    if (remaining_buckets == 0) {
                        cout << "Full..." << endl;
                        exit(0);
                    }

                    if (keys[i] == -1) {
                        keys[i] = k;
                        values[i] = v;
                        --remaining_buckets;
                        return;
                    } else if (equal(keys[i], k)) {
                        values[i] += v;
                        return;
                    }
                    i = (i + 1)%num_buckets;  // Sequential rehash.
                }
            }

            // Make the hash table clean.
            void clear() {
                remaining_buckets = num_buckets;
                for (int i = 0; i < num_buckets; ++i) {
                    keys[i] = -1;
                }
            }

            double CheckSum() const {
                double ret = 0;
                for (int i = 0; i < num_buckets; ++i) {
                    if (keys[i] != -1) {
                        ret += sum(values[i]);
                    }
                }
                return ret;
            }

            ~FastHash() {
                if (values) {
                    _mm_free(values);
                    values = 0;
                }
                if (keys) {
                    delete [] keys;
                    keys = 0;
                }
            }
        };

#endif /* FAST_HASH_H */
