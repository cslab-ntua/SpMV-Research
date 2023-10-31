#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <omp.h>

#include "debug.h"
#include "hash/hash.h"
#include "omp_functions.h"
#include "parallel_util.h"

#include "hashtable_gen.h"


//==========================================================================================================================================
//= User Functions Declarations
//==========================================================================================================================================


/* It is necessary for the user to specify 'HASHTABLE_GEN_KEY_IS_REF',
 * because there are two options for pointers:
 *     - use the address as a key
 *     - use the referenced memory region as a key
 *
 * If 'HASHTABLE_GEN_KEY_IS_REF' is true, then the user also has to define the
 * function 'hashtable_sizeof_key' that returns the number of bytes of the
 * referenced memory region.
 *
 * Warning:
 *     Key type can NOT be a struct.
 *
 *     Extract from "C: A Reference Manual", by Harbison and Steele:
 *         "Structures and unions cannot be compared for equality, even though assignment for these types is allowed.
 *          The gaps in structures and unions caused by alignment restrictions could contain arbitrary values,
 *          and compensating for this would impose an unacceptable overhead on the equality comparison or on all operations that modified structure and union types."
 *
 *     This means that comparing keys byte to byte (e.g. 'memcmp()') CAN ACTUALLY FAIL if 'key' type is a structure.
 *
 *     Also, the hash of such types is not deterministic!
 *
 * If 'HASHTABLE_GEN_KEY_IS_REF' is false, we expect a basic type whose members can be
 * compared with the standard operators (==, <, <=, >, >=),
 * i.e. NOT a struct or constant size array.
 */


#if HASHTABLE_GEN_KEY_IS_REF

	#undef  hashtable_sizeof_key
	#define hashtable_sizeof_key  HASHTABLE_GEN_EXPAND(hashtable_sizeof_key)
	static size_t hashtable_sizeof_key(_TYPE_K key);

#else

	#undef  hashtable_sizeof_key
	#define hashtable_sizeof_key(key)  sizeof(key)

#endif


//==========================================================================================================================================
//= Includes
//==========================================================================================================================================


//==========================================================================================================================================
//= Local Defines
//==========================================================================================================================================


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


#if HASHTABLE_GEN_KEY_IS_REF

	#undef  hashtable_cast_key_to_char_array
	#define hashtable_cast_key_to_char_array(key)  ((const unsigned char *) key)

#else

	#undef  hashtable_cast_key_to_char_array
	#define hashtable_cast_key_to_char_array(key)  ((const unsigned char *) &key)

#endif


//==========================================================================================================================================
//------------------------------------------------------------------------------------------------------------------------------------------
//-                                                              Templates                                                                 -
//------------------------------------------------------------------------------------------------------------------------------------------
//==========================================================================================================================================


//==========================================================================================================================================
//= Locks
//==========================================================================================================================================


#undef  hashtable_cpu_relax
#define hashtable_cpu_relax  HASHTABLE_GEN_EXPAND(hashtable_cpu_relax)
static inline
void hashtable_cpu_relax()
{
	// __asm volatile ("pause" : : : "memory");
	__asm volatile ("rep; pause" : : : "memory");
	// __asm volatile ("rep; nop" : : : "memory");
}


//------------------------------------------------------------------------------------------------------------------------------------------
//- Spinlock - TTAS
//------------------------------------------------------------------------------------------------------------------------------------------


#undef  hashtable_spinlock_lock
#define hashtable_spinlock_lock  HASHTABLE_GEN_EXPAND(hashtable_spinlock_lock)
static inline
void
hashtable_spinlock_lock(int8_t * lock)
{
	while (1)
	{
		if (__atomic_load_n(lock, __ATOMIC_RELAXED) == 0)
		{
			if (!__atomic_exchange_n(lock, 1, __ATOMIC_ACQUIRE))
				break;
		}
		hashtable_cpu_relax();
	}
}


#undef  hashtable_spinlock_unlock
#define hashtable_spinlock_unlock  HASHTABLE_GEN_EXPAND(hashtable_spinlock_unlock)
static inline
void
hashtable_spinlock_unlock(int8_t * lock)
{
	__atomic_store_n(lock, 0, __ATOMIC_RELEASE);
}


//==========================================================================================================================================
//= Hash Functions
//==========================================================================================================================================


/* Notes:
 *     - Computing multiple hash values can be more expensive than searching the buckets.
 */


