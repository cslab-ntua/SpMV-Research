/******************************************************************************
 * Copyright (c) 2011, Duane Merrill.  All rights reserved.
 * Copyright (c) 2011-2016, NVIDIA CORPORATION.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the NVIDIA CORPORATION nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL NVIDIA CORPORATION BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ******************************************************************************/

/**
 * \file
 * cub::AgentReduce implements a stateful abstraction of CUDA thread blocks for participating in device-wide reduction .
 */

#pragma once

#include <iterator>

#include "../block/block_load.cuh"
#include "../block/block_reduce.cuh"
#include "../grid/grid_mapping.cuh"
#include "../grid/grid_queue.cuh"
#include "../grid/grid_even_share.cuh"
#include "../util_type.cuh"
#include "../iterator/cache_modified_input_iterator.cuh"
#include "../util_namespace.cuh"


/// Optional outer namespace(s)
CUB_NS_PREFIX

/// CUB namespace
namespace cub {


/******************************************************************************
 * Tuning policy types
 ******************************************************************************/

/**
 * Parameterizable tuning policy type for AgentReduce
 */
template <
    int                     _BLOCK_THREADS,         ///< Threads per thread block
    int                     _ITEMS_PER_THREAD,      ///< Items per thread (per tile of input)
    int                     _VECTOR_LOAD_LENGTH,    ///< Number of items per vectorized load
    BlockReduceAlgorithm    _BLOCK_ALGORITHM,       ///< Cooperative block-wide reduction algorithm to use
    CacheLoadModifier       _LOAD_MODIFIER,         ///< Cache load modifier for reading input elements
    GridMappingStrategy     _GRID_MAPPING>          ///< How to map tiles of input onto thread blocks
struct AgentReducePolicy
{
    enum
    {
        BLOCK_THREADS       = _BLOCK_THREADS,       ///< Threads per thread block
        ITEMS_PER_THREAD    = _ITEMS_PER_THREAD,    ///< Items per thread (per tile of input)
        VECTOR_LOAD_LENGTH  = _VECTOR_LOAD_LENGTH,  ///< Number of items per vectorized load
    };

    static const BlockReduceAlgorithm  BLOCK_ALGORITHM      = _BLOCK_ALGORITHM;     ///< Cooperative block-wide reduction algorithm to use
    static const CacheLoadModifier     LOAD_MODIFIER        = _LOAD_MODIFIER;       ///< Cache load modifier for reading input elements
    static const GridMappingStrategy   GRID_MAPPING         = _GRID_MAPPING;        ///< How to map tiles of input onto thread blocks
};



/******************************************************************************
 * Thread block abstractions
 ******************************************************************************/

/**
 * \brief AgentReduce implements a stateful abstraction of CUDA thread blocks for participating in device-wide reduction .
 *
 * Each thread reduces only the values it loads. If \p FIRST_TILE, this
 * partial reduction is stored into \p thread_aggregate.  Otherwise it is
 * accumulated into \p thread_aggregate.
 */
template <
    typename AgentReducePolicy,        ///< Parameterized AgentReducePolicy tuning policy type
    typename InputIteratorT,                ///< Random-access iterator type for input
    typename OffsetT_NV,                       ///< Signed integer type for global offsets
    typename ReductionOp>                   ///< Binary reduction operator type having member <tt>T operator()(const T &a, const T &b)</tt>
struct AgentReduce
{

    //---------------------------------------------------------------------
    // Types and constants
    //---------------------------------------------------------------------

    /// The value type of the input iterator
    typedef typename std::iterator_traits<InputIteratorT>::value_type T;

    /// Vector type of T for data movement
    typedef typename CubVector<T, AgentReducePolicy::VECTOR_LOAD_LENGTH>::Type VectorT;

    /// Input iterator wrapper type (for applying cache modifier)
    typedef typename If<IsPointer<InputIteratorT>::VALUE,
            CacheModifiedInputIterator<AgentReducePolicy::LOAD_MODIFIER, T, OffsetT_NV>,  // Wrap the native input pointer with CacheModifiedInputIterator
            InputIteratorT>::Type                                                            // Directly use the supplied input iterator type
        WrappedInputIteratorT;

    /// Constants
    enum
    {
        BLOCK_THREADS       = AgentReducePolicy::BLOCK_THREADS,
        ITEMS_PER_THREAD    = AgentReducePolicy::ITEMS_PER_THREAD,
        VECTOR_LOAD_LENGTH  = CUB_MIN(ITEMS_PER_THREAD, AgentReducePolicy::VECTOR_LOAD_LENGTH),
        TILE_ITEMS          = BLOCK_THREADS * ITEMS_PER_THREAD,

