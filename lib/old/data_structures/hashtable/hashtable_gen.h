#if !defined(HASHTABLE_GEN_VALUE_SAME_AS_KEY)
	#error "HASHTABLE_GEN_VALUE_SAME_AS_KEY not defined: [ Boolean ] Whether the key is also used as the value."
#elif !defined(HASHTABLE_GEN_KEY_IS_REF)
	#error "HASHTABLE_GEN_KEY_IS_REF not defined: [ Boolean ] Whether the key type is a reference (pointer) to the actual key."
#elif !defined(HASHTABLE_GEN_TYPE_1)
	#error "HASHTABLE_GEN_TYPE_1 not defined: [ Key Type ] Must be a basic type or array, NOT a structure (can't be safely compared without user intervention, non-deterministic hash)."
#elif (!HASHTABLE_GEN_VALUE_SAME_AS_KEY && !defined(HASHTABLE_GEN_TYPE_2))
	#error "HASHTABLE_GEN_TYPE_2 not defined: [ Value Type ]"
#elif (HASHTABLE_GEN_VALUE_SAME_AS_KEY && defined(HASHTABLE_GEN_TYPE_2))
	#error "HASHTABLE_GEN_TYPE_2 defines a value type, but HASHTABLE_GEN_VALUE_SAME_AS_KEY is defined as true (i.e. the keys also act as the values); undefine HASHTABLE_GEN_TYPE_2 or set HASHTABLE_GEN_VALUE_SAME_AS_KEY to false (0)."
#elif !defined(HASHTABLE_GEN_TYPE_3)
	#error "HASHTABLE_GEN_TYPE_3 not defined: [ Bucket Size Type (Signed Integer) ] Decide while considering the ratio of the hashtable size versus the total inserted data and also the possible collisions for the total amount of inserted data (a small %% of a big number can also be a big number)"
#elif !defined(HASHTABLE_GEN_SUFFIX)
	#error "HASHTABLE_GEN_SUFFIX not defined"
#endif

#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <stdint.h>

#include "macros/cpp_defines.h"
#include "macros/macrolib.h"
#include "genlib.h"


#define HASHTABLE_GEN_EXPAND(name)  CONCAT(name, HASHTABLE_GEN_SUFFIX)

#undef  _TYPE_K
#define _TYPE_K  HASHTABLE_GEN_EXPAND(_TYPE_K)
typedef HASHTABLE_GEN_TYPE_1  _TYPE_K;

#undef  _VC
#if !HASHTABLE_GEN_VALUE_SAME_AS_KEY
	#undef  _TYPE_V
	#define _TYPE_V  HASHTABLE_GEN_EXPAND(_TYPE_V)
	typedef HASHTABLE_GEN_TYPE_2  _TYPE_V;

	#define _VC(...)  __VA_ARGS__
#else
	#define _VC(...)
#endif

#undef  _TYPE_BS
#define _TYPE_BS  HASHTABLE_GEN_EXPAND(_TYPE_BS)
typedef HASHTABLE_GEN_TYPE_3  _TYPE_BS;


//==========================================================================================================================================
//= Structs
//==========================================================================================================================================


#undef  hashtable_kv_pair
#define hashtable_kv_pair  HASHTABLE_GEN_EXPAND(hashtable_kv_pair)
struct hashtable_kv_pair {
	_TYPE_K key;
	_VC(_TYPE_V value;)
};


/*
 * 'hashtable_mru_cache':
 *     Locking for concurrent operations is expensive.
 *     We always assume that the buckets will have about the same elements (i.e. good hash functions).
 *     Still, we need to locate the buckets with big traffic, i.e. values that appear multiple times -> failed insertions,
 *     and bypass the locking.
 *     Maybe we need some kind of static (i.e. never freed during inserts) MRU structure.
 */

#undef  HASHTABLE_GEN_MRU_N
#define HASHTABLE_GEN_MRU_N  16