#undef  hashtable_hash_base
#define hashtable_hash_base  HASHTABLE_GEN_EXPAND(hashtable_hash_base)
static inline
uint64_t
hashtable_hash_base(_TYPE_K key, int variant)
{
	uint64_t hash;
	const long len = hashtable_sizeof_key(key);
	#if HASHTABLE_GEN_KEY_IS_REF
		hash = xorshift64(key, len, variant);
		// hash = fasthash64(key, len, variant);
	#else
		if (len <= 8)
		{
			const unsigned char * bytes = (const unsigned char *) &key;
			uint64_t v = 0;
			switch (len) {
				case 8: v ^= *((uint64_t *) (bytes)); break;
				case 7: v ^= (uint64_t) bytes[6] << 48; /* fallthrough */
				case 6: v ^= (uint64_t) bytes[5] << 40; /* fallthrough */
				case 5: v ^= (uint64_t) bytes[4] << 32; /* fallthrough */
				case 4: v ^= (uint64_t) *((uint32_t *) (bytes)); break;
				case 3: v ^= (uint64_t) bytes[2] << 16; /* fallthrough */
				case 2: v ^= (uint64_t) *((uint16_t *) (bytes)); break;
				case 1: v ^= (uint64_t) *((uint8_t *) (bytes)); break;
			}
			hash = xorshift64_int(v, 0, variant);
			// hash = fasthash64(&key, len, 0);
		}
		else
			hash = xorshift64(&key, len, variant);
	#endif
	return hash;
}


#undef  hashtable_hash
#define hashtable_hash  HASHTABLE_GEN_EXPAND(hashtable_hash)
static inline
uint64_t
hashtable_hash(_TYPE_K key)
{
	return hashtable_hash_base(key, 0);
}


//==========================================================================================================================================
//= Constructors / Destructors
//==========================================================================================================================================


//------------------------------------------------------------------------------------------------------------------------------------------
//- Buckets
//------------------------------------------------------------------------------------------------------------------------------------------


#undef  hashtable_bucket_initial_size
#define hashtable_bucket_initial_size  HASHTABLE_GEN_EXPAND(hashtable_bucket_initial_size)
static const long hashtable_bucket_initial_size = 2;


#undef  hashtable_bucket_init
#define hashtable_bucket_init  HASHTABLE_GEN_EXPAND(hashtable_bucket_init)
static inline
void
hashtable_bucket_init(struct hashtable * ht, long pos)
{
	long j;
	struct hashtable_bucket * bucket = &ht->buckets[pos];
	struct hashtable_kv_pair * kv_pairs;

	ht->buf_kv_pairs_ownership[pos] = 0;

	// bucket->size = 0;
	// bucket->n = 0;

	bucket->lock = 0;
	bucket->fail_counter = 0;
	bucket->size = hashtable_bucket_initial_size;
	bucket->n = 0;
	bucket->kv_pairs = NULL;
	bucket->mru_keys = NULL;
	// This is a good opportunity to touch the buffer memory, as it is serial access, parallel and distributes it across numa nodes.
	kv_pairs = &ht->buf_kv_pairs[hashtable_bucket_initial_size * pos];
	for (j=0;j<hashtable_bucket_initial_size;j++)
		kv_pairs[j].key = 0;
}


#undef  hashtable_bucket_clean
#define hashtable_bucket_clean  HASHTABLE_GEN_EXPAND(hashtable_bucket_clean)
static inline
void
hashtable_bucket_clean(struct hashtable * ht, long pos)
{
	struct hashtable_bucket * bucket = &ht->buckets[pos];
	long space_is_malloced;
	space_is_malloced = bucket->kv_pairs < ht->buf_kv_pairs || bucket->kv_pairs > ht->buf_kv_pairs_end;
	if (space_is_malloced)
		free(bucket->kv_pairs);
	bucket->kv_pairs = NULL;
	free(bucket->mru_keys);
	bucket->mru_keys = NULL;
}


//------------------------------------------------------------------------------------------------------------------------------------------
//- Constructors
//------------------------------------------------------------------------------------------------------------------------------------------


#undef  hashtable_init_base
#define hashtable_init_base  HASHTABLE_GEN_EXPAND(hashtable_init_base)
static
void
hashtable_init_base(struct hashtable * ht, long buckets_n)
{
	struct hashtable_bucket * buckets;
	struct hashtable_kv_pair * buf_kv_pairs;
	char * buf_kv_pairs_ownership;
	long buf_kv_pairs_n;

	// buckets_n *= 2;

	ht->buckets_n = buckets_n;

	buckets = (typeof(buckets)) malloc(buckets_n * sizeof(*buckets));
	ht->buckets = buckets;

	buf_kv_pairs_n = hashtable_bucket_initial_size * buckets_n;
	buf_kv_pairs = (typeof(buf_kv_pairs)) malloc(buf_kv_pairs_n * sizeof(*buf_kv_pairs));
	ht->buf_kv_pairs = buf_kv_pairs;
	ht->buf_kv_pairs_end = buf_kv_pairs + buf_kv_pairs_n;
	buf_kv_pairs_ownership = (typeof(buf_kv_pairs_ownership)) malloc(buf_kv_pairs_n * sizeof(*buf_kv_pairs_ownership));
	ht->buf_kv_pairs_ownership = buf_kv_pairs_ownership;
}