        // Can vectorize according to the policy if the input iterator is a native pointer to a primitive type
        ATTEMPT_VECTORIZATION   = (VECTOR_LOAD_LENGTH > 1) &&
                                    (ITEMS_PER_THREAD % VECTOR_LOAD_LENGTH == 0) &&
                                    (IsPointer<InputIteratorT>::VALUE) && Traits<T>::PRIMITIVE,

    };

    static const CacheLoadModifier    LOAD_MODIFIER   = AgentReducePolicy::LOAD_MODIFIER;
    static const BlockReduceAlgorithm BLOCK_ALGORITHM = AgentReducePolicy::BLOCK_ALGORITHM;

    /// Parameterized BlockReduce primitive
    typedef BlockReduce<T, BLOCK_THREADS, AgentReducePolicy::BLOCK_ALGORITHM> BlockReduceT;

    /// Shared memory type required by this thread block
    struct _TempStorage
    {
        typename BlockReduceT::TempStorage  reduce;
        OffsetT_NV                             dequeue_offset;
    };

    /// Alias wrapper allowing storage to be unioned
    struct TempStorage : Uninitialized<_TempStorage> {};


    //---------------------------------------------------------------------
    // Per-thread fields
    //---------------------------------------------------------------------

    _TempStorage&           temp_storage;       ///< Reference to temp_storage
    InputIteratorT          d_in;               ///< Input data to reduce
    WrappedInputIteratorT   d_wrapped_in;       ///< Wrapped input data to reduce
    ReductionOp             reduction_op;       ///< Binary reduction operator


    //---------------------------------------------------------------------
    // Utility
    //---------------------------------------------------------------------


    // Whether or not the input is aligned with the vector type (specialized for types we can vectorize)
    template <typename Iterator>
    static __device__ __forceinline__ bool IsAligned(
        Iterator        d_in,
        Int2Type<true>  can_vectorize)
    {
        return (size_t(d_in) & (sizeof(VectorT) - 1)) == 0;
    }

    // Whether or not the input is aligned with the vector type (specialized for types we cannot vectorize)
    template <typename Iterator>
    static __device__ __forceinline__ bool IsAligned(
        Iterator        d_in,
        Int2Type<false> can_vectorize)
    {
        return false;
    }


    //---------------------------------------------------------------------
    // Constructor
    //---------------------------------------------------------------------

    /**
     * Constructor
     */
    __device__ __forceinline__ AgentReduce(
        TempStorage&            temp_storage,       ///< Reference to temp_storage
        InputIteratorT          d_in,               ///< Input data to reduce
        ReductionOp             reduction_op)       ///< Binary reduction operator
    :
        temp_storage(temp_storage.Alias()),
        d_in(d_in),
        d_wrapped_in(d_in),
        reduction_op(reduction_op)
    {}


    //---------------------------------------------------------------------
    // Tile consumption
    //---------------------------------------------------------------------

    /**
     * Consume a full tile of input (non-vectorized)
     */
    template <int IS_FIRST_TILE>
    __device__ __forceinline__ void ConsumeTile(
        T                       &thread_aggregate,
        OffsetT_NV                 block_offset,       ///< The offset the tile to consume
        int                     valid_items,        ///< The number of valid items in the tile
        Int2Type<true>          is_full_tile,       ///< Whether or not this is a full tile
        Int2Type<false>         can_vectorize)      ///< Whether or not we can vectorize loads
    {
        T items[ITEMS_PER_THREAD];

        // Load items in striped fashion
        LoadDirectStriped<BLOCK_THREADS>(threadIdx.x, d_wrapped_in + block_offset, items);

        // Reduce items within each thread stripe
        thread_aggregate = (IS_FIRST_TILE) ?
            ThreadReduce(items, reduction_op) :
            ThreadReduce(items, reduction_op, thread_aggregate);
    }