#undef  hashtable_mru_cache
#define hashtable_mru_cache  HASHTABLE_GEN_EXPAND(hashtable_mru_cache)
struct hashtable_mru_cache {
	int n;
	int next;
	_TYPE_K keys[HASHTABLE_GEN_MRU_N];
};

/* 'fail_counter':
 *     There is a common pitfall for the 'mru_keys', which is cases like the values of symmetric matrices.
 *     For each of half of the nnz we add 'HASHTABLE_GEN_MRU_N * sizeof(_TYPE_K)' extra space.
 *
 *     We need some space guarantees when adding the 'mru_keys' arrays, so we only add them
 *     if we fail the insert at least some lower bound, keeping counts in 'fail_counter'.
 *     For example, if we use a lower bound:
 *         lb = HASHTABLE_GEN_MRU_N
 *     then we add 'HASHTABLE_GEN_MRU_N * sizeof(_TYPE_K)' space for each nnz,
 *     but only after 'lb' other nnz have failed insertion.
 *     Therefore, in the worst case we add space:
 *         HASHTABLE_GEN_MRU_N * sizeof(_TYPE_K) * (nnz / lb)  =  sizeof(_TYPE_K) * nnz
 */
#undef  hashtable_bucket
#define hashtable_bucket  HASHTABLE_GEN_EXPAND(hashtable_bucket)
struct hashtable_bucket {
	int8_t lock;
	int8_t fail_counter;
	_TYPE_BS size;
	_TYPE_BS n;
	struct hashtable_kv_pair * kv_pairs;
	struct hashtable_mru_cache * mru_keys;
};


#undef  hashtable
#define hashtable  HASHTABLE_GEN_EXPAND(hashtable)
struct hashtable {
	long buckets_n;
	struct hashtable_bucket * buckets;
	struct hashtable_kv_pair * buf_kv_pairs;
	struct hashtable_kv_pair * buf_kv_pairs_end;
	char * buf_kv_pairs_ownership;
};


//------------------------------------------------------------------------------------------------------------------------------------------
//- Constructors
//------------------------------------------------------------------------------------------------------------------------------------------


#undef  hashtable_init_serial
#define hashtable_init_serial  HASHTABLE_GEN_EXPAND(hashtable_init_serial)
void hashtable_init_serial(struct hashtable * ht, long buckets_n);
#undef  hashtable_init_concurrent
#define hashtable_init_concurrent  HASHTABLE_GEN_EXPAND(hashtable_init_concurrent)
void hashtable_init_concurrent(struct hashtable * ht, long buckets_n);
#undef  hashtable_init
#define hashtable_init  HASHTABLE_GEN_EXPAND(hashtable_init)
void hashtable_init(struct hashtable * ht, long buckets_n);

#undef  hashtable_new_serial
#define hashtable_new_serial  HASHTABLE_GEN_EXPAND(hashtable_new_serial)
struct hashtable * hashtable_new_serial(long buckets_n);
#undef  hashtable_new_concurrent
#define hashtable_new_concurrent  HASHTABLE_GEN_EXPAND(hashtable_new_concurrent)
struct hashtable * hashtable_new_concurrent(long buckets_n);
#undef  hashtable_new
#define hashtable_new  HASHTABLE_GEN_EXPAND(hashtable_new)
struct hashtable * hashtable_new(long buckets_n);


//------------------------------------------------------------------------------------------------------------------------------------------
//- Destructors
//------------------------------------------------------------------------------------------------------------------------------------------


#undef  hashtable_clean_serial
#define hashtable_clean_serial  HASHTABLE_GEN_EXPAND(hashtable_clean_serial)
void hashtable_clean_serial(struct hashtable * ht);
#undef  hashtable_clean_concurrent
#define hashtable_clean_concurrent  HASHTABLE_GEN_EXPAND(hashtable_clean_concurrent)
void hashtable_clean_concurrent(struct hashtable * ht);
#undef  hashtable_clean
#define hashtable_clean  HASHTABLE_GEN_EXPAND(hashtable_clean)
void hashtable_clean(struct hashtable * ht);