#undef  hashtable_init_serial
#define hashtable_init_serial  HASHTABLE_GEN_EXPAND(hashtable_init_serial)
void
hashtable_init_serial(struct hashtable * ht, long buckets_n)
{
	long i;
	hashtable_init_base(ht, buckets_n);
	for (i=0;i<ht->buckets_n;i++)
		hashtable_bucket_init(ht, i);
}

#undef  hashtable_init_concurrent
#define hashtable_init_concurrent  HASHTABLE_GEN_EXPAND(hashtable_init_concurrent)
void
hashtable_init_concurrent(struct hashtable * ht, long buckets_n)
{
	int num_threads = safe_omp_get_num_threads();
	int tnum = omp_get_thread_num();
	long i, i_s, i_e;
	#pragma omp single nowait
	{
		hashtable_init_base(ht, buckets_n);
	}
	#pragma omp barrier
	loop_partitioner_balance_iterations(num_threads, tnum, 0, ht->buckets_n, &i_s, &i_e);
	for (i=i_s;i<i_e;i++)
		hashtable_bucket_init(ht, i);
	#pragma omp barrier
}

#undef  hashtable_init
#define hashtable_init  HASHTABLE_GEN_EXPAND(hashtable_init)
void
hashtable_init(struct hashtable * ht, long buckets_n)
{
	if (omp_get_level() > 0)
		hashtable_init_serial(ht, buckets_n);
	else
	{
		#pragma omp parallel
		{
			hashtable_init_concurrent(ht, buckets_n);
		}
	}
}


#undef  hashtable_new_serial
#define hashtable_new_serial  HASHTABLE_GEN_EXPAND(hashtable_new_serial)
struct hashtable *
hashtable_new_serial(long buckets_n)
{
	struct hashtable * ht;
	ht = (typeof(ht)) malloc(sizeof(*ht));
	hashtable_init_serial(ht, buckets_n);
	return ht;
}

#undef  hashtable_new_concurrent
#define hashtable_new_concurrent  HASHTABLE_GEN_EXPAND(hashtable_new_concurrent)
struct hashtable *
hashtable_new_concurrent(long buckets_n)
{
	static struct hashtable * ht;
	#pragma omp single nowait
	{
		ht = (typeof(ht)) malloc(sizeof(*ht));
	}
	#pragma omp barrier
	hashtable_init_concurrent(ht, buckets_n);
	#pragma omp barrier
	return ht;
}

#undef  hashtable_new
#define hashtable_new  HASHTABLE_GEN_EXPAND(hashtable_new)
struct hashtable *
hashtable_new(long buckets_n)
{
	struct hashtable * ret;
	if (omp_get_level() > 0)
		return hashtable_new_serial(buckets_n);
	#pragma omp parallel
	{
		struct hashtable * ht = hashtable_new_concurrent(buckets_n);
		#pragma omp single nowait
		{
			ret = ht;
		}
	}
	return ret;
}


//------------------------------------------------------------------------------------------------------------------------------------------
//- Destructors
//------------------------------------------------------------------------------------------------------------------------------------------


#undef  hashtable_clean_free
#define hashtable_clean_free  HASHTABLE_GEN_EXPAND(hashtable_clean_free)
static
void
hashtable_clean_free(struct hashtable * ht)
{
	free(ht->buckets);
	free(ht->buf_kv_pairs);
	free(ht->buf_kv_pairs_ownership);
}

#undef  hashtable_clean_serial
#define hashtable_clean_serial  HASHTABLE_GEN_EXPAND(hashtable_clean_serial)
void
hashtable_clean_serial(struct hashtable * ht)
{
	long i;
	for (i=0;i<ht->buckets_n;i++)
		hashtable_bucket_clean(ht, i);
	hashtable_clean_free(ht);
}

#undef  hashtable_clean_concurrent
#define hashtable_clean_concurrent  HASHTABLE_GEN_EXPAND(hashtable_clean_concurrent)
void
hashtable_clean_concurrent(struct hashtable * ht)
{
	int num_threads = safe_omp_get_num_threads();
	int tnum = omp_get_thread_num();
	long i, i_s, i_e;
	loop_partitioner_balance_iterations(num_threads, tnum, 0, ht->buckets_n, &i_s, &i_e);
	for (i=i_s;i<i_e;i++)
		hashtable_bucket_clean(ht, i);
	#pragma omp barrier
	#pragma omp single
	{
		hashtable_clean_free(ht);
	}
	#pragma omp barrier
}

#undef  hashtable_clean
#define hashtable_clean  HASHTABLE_GEN_EXPAND(hashtable_clean)
void
hashtable_clean(struct hashtable * ht)
{
	if (omp_get_level() > 0)
		hashtable_clean_serial(ht);
	else
	{
		#pragma omp parallel
		{
			hashtable_clean_concurrent(ht);
		}
	}
}