    /**
     * Consume a full tile of input (vectorized)
     */
    template <int IS_FIRST_TILE>
    __device__ __forceinline__ void ConsumeTile(
        T                       &thread_aggregate,
        OffsetT_NV                 block_offset,       ///< The offset the tile to consume
        int                     valid_items,        ///< The number of valid items in the tile
        Int2Type<true>          is_full_tile,       ///< Whether or not this is a full tile
        Int2Type<true>          can_vectorize)      ///< Whether or not we can vectorize loads
    {
        // Alias items as an array of VectorT and load it in striped fashion
        enum { WORDS =  ITEMS_PER_THREAD / VECTOR_LOAD_LENGTH };

        T items[ITEMS_PER_THREAD];

        VectorT *vec_items = reinterpret_cast<VectorT*>(items);

        // Vector Input iterator wrapper type (for applying cache modifier)
        T *d_in_unqualified = const_cast<T*>(d_in) + block_offset + (threadIdx.x * VECTOR_LOAD_LENGTH);
        CacheModifiedInputIterator<AgentReducePolicy::LOAD_MODIFIER, VectorT, OffsetT_NV> d_vec_in(
            reinterpret_cast<VectorT*>(d_in_unqualified));

        #pragma unroll
        for (int i = 0; i < WORDS; ++i)
            vec_items[i] = d_vec_in[BLOCK_THREADS * i];

        // Reduce items within each thread stripe
        thread_aggregate = (IS_FIRST_TILE) ?
            ThreadReduce(items, reduction_op) :
            ThreadReduce(items, reduction_op, thread_aggregate);
    }


    /**
     * Consume a partial tile of input
     */
    template <int IS_FIRST_TILE, int CAN_VECTORIZE>
    __device__ __forceinline__ void ConsumeTile(
        T                       &thread_aggregate,
        OffsetT_NV                 block_offset,       ///< The offset the tile to consume
        int                     valid_items,        ///< The number of valid items in the tile
        Int2Type<false>         is_full_tile,       ///< Whether or not this is a full tile
        Int2Type<CAN_VECTORIZE> can_vectorize)      ///< Whether or not we can vectorize loads
    {
        // Partial tile
        int thread_offset = threadIdx.x;

        // Read first item
        if ((IS_FIRST_TILE) && (thread_offset < valid_items))
        {
            thread_aggregate = d_wrapped_in[block_offset + thread_offset];
            thread_offset += BLOCK_THREADS;
        }

        // Continue reading items (block-striped)
        while (thread_offset < valid_items)
        {
            thread_aggregate = reduction_op(
                thread_aggregate,
                d_wrapped_in[block_offset + thread_offset]);
            thread_offset += BLOCK_THREADS;
        }
    }


    //---------------------------------------------------------------
    // Consume a contiguous segment of tiles
    //---------------------------------------------------------------------

    /**
     * \brief Reduce a contiguous segment of input tiles
     */
    template <int CAN_VECTORIZE>
    __device__ __forceinline__ T ConsumeRange(
        OffsetT_NV block_offset,                       ///< [in] Threadblock begin offset (inclusive)
        OffsetT_NV block_end,                          ///< [in] Threadblock end offset (exclusive)
        Int2Type<CAN_VECTORIZE> can_vectorize)      ///< Whether or not we can vectorize loads
    {
        T thread_aggregate;

        if (block_offset + TILE_ITEMS > block_end)
        {
            // First tile isn't full (not all threads have valid items)
            int valid_items = block_end - block_offset;
            ConsumeTile<true>(thread_aggregate, block_offset, valid_items, Int2Type<false>(), can_vectorize);
            return BlockReduceT(temp_storage.reduce).Reduce(thread_aggregate, reduction_op, valid_items);
        }

        // At least one full block
        ConsumeTile<true>(thread_aggregate, block_offset, TILE_ITEMS, Int2Type<true>(), can_vectorize);
        block_offset += TILE_ITEMS;

        // Consume subsequent full tiles of input
        while (block_offset + TILE_ITEMS <= block_end)
        {
            ConsumeTile<false>(thread_aggregate, block_offset, TILE_ITEMS, Int2Type<true>(), can_vectorize);
            block_offset += TILE_ITEMS;
        }

        // Consume a partially-full tile
        if (block_offset < block_end)
        {
            int valid_items = block_end - block_offset;
            ConsumeTile<false>(thread_aggregate, block_offset, valid_items, Int2Type<false>(), can_vectorize);
        }

        // Compute block-wide reduction (all threads have valid items)
        return BlockReduceT(temp_storage.reduce).Reduce(thread_aggregate, reduction_op);
    }


    /**
     * \brief Reduce a contiguous segment of input tiles
     */
    __device__ __forceinline__ T ConsumeRange(
        OffsetT_NV block_offset,                       ///< [in] Threadblock begin offset (inclusive)
        OffsetT_NV block_end)                          ///< [in] Threadblock end offset (exclusive)
    {
        return (IsAligned(d_in + block_offset, Int2Type<ATTEMPT_VECTORIZATION>())) ?
            ConsumeRange(block_offset, block_end, Int2Type<true && ATTEMPT_VECTORIZATION>()) :
            ConsumeRange(block_offset, block_end, Int2Type<false && ATTEMPT_VECTORIZATION>());
    }