#undef  hashtable_destroy_serial
#define hashtable_destroy_serial  HASHTABLE_GEN_EXPAND(hashtable_destroy_serial)
void hashtable_destroy_serial(struct hashtable ** ht_ptr);
#undef  hashtable_destroy_concurrent
#define hashtable_destroy_concurrent  HASHTABLE_GEN_EXPAND(hashtable_destroy_concurrent)
void hashtable_destroy_concurrent(struct hashtable ** ht_ptr);
#undef  hashtable_destroy
#define hashtable_destroy  HASHTABLE_GEN_EXPAND(hashtable_destroy)
void hashtable_destroy(struct hashtable ** ht_ptr);


//==========================================================================================================================================
//= Functions
//==========================================================================================================================================


#undef  hashtable_empty_serial
#define hashtable_empty_serial  HASHTABLE_GEN_EXPAND(hashtable_empty_serial)
void hashtable_empty_serial(struct hashtable * ht);
#undef  hashtable_empty_concurrent
#define hashtable_empty_concurrent  HASHTABLE_GEN_EXPAND(hashtable_empty_concurrent)
void hashtable_empty_concurrent(struct hashtable * ht);
#undef  hashtable_empty
#define hashtable_empty  HASHTABLE_GEN_EXPAND(hashtable_empty)
void hashtable_empty(struct hashtable * ht);

#undef  hashtable_contains
#define hashtable_contains  HASHTABLE_GEN_EXPAND(hashtable_contains)
int hashtable_contains(struct hashtable * ht, _TYPE_K key);

#undef  hashtable_insert_serial
#define hashtable_insert_serial  HASHTABLE_GEN_EXPAND(hashtable_insert_serial)
int hashtable_insert_serial(struct hashtable * ht, _TYPE_K key  _VC(, _TYPE_V value));

#undef  hashtable_insert_concurrent
#define hashtable_insert_concurrent  HASHTABLE_GEN_EXPAND(hashtable_insert_concurrent)
int hashtable_insert_concurrent(struct hashtable * ht, _TYPE_K key  _VC(, _TYPE_V value));

#undef  hashtable_insert
#define hashtable_insert  HASHTABLE_GEN_EXPAND(hashtable_insert)
int hashtable_insert(struct hashtable * ht, _TYPE_K key  _VC(, _TYPE_V value));

#undef  hashtable_num_entries_serial
#define hashtable_num_entries_serial  HASHTABLE_GEN_EXPAND(hashtable_num_entries_serial)
long hashtable_num_entries_serial(struct hashtable * ht);
#undef  hashtable_num_entries_concurrent
#define hashtable_num_entries_concurrent  HASHTABLE_GEN_EXPAND(hashtable_num_entries_concurrent)
long hashtable_num_entries_concurrent(struct hashtable * ht);
#undef  hashtable_num_entries
#define hashtable_num_entries  HASHTABLE_GEN_EXPAND(hashtable_num_entries)
long hashtable_num_entries(struct hashtable * ht);

#undef  hashtable_entries_serial
#define hashtable_entries_serial  HASHTABLE_GEN_EXPAND(hashtable_entries_serial)
void hashtable_entries_serial(struct hashtable * ht, _TYPE_K ** keys_out, _VC(_TYPE_V ** values_out,) long * num_entries_out);
#undef  hashtable_entries_concurrent
#define hashtable_entries_concurrent  HASHTABLE_GEN_EXPAND(hashtable_entries_concurrent)
void hashtable_entries_concurrent(struct hashtable * ht, _TYPE_K ** keys_out, _VC(_TYPE_V ** values_out,) long * num_entries_out);
#undef  hashtable_entries
#define hashtable_entries  HASHTABLE_GEN_EXPAND(hashtable_entries)
void hashtable_entries(struct hashtable * ht, _TYPE_K ** keys_out, _VC(_TYPE_V ** values_out,) long * num_entries_out);