#undef  hashtable_destroy_serial
#define hashtable_destroy_serial  HASHTABLE_GEN_EXPAND(hashtable_destroy_serial)
void
hashtable_destroy_serial(struct hashtable ** ht_ptr)
{
	hashtable_clean_serial(*ht_ptr);
	free(*ht_ptr);
	*ht_ptr = NULL;
}

#undef  hashtable_destroy_concurrent
#define hashtable_destroy_concurrent  HASHTABLE_GEN_EXPAND(hashtable_destroy_concurrent)
void
hashtable_destroy_concurrent(struct hashtable ** ht_ptr)
{
	hashtable_clean_concurrent(*ht_ptr);
	#pragma omp barrier
	#pragma omp single nowait
	{
		free(*ht_ptr);
	}
	#pragma omp barrier
	*ht_ptr = NULL; // Make NULL all thread local pointers.
}

#undef  hashtable_destroy
#define hashtable_destroy  HASHTABLE_GEN_EXPAND(hashtable_destroy)
void
hashtable_destroy(struct hashtable ** ht_ptr)
{
	if (omp_get_level() > 0)
		hashtable_destroy_serial(ht_ptr);
	else
	{
		#pragma omp parallel
		{
			hashtable_destroy_concurrent(ht_ptr);
		}
	}
}


//------------------------------------------------------------------------------------------------------------------------------------------
//- Empty
//------------------------------------------------------------------------------------------------------------------------------------------


#undef  hashtable_empty_serial
#define hashtable_empty_serial  HASHTABLE_GEN_EXPAND(hashtable_empty_serial)
void
hashtable_empty_serial(struct hashtable * ht)
{
	long i;
	for (i=0;i<ht->buckets_n;i++)
	{
		hashtable_bucket_clean(ht, i);
		hashtable_bucket_init(ht, i);
	}
}

#undef  hashtable_empty_concurrent
#define hashtable_empty_concurrent  HASHTABLE_GEN_EXPAND(hashtable_empty_concurrent)
void
hashtable_empty_concurrent(struct hashtable * ht)
{
	int num_threads = safe_omp_get_num_threads();
	int tnum = omp_get_thread_num();
	long i, i_s, i_e;
	loop_partitioner_balance_iterations(num_threads, tnum, 0, ht->buckets_n, &i_s, &i_e);
	for (i=i_s;i<i_e;i++)
	{
		hashtable_bucket_clean(ht, i);
		hashtable_bucket_init(ht, i);
	}
	#pragma omp barrier
}

#undef  hashtable_empty
#define hashtable_empty  HASHTABLE_GEN_EXPAND(hashtable_empty)
void
hashtable_empty(struct hashtable * ht)
{
	if (omp_get_level() > 0)
		hashtable_empty_serial(ht);
	else
	{
		#pragma omp parallel
		{
			hashtable_empty_concurrent(ht);
		}
	}
}


//------------------------------------------------------------------------------------------------------------------------------------------
//- Resize
//------------------------------------------------------------------------------------------------------------------------------------------


#undef  hashtable_bucket_resize
#define hashtable_bucket_resize  HASHTABLE_GEN_EXPAND(hashtable_bucket_resize)
static
void
hashtable_bucket_resize(struct hashtable * ht, long pos, long new_size)
{
	struct hashtable_bucket * bucket = &ht->buckets[pos];
	struct hashtable_kv_pair * kv_pairs;
	long space_is_malloced;
	long n = bucket->n;
	space_is_malloced = (bucket->kv_pairs < ht->buf_kv_pairs) || (bucket->kv_pairs > ht->buf_kv_pairs_end);
	if (new_size <= bucket->size)
		error("new size must be greater than older: old=%ld, new=%ld", bucket->size, new_size);
	if (!space_is_malloced)
	{
		long pos_next;
		pos_next = pos + bucket->size / hashtable_bucket_initial_size;
		if (ht->buf_kv_pairs_ownership[pos_next] <= 0)    // Only grows downward, so the only nodes we can't steal are root nodes (== 1).
		{
			ht->buf_kv_pairs_ownership[pos_next] = -1;
			bucket->size += hashtable_bucket_initial_size;
			return;
		}
	}
	kv_pairs = (typeof(kv_pairs)) malloc(new_size * sizeof(*kv_pairs));
	if (n > 0)
	{
		memcpy(kv_pairs, bucket->kv_pairs, n * sizeof(*kv_pairs));
	}
	if (space_is_malloced)
		free(bucket->kv_pairs);
	else
		ht->buf_kv_pairs_ownership[pos] = 0;   // Only need to free the root node.
	bucket->kv_pairs = kv_pairs;
	bucket->size = new_size;
}