    /**
     * Reduce a contiguous segment of input tiles
     */
    __device__ __forceinline__ T ConsumeTiles(
        OffsetT_NV                             num_items,          ///< [in] Total number of global input items
        GridEvenShare<OffsetT_NV>              &even_share,        ///< [in] GridEvenShare descriptor
        GridQueue<OffsetT_NV>                  &queue,             ///< [in,out] GridQueue descriptor
        Int2Type<GRID_MAPPING_EVEN_SHARE>   is_even_share)      ///< [in] Marker type indicating this is an even-share mapping
    {
        // Initialize even-share descriptor for this thread block
        even_share.BlockInit();

        return (IsAligned(d_in, Int2Type<ATTEMPT_VECTORIZATION>())) ?
            ConsumeRange(even_share.block_offset, even_share.block_end, Int2Type<true && ATTEMPT_VECTORIZATION>()) :
            ConsumeRange(even_share.block_offset, even_share.block_end, Int2Type<false && ATTEMPT_VECTORIZATION>());

    }


    //---------------------------------------------------------------------
    // Dynamically consume tiles
    //---------------------------------------------------------------------

    /**
     * Dequeue and reduce tiles of items as part of a inter-block reduction
     */
    template <int CAN_VECTORIZE>
    __device__ __forceinline__ T ConsumeTiles(
        int                     num_items,          ///< Total number of input items
        GridQueue<OffsetT_NV>      queue,              ///< Queue descriptor for assigning tiles of work to thread blocks
        Int2Type<CAN_VECTORIZE> can_vectorize)      ///< Whether or not we can vectorize loads
    {
        // We give each thread block at least one tile of input.
        T thread_aggregate;
        OffsetT_NV block_offset = blockIdx.x * TILE_ITEMS;
        OffsetT_NV even_share_base = gridDim.x * TILE_ITEMS;

        if (block_offset + TILE_ITEMS > num_items)
        {
            // First tile isn't full (not all threads have valid items)
            int valid_items = num_items - block_offset;
            ConsumeTile<true>(thread_aggregate, block_offset, valid_items, Int2Type<false>(), can_vectorize);
            return BlockReduceT(temp_storage.reduce).Reduce(thread_aggregate, reduction_op, valid_items);
        }

        // Consume first full tile of input
        ConsumeTile<true>(thread_aggregate, block_offset, TILE_ITEMS, Int2Type<true>(), can_vectorize);

        if (num_items > even_share_base)
        {
            // Dequeue a tile of items
            if (threadIdx.x == 0)
                temp_storage.dequeue_offset = queue.Drain(TILE_ITEMS) + even_share_base;

            __syncthreads();

            // Grab tile offset and check if we're done with full tiles
            block_offset = temp_storage.dequeue_offset;

            // Consume more full tiles
            while (block_offset + TILE_ITEMS <= num_items)
            {
                ConsumeTile<false>(thread_aggregate, block_offset, TILE_ITEMS, Int2Type<true>(), can_vectorize);

                __syncthreads();

                // Dequeue a tile of items
                if (threadIdx.x == 0)
                    temp_storage.dequeue_offset = queue.Drain(TILE_ITEMS) + even_share_base;

                __syncthreads();

                // Grab tile offset and check if we're done with full tiles
                block_offset = temp_storage.dequeue_offset;
            }

            // Consume partial tile
            if (block_offset < num_items)
            {
                int valid_items = num_items - block_offset;
                ConsumeTile<false>(thread_aggregate, block_offset, valid_items, Int2Type<false>(), can_vectorize);
            }
        }

        // Compute block-wide reduction (all threads have valid items)
        return BlockReduceT(temp_storage.reduce).Reduce(thread_aggregate, reduction_op);

    }

    /**
     * Dequeue and reduce tiles of items as part of a inter-block reduction
     */
    __device__ __forceinline__ T ConsumeTiles(
        OffsetT_NV                         num_items,          ///< [in] Total number of global input items
        GridEvenShare<OffsetT_NV>          &even_share,        ///< [in] GridEvenShare descriptor
        GridQueue<OffsetT_NV>              &queue,             ///< [in,out] GridQueue descriptor
        Int2Type<GRID_MAPPING_DYNAMIC>  is_dynamic)         ///< [in] Marker type indicating this is a dynamic mapping
    {
        return (IsAligned(d_in, Int2Type<ATTEMPT_VECTORIZATION>())) ?
            ConsumeTiles(num_items, queue, Int2Type<true && ATTEMPT_VECTORIZATION>()) :
            ConsumeTiles(num_items, queue, Int2Type<false && ATTEMPT_VECTORIZATION>());
    }

};


}               // CUB namespace
CUB_NS_POSTFIX  // Optional outer namespace(s)

