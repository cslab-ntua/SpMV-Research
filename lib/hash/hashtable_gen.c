#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <omp.h>

#include "debug.h"
#include "macros/constants.h"
#include "hash/hash.h"
#include "omp_functions.h"
#include "parallel_util.h"

#include "hashtable_gen.h"


//==========================================================================================================================================
//= User Functions Declarations
//==========================================================================================================================================


/* It is necessary for the user to specify 'HASHTABLE_GEN_KEY_IS_REF',
 * because there are two possibilities for pointers:
 *     - use the address as a key
 *     - use the referenced memory region as a key
 *
 * If 'HASHTABLE_GEN_KEY_IS_REF' is true, then the user also has to define the
 * function 'hashtable_sizeof_key' that returns the number of bytes of the
 * referenced memory region.
 * Warning:
 *     Problem with comparison for equality, extract from C: A Reference Manual by Harbison and Steele:
 *         "Structures and unions cannot be compared for equality, even though assignment for these types is allowed.
 *          The gaps in structures and unions caused by alignment restrictions could contain arbitrary values,
 *          and compensating for this would impose an unacceptable overhead on the equality comparison or on all operations that modified structure and union types."
 *     This means that comparing keys byte to byte (e.g. 'memcmp()') CAN ACTUALLY FAIL if 'key' type is a structure.
 *
 * If 'HASHTABLE_GEN_KEY_IS_REF' is false, we expect a basic type whose members can be
 * compared with the standard operators (==, <, <=, >, >=), i.e. NOT a struct or
 * constant size array.
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

#if !HASHTABLE_GEN_VALUE_SAME_AS_KEY
	#undef  _TYPE_V
	#define _TYPE_V  HASHTABLE_GEN_EXPAND(_TYPE_V)
	typedef HASHTABLE_GEN_TYPE_2  _TYPE_V;
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


//------------------------------------------------------------------------------------------------------------------------------------------
//- Semaphore
//------------------------------------------------------------------------------------------------------------------------------------------


#undef  hashtable_semaphore_lock
#define hashtable_semaphore_lock  HASHTABLE_GEN_EXPAND(hashtable_semaphore_lock)
static inline
int
hashtable_semaphore_lock(int32_t * sem)
{
	int num_threads = safe_omp_get_num_threads();
	int32_t prev;
	if (__atomic_load_n(sem, __ATOMIC_RELAXED) < 0)   // Someone already locked the semaphore.
		return 0;
	prev = __atomic_fetch_add(sem, -num_threads, __ATOMIC_ACQUIRE);
	if (prev < 0)
	{
		__atomic_fetch_add(sem, num_threads, __ATOMIC_RELEASE);
		return 0;
	}
	while (1)
	{
		// if (__atomic_load_n(sem, __ATOMIC_RELAXED) == - (num_threads - 1)) // Everyone else out.
		if (__atomic_load_n(sem, __ATOMIC_RELAXED) == -num_threads) // Everyone out.
			break;
		hashtable_cpu_relax();
	}
	return 1;
}


#undef  hashtable_semaphore_unlock
#define hashtable_semaphore_unlock  HASHTABLE_GEN_EXPAND(hashtable_semaphore_unlock)
static inline
void
hashtable_semaphore_unlock(int32_t * sem)
{
	int num_threads = safe_omp_get_num_threads();
	__atomic_fetch_add(sem, num_threads, __ATOMIC_RELEASE);
}


#undef  hashtable_semaphore_enter
#define hashtable_semaphore_enter  HASHTABLE_GEN_EXPAND(hashtable_semaphore_enter)
static inline
void
hashtable_semaphore_enter(int32_t * sem)
{
	while (1)
	{
		if (__atomic_load_n(sem, __ATOMIC_RELAXED) >= 0)
		{
			__atomic_fetch_add(sem, 1, __ATOMIC_ACQUIRE);
			if (__atomic_load_n(sem, __ATOMIC_RELAXED) > 0)
				break;
			__atomic_fetch_add(sem, -1, __ATOMIC_RELEASE);
		}
		hashtable_cpu_relax();
	}
}


#undef  hashtable_semaphore_exit
#define hashtable_semaphore_exit  HASHTABLE_GEN_EXPAND(hashtable_semaphore_exit)
static inline
void
hashtable_semaphore_exit(int32_t * sem)
{
	__atomic_fetch_add(sem, -1, __ATOMIC_RELEASE);
}


//==========================================================================================================================================
//= Hash Functions
//==========================================================================================================================================


#undef  hashtable_hash_base
#define hashtable_hash_base  HASHTABLE_GEN_EXPAND(hashtable_hash_base)
static inline
uint64_t
hashtable_hash_base(__attribute__((unused)) struct hashtable * ht, _TYPE_K key, int variant)
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
hashtable_hash(struct hashtable * ht, _TYPE_K key)
{
	return hashtable_hash_base(ht, key, 0);
}


/* Computing multiple hash values can be more expensive than searching the buckets.
 */