// Bucket should be already locked.
#undef  hashtable_bucket_resize_concurrent
#define hashtable_bucket_resize_concurrent  HASHTABLE_GEN_EXPAND(hashtable_bucket_resize_concurrent)
static
void
hashtable_bucket_resize_concurrent(struct hashtable * ht, long pos, long new_size)
{
	struct hashtable_bucket * bucket = &ht->buckets[pos];
	struct hashtable_kv_pair * kv_pairs;
	long space_is_malloced;
	long n;
	char ownership;
	char zero = 0;
	n = bucket->n;
	space_is_malloced = (bucket->kv_pairs < ht->buf_kv_pairs) || (bucket->kv_pairs > ht->buf_kv_pairs_end);
	if (new_size <= bucket->size)
		error("new size must be greater than older: old=%ld, new=%ld", bucket->size, new_size);
	if (!space_is_malloced)
	{
		long pos_next;
		pos_next = pos + bucket->size / hashtable_bucket_initial_size;
		ownership = __atomic_load_n(&ht->buf_kv_pairs_ownership[pos_next], __ATOMIC_RELAXED);
		// Only grows downward, so the only nodes we can't steal are root nodes and when already claimed (== 1).
		// Therefore if already -1 we can use it without other checks.
		if (ownership == 0)
		{
			ownership = __atomic_compare_exchange_n(&ht->buf_kv_pairs_ownership[pos_next], &zero, -1, 0, __ATOMIC_RELAXED, __ATOMIC_RELAXED);
		}
		if (ownership < 0)
		{
			ht->buf_kv_pairs_ownership[pos_next] = -1;
			bucket->size += hashtable_bucket_initial_size;
			return;
		}
	}
	kv_pairs = (typeof(kv_pairs)) malloc(new_size * sizeof(*kv_pairs));
	if (n > 0)
	{
		memcpy(kv_pairs, bucket->kv_pairs, n * sizeof(*kv_pairs));
	}
	if (space_is_malloced)
		free(bucket->kv_pairs);
	else
		ht->buf_kv_pairs_ownership[pos] = 0;   // Only need to free the root node.
	bucket->kv_pairs = kv_pairs;
	bucket->size = new_size;
}


//==========================================================================================================================================
//= Operations
//==========================================================================================================================================


//------------------------------------------------------------------------------------------------------------------------------------------
//- Contains
//------------------------------------------------------------------------------------------------------------------------------------------


#undef  hashtable_bucket_contains
#define hashtable_bucket_contains  HASHTABLE_GEN_EXPAND(hashtable_bucket_contains)
static inline
int
hashtable_bucket_contains(struct hashtable_bucket * bucket, _TYPE_K target_key)
{
	struct hashtable_kv_pair * kv_pairs = bucket->kv_pairs;
	_TYPE_K key;
	long n, i;
	n = bucket->n;
	if (n == 0)
		return 0;
	#if HASHTABLE_GEN_KEY_IS_REF
		unsigned long target_len;
		const unsigned char * target_buf, * buf;
		target_len = hashtable_sizeof_key(target_key);
		target_buf = hashtable_cast_key_to_char_array(target_key);
	#endif
	for (i=0;i<n;i++)
	{
		key = kv_pairs[i].key;
		#if HASHTABLE_GEN_KEY_IS_REF
			if (hashtable_sizeof_key(key) != target_len)
				continue;
			buf = hashtable_cast_key_to_char_array(key);
			if (!memcmp(target_buf, buf, target_len))
				return 1;
		#else
			if (target_key == key)
				return 1;
		#endif
	}
	return 0;
}


#undef  hashtable_bucket_contains_MRU
#define hashtable_bucket_contains_MRU  HASHTABLE_GEN_EXPAND(hashtable_bucket_contains_MRU)
static inline
int
hashtable_bucket_contains_MRU(struct hashtable_bucket * bucket, _TYPE_K target_key)
{
	struct hashtable_mru_cache * mru_keys = bucket->mru_keys;
	_TYPE_K key;
	long n, i;
	n = mru_keys->n;
	if (n == 0)
		return 0;
	#if HASHTABLE_GEN_KEY_IS_REF
		unsigned long target_len;
		const unsigned char * target_buf, * buf;
		target_len = hashtable_sizeof_key(target_key);
		target_buf = hashtable_cast_key_to_char_array(target_key);
	#endif
	for (i=0;i<n;i++)
	{
		key = mru_keys->keys[i];
		#if HASHTABLE_GEN_KEY_IS_REF
			if (hashtable_sizeof_key(key) != target_len)
				continue;
			buf = hashtable_cast_key_to_char_array(key);
			if (!memcmp(target_buf, buf, target_len))
				return 1;
		#else
			if (target_key == key)
				return 1;
		#endif
	}
	return 0;
}


#undef  hashtable_contains
#define hashtable_contains  HASHTABLE_GEN_EXPAND(hashtable_contains)
int
hashtable_contains(struct hashtable * ht, _TYPE_K key)
{
	struct hashtable_bucket * bucket;
	uint64_t hash;
	long pos;
	hash = hashtable_hash(key);
	pos = (((uint64_t) ((uint32_t) hash)) * ((uint64_t) ht->buckets_n)) >> 32;
	bucket = &ht->buckets[pos];
	return hashtable_bucket_contains(bucket, key);
}


//------------------------------------------------------------------------------------------------------------------------------------------
//- Insert
//------------------------------------------------------------------------------------------------------------------------------------------


#undef  hashtable_insert_serial
#define hashtable_insert_serial  HASHTABLE_GEN_EXPAND(hashtable_insert_serial)
int 
hashtable_insert_serial(struct hashtable * ht, _TYPE_K key  _VC(, _TYPE_V value))
{
	struct hashtable_bucket * bucket;
	uint64_t hash;
	long pos;
	long n;
	hash = hashtable_hash(key);
	// pos = hash % ht->buckets_n;
	pos = (((uint64_t) ((uint32_t) hash)) * ((uint64_t) ht->buckets_n)) >> 32;
	bucket = &ht->buckets[pos];
	n = bucket->n;
	if (n > 0)
	{

		if (hashtable_bucket_contains(bucket, key))
			return 0;
		if (n >= bucket->size)
			hashtable_bucket_resize(ht, pos, 2 * bucket->size);
	}
	else
	{
		// Root kv_pairs are unmovable, so it is either free (0), or stolen (-1). Can't be ours (1) because n == 0 (first time inserting in this bucket).
		bucket->size = hashtable_bucket_initial_size;
		if (ht->buf_kv_pairs_ownership[pos] < 0)
		{
			bucket->kv_pairs = (typeof(bucket->kv_pairs)) malloc(hashtable_bucket_initial_size * sizeof(*bucket->kv_pairs));
		}
		else
		{
			ht->buf_kv_pairs_ownership[pos] = 1;
			bucket->kv_pairs = &ht->buf_kv_pairs[hashtable_bucket_initial_size * pos];
		}
	}
	bucket->kv_pairs[n].key = key;
	_VC(
		bucket->kv_pairs[n].value = value;
	)
	n++;
	bucket->n = n;
	return 1;
}


#undef  hashtable_insert_concurrent
#define hashtable_insert_concurrent  HASHTABLE_GEN_EXPAND(hashtable_insert_concurrent)
int 
hashtable_insert_concurrent(struct hashtable * ht, _TYPE_K key  _VC(, _TYPE_V value))
{
	struct hashtable_bucket * bucket;
	struct hashtable_kv_pair * kv_pairs;
	uint64_t hash;
	long pos;
	long n;
	char ownership;
	char zero = 0;
	hash = hashtable_hash(key);
	pos = (((uint64_t) ((uint32_t) hash)) * ((uint64_t) ht->buckets_n)) >> 32;
	bucket = &ht->buckets[pos];
	if (bucket->mru_keys != NULL)
	{
		if (hashtable_bucket_contains_MRU(bucket, key))
		{
			return 0;
		}
	}
	hashtable_spinlock_lock(&bucket->lock);
	n = bucket->n;
	if (n > 0)
	{
		if (hashtable_bucket_contains(bucket, key))
		{
			struct hashtable_mru_cache * mru_keys = bucket->mru_keys;
			if (bucket->fail_counter >= HASHTABLE_GEN_MRU_N)
			{
				if (mru_keys == NULL)
				{
					mru_keys = (typeof(mru_keys)) malloc(sizeof(*mru_keys));
					mru_keys->n = 0;
					mru_keys->next = 0;
					__atomic_store_n(&bucket->mru_keys, mru_keys, __ATOMIC_RELEASE);
				}
				mru_keys->keys[mru_keys->next] = key;
				mru_keys->next = (mru_keys->next + 1) % HASHTABLE_GEN_MRU_N;
				if (mru_keys->n < HASHTABLE_GEN_MRU_N)
					__atomic_store_n(&mru_keys->n, mru_keys->n + 1, __ATOMIC_RELEASE);
			}
			else
				bucket->fail_counter++;
			hashtable_spinlock_unlock(&bucket->lock);
			return 0;
		}
		if (n >= bucket->size)
		{
			hashtable_bucket_resize_concurrent(ht, pos, 2 * bucket->size);
		}
	}
	else
	{
		// Root kv_pairs are unmovable, so it is either free (0), or stolen (-1). Can't be ours (1) because n == 0 (first time inserting in this bucket).
		kv_pairs = &ht->buf_kv_pairs[hashtable_bucket_initial_size * pos]; // Here, to silence 'uninitialized' warning for when ownership > 0 (can't happen).
		bucket->size = hashtable_bucket_initial_size;
		ownership = __atomic_load_n(&ht->buf_kv_pairs_ownership[pos], __ATOMIC_RELAXED);
		if (ownership == 0)
		{
			ownership = __atomic_compare_exchange_n(&ht->buf_kv_pairs_ownership[pos], &zero, 1, 0, __ATOMIC_RELAXED, __ATOMIC_RELAXED);
		}
		if (ownership < 0) // Always check, since above claim might fail.
		{
			kv_pairs = (typeof(bucket->kv_pairs)) malloc(hashtable_bucket_initial_size * sizeof(*bucket->kv_pairs));
			bucket->size = hashtable_bucket_initial_size;
		}
		__atomic_store_n(&bucket->kv_pairs, kv_pairs, __ATOMIC_RELEASE);  // Only now we can release other threads that are waiting.
	}
	bucket->kv_pairs[n].key = key;
	_VC(
		bucket->kv_pairs[n].value = value;
	)
	n++;
	__atomic_store_n(&bucket->n, n, __ATOMIC_RELEASE);
	hashtable_spinlock_unlock(&bucket->lock);
	return 1;
}