#undef  hashtable_hash_alt
#define hashtable_hash_alt  HASHTABLE_GEN_EXPAND(hashtable_hash_alt)
__attribute__((unused))
static inline
uint64_t
hashtable_hash_alt(struct hashtable * ht, _TYPE_K key)
{
	return hashtable_hash_base(ht, key, 1);
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
	bucket->counter = 0;
	bucket->size = hashtable_bucket_initial_size;
	bucket->n = 0;
	// bucket->kv_pairs = (typeof(bucket->kv_pairs)) malloc(hashtable_bucket_initial_size * sizeof(*bucket->kv_pairs));
	bucket->kv_pairs = NULL;
	bucket->mru_keys = NULL;
	// This is a good opportunity to allocate the buffer memory, as it is serial access, parallel and distributes it across numa nodes.
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


#undef  hashtable_new
#define hashtable_new  HASHTABLE_GEN_EXPAND(hashtable_new)
struct hashtable *
hashtable_new(long buckets_n)
{
	struct hashtable * ht;
	ht = (typeof(ht)) malloc(sizeof(*ht));
	hashtable_init(ht, buckets_n);
	return ht;
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


#undef  hashtable_destroy
#define hashtable_destroy  HASHTABLE_GEN_EXPAND(hashtable_destroy)
void
hashtable_destroy(struct hashtable ** ht_ptr)
{
	hashtable_clean(*ht_ptr);
	free(*ht_ptr);
	*ht_ptr = NULL;
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
	space_is_malloced = bucket->kv_pairs < ht->buf_kv_pairs || bucket->kv_pairs > ht->buf_kv_pairs_end;
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
	space_is_malloced = bucket->kv_pairs < ht->buf_kv_pairs || bucket->kv_pairs > ht->buf_kv_pairs_end;
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
	struct HASHTABLE_GEN_MRU_s * mru_keys = bucket->mru_keys;
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
	hash = hashtable_hash(ht, key);
	pos = (((uint64_t) ((uint32_t) hash)) * ((uint64_t) ht->buckets_n)) >> 32;
	bucket = &ht->buckets[pos];
	return hashtable_bucket_contains(bucket, key);
}


//------------------------------------------------------------------------------------------------------------------------------------------
//- Insert
//------------------------------------------------------------------------------------------------------------------------------------------


#undef  hashtable_insert
#define hashtable_insert  HASHTABLE_GEN_EXPAND(hashtable_insert)
int 
hashtable_insert(struct hashtable * ht, _TYPE_K key
	#if !HASHTABLE_GEN_VALUE_SAME_AS_KEY
		, _TYPE_V value
	#endif
	)
{
	struct hashtable_bucket * bucket;
	uint64_t hash;
	long pos;
	long n;
	hash = hashtable_hash(ht, key);
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
	#if !HASHTABLE_GEN_VALUE_SAME_AS_KEY
		bucket->kv_pairs[n].value = value;
	#endif
	n++;
	bucket->n = n;
	return 1;
}


#undef  hashtable_insert_concurrent
#define hashtable_insert_concurrent  HASHTABLE_GEN_EXPAND(hashtable_insert_concurrent)
int 
hashtable_insert_concurrent(struct hashtable * ht, _TYPE_K key
	#if !HASHTABLE_GEN_VALUE_SAME_AS_KEY
		, _TYPE_V value
	#endif
	)
{
	struct hashtable_bucket * bucket;
	struct hashtable_kv_pair * kv_pairs;
	uint64_t hash;
	long pos;
	long n;
	char ownership;
	char zero = 0;
	hash = hashtable_hash(ht, key);
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
			struct HASHTABLE_GEN_MRU_s * mru_keys = bucket->mru_keys;
			if (bucket->counter >= HASHTABLE_GEN_MRU_N)
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
				bucket->counter++;
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
	#if !HASHTABLE_GEN_VALUE_SAME_AS_KEY
		bucket->kv_pairs[n].value = value;
	#endif
	n++;
	__atomic_store_n(&bucket->n, n, __ATOMIC_RELEASE);
	hashtable_spinlock_unlock(&bucket->lock);
	return 1;
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

#undef  hashtable_num_entries_concurrent
#define hashtable_num_entries_concurrent  HASHTABLE_GEN_EXPAND(hashtable_num_entries_concurrent)
long
hashtable_num_entries_concurrent(struct hashtable * ht)
{
	static long total_num = 0;
	int num_threads = safe_omp_get_num_threads();
	int tnum = omp_get_thread_num();
	long num;
	long i, i_s, i_e;
	num = 0;
	#pragma omp single nowait
	{
		total_num = 0;
	}
	loop_partitioner_balance_iterations(num_threads, tnum, 0, ht->buckets_n, &i_s, &i_e);
	for (i=i_s;i<i_e;i++)
		num += ht->buckets[i].n;
	__atomic_fetch_add(&total_num, num, __ATOMIC_RELAXED);
	#pragma omp barrier
	return total_num;
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


//==========================================================================================================================================
//= Includes Undefs
//==========================================================================================================================================