#undef  hashtable_insert
#define hashtable_insert  HASHTABLE_GEN_EXPAND(hashtable_insert)
int 
hashtable_insert(struct hashtable * ht, _TYPE_K key  _VC(, _TYPE_V value))
{
	if (omp_get_level() > 0)
	{
		return hashtable_insert_concurrent(ht, key  _VC(, value));
	}
	else
	{
		return hashtable_insert_serial(ht, key  _VC(, value));
	}
}


//------------------------------------------------------------------------------------------------------------------------------------------
//- Number Of Entries
//------------------------------------------------------------------------------------------------------------------------------------------


#undef  hashtable_num_entries_serial
#define hashtable_num_entries_serial  HASHTABLE_GEN_EXPAND(hashtable_num_entries_serial)
long
hashtable_num_entries_serial(struct hashtable * ht)
{
	long total_num = 0;
	long i;
	for (i=0;i<ht->buckets_n;i++)
		total_num += ht->buckets[i].n;
	return total_num;
}

#undef  hashtable_reduce_add
#define hashtable_reduce_add  HASHTABLE_GEN_EXPAND(hashtable_reduce_add)
static inline
long
hashtable_reduce_add(long a, long b)
{
	return a + b;
}

#undef  hashtable_num_entries_concurrent_base
#define hashtable_num_entries_concurrent_base  HASHTABLE_GEN_EXPAND(hashtable_num_entries_concurrent_base)
static
long
hashtable_num_entries_concurrent_base(struct hashtable * ht, long * local_offset_out)
{
	long total_num = 0, local_offset;
	int num_threads = safe_omp_get_num_threads();
	int tnum = omp_get_thread_num();
	long num;
	long i, i_s, i_e;
	num = 0;
	loop_partitioner_balance_iterations(num_threads, tnum, 0, ht->buckets_n, &i_s, &i_e);
	for (i=i_s;i<i_e;i++)
		num += ht->buckets[i].n;
	omp_thread_reduce_global(hashtable_reduce_add, num, 0, 0, &local_offset, &total_num);
	if (local_offset_out != NULL)
		*local_offset_out = local_offset;
	return total_num;
}

#undef  hashtable_num_entries_concurrent
#define hashtable_num_entries_concurrent  HASHTABLE_GEN_EXPAND(hashtable_num_entries_concurrent)
long
hashtable_num_entries_concurrent(struct hashtable * ht)
{
	return hashtable_num_entries_concurrent_base(ht, NULL);
}

#undef  hashtable_num_entries
#define hashtable_num_entries  HASHTABLE_GEN_EXPAND(hashtable_num_entries)
long
hashtable_num_entries(struct hashtable * ht)
{
	long ret = 0;
	if (omp_get_level() > 0)
		return hashtable_num_entries_serial(ht);
	#pragma omp parallel
	{
		long agg = hashtable_num_entries_concurrent(ht);
		#pragma omp single nowait
		{
			ret = agg;
		}
	}
	return ret;
}


//------------------------------------------------------------------------------------------------------------------------------------------
//- Entries
//------------------------------------------------------------------------------------------------------------------------------------------


#undef  hashtable_entries_check_parameters
#define hashtable_entries_check_parameters  HASHTABLE_GEN_EXPAND(hashtable_entries_check_parameters)
static
void
hashtable_entries_check_parameters(_TYPE_K ** keys_out, _VC(_TYPE_V ** values_out,) long * num_entries_out)
{
	#if !HASHTABLE_GEN_VALUE_SAME_AS_KEY
		if (keys_out == NULL && values_out == NULL)
			error("Both 'keys_out' and 'values_out' return parameters are NULL");
	#else
		if (keys_out == NULL)
			error("'keys_out' return parameter is NULL");
	#endif
	if (num_entries_out == NULL)
		error("'num_entries_out' return parameter is NULL");
}


#undef  hashtable_entries_serial
#define hashtable_entries_serial  HASHTABLE_GEN_EXPAND(hashtable_entries_serial)
void
hashtable_entries_serial(struct hashtable * ht, _TYPE_K ** keys_out, _VC(_TYPE_V ** values_out,) long * num_entries_out)
{
	long i, j, k;
	_TYPE_K * keys;
	_VC(_TYPE_V * values;)
	long num_entries;
	struct hashtable_bucket * bucket;
	hashtable_entries_check_parameters(keys_out, _VC(values_out,) num_entries_out);
	num_entries = hashtable_num_entries_serial(ht);
	if (keys_out != NULL)
		keys = (typeof(keys)) malloc(num_entries * sizeof(*keys));
	_VC(
		if (values_out != NULL)
			values = (typeof(values)) malloc(num_entries * sizeof(*values));
	)
	k = 0;
	for (i=0;i<ht->buckets_n;i++)
	{
		bucket = &ht->buckets[i];
		for (j=0;j<bucket->n;j++)
		{
			if (keys_out != NULL)
				keys[k] = bucket->kv_pairs[j].key;
			_VC(
				if (values_out != NULL)
					values[k] = bucket->kv_pairs[j].value;
			)
			k++;
		}
	}
	if (keys_out != NULL)
		*keys_out = keys;
	_VC(
		if (values_out != NULL)
			*values_out = values;
	)
	*num_entries_out = num_entries;
}

#undef  hashtable_entries_concurrent
#define hashtable_entries_concurrent  HASHTABLE_GEN_EXPAND(hashtable_entries_concurrent)
void
hashtable_entries_concurrent(struct hashtable * ht, _TYPE_K ** keys_out, _VC(_TYPE_V ** values_out,) long * num_entries_out)
{
	static _TYPE_K * keys;
	_VC(
		static _TYPE_V * values;
	)
	int num_threads = safe_omp_get_num_threads();
	int tnum = omp_get_thread_num();
	long i, i_s, i_e, j, k;
	long num_entries;
	long local_offset;
	struct hashtable_bucket * bucket;
	hashtable_entries_check_parameters(keys_out, _VC(values_out,) num_entries_out);
	loop_partitioner_balance_iterations(num_threads, tnum, 0, ht->buckets_n, &i_s, &i_e);
	num_entries = hashtable_num_entries_concurrent_base(ht, &local_offset);
	#pragma omp single nowait
	{
		if (keys_out != NULL)
			keys = (typeof(keys)) malloc(num_entries * sizeof(*keys));
		_VC(
			if (values_out != NULL)
				values = (typeof(values)) malloc(num_entries * sizeof(*values));
		)
	}
	#pragma omp barrier
	k = local_offset;
	for (i=i_s;i<i_e;i++)
	{
		bucket = &ht->buckets[i];
		for (j=0;j<bucket->n;j++)
		{
			if (keys_out != NULL)
				keys[k] = bucket->kv_pairs[j].key;
			_VC(
				if (values_out != NULL)
					values[k] = bucket->kv_pairs[j].value;
			)
			k++;
		}
	}
	#pragma omp barrier
	if (keys_out != NULL)
		*keys_out = keys;
	_VC(
		if (values_out != NULL)
			*values_out = values;
	)
	*num_entries_out = num_entries;
}

#undef  hashtable_entries
#define hashtable_entries  HASHTABLE_GEN_EXPAND(hashtable_entries)
void
hashtable_entries(struct hashtable * ht, _TYPE_K ** keys_out, _VC(_TYPE_V ** values_out,) long * num_entries_out)
{
	hashtable_entries_check_parameters(keys_out, _VC(values_out,) num_entries_out);
	if (omp_get_level() > 0)
		hashtable_entries_serial(ht, (keys_out != NULL) ? keys_out : NULL, _VC((values_out != NULL) ? values_out : NULL,) num_entries_out);
	#pragma omp parallel
	{
		_TYPE_K * keys;
		_VC(
			static _TYPE_V * values;
		)
		long num_entries;
		hashtable_entries_concurrent(ht, (keys_out != NULL) ? &keys : NULL, _VC((values_out != NULL) ? &values : NULL,) num_entries_out);
		#pragma omp single nowait
		{
			if (keys_out != NULL)
				*keys_out = keys;
			_VC(
				if (values_out != NULL)
					*values_out = values;
			)
			*num_entries_out = num_entries;
		}
	}
}


//==========================================================================================================================================
//= Includes Undefs
//==========================================================================================================================================

