
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
 * cub::DeviceReduce provides device-wide, parallel operations for computing a reduction across a sequence of data items residing within device-accessible memory.
 */

#pragma once

#include <stdio.h>
#include <iterator>

#include "../../agent/agent_reduce.cuh"
#include "../../iterator/arg_index_input_iterator.cuh"
#include "../../thread/thread_operators.cuh"
#include "../../grid/grid_even_share.cuh"
#include "../../grid/grid_queue.cuh"
#include "../../iterator/arg_index_input_iterator.cuh"
#include "../../util_debug.cuh"
#include "../../util_device.cuh"
#include "../../util_namespace.cuh"

/// Optional outer namespace(s)
CUB_NS_PREFIX

/// CUB namespace
namespace cub {

/******************************************************************************
 * Kernel entry points
 *****************************************************************************/

/**
 * Reduce region kernel entry point (multi-block).  Computes privatized reductions, one per thread block.
 */
template <
    typename                ChainedPolicyT,             ///< Chained tuning policy
    typename                InputIteratorT,             ///< Random-access input iterator type for reading input items \iterator
    typename                OutputIteratorT,            ///< Output iterator type for recording the reduced aggregate \iterator
    typename                OffsetT_NV,                    ///< Signed integer type for global offsets
    typename                ReductionOpT>               ///< Binary reduction functor type having member <tt>T operator()(const T &a, const T &b)</tt>
__launch_bounds__ (int(ChainedPolicyT::ActivePolicy::ReducePolicy::BLOCK_THREADS))
__global__ void DeviceReduceKernel(
    InputIteratorT          d_in,                       ///< [in] Pointer to the input sequence of data items
    OutputIteratorT         d_out,                      ///< [out] Pointer to the output aggregate
    OffsetT_NV                 num_items,                  ///< [in] Total number of input data items
    GridEvenShare<OffsetT_NV>  even_share,                 ///< [in] Even-share descriptor for mapping an equal number of tiles onto each thread block
    GridQueue<OffsetT_NV>      queue,                      ///< [in] Drain queue descriptor for dynamically mapping tile data onto thread blocks
    ReductionOpT            reduction_op)               ///< [in] Binary reduction functor
{
    // Data type
    typedef typename std::iterator_traits<InputIteratorT>::value_type T;

    // Thread block type for reducing input tiles
    typedef AgentReduce<
            typename ChainedPolicyT::ActivePolicy::ReducePolicy,
            InputIteratorT,
            OffsetT_NV,
            ReductionOpT>
        AgentReduceT;

    // Shared memory storage
    __shared__ typename AgentReduceT::TempStorage temp_storage;

    // Consume input tiles
    T block_aggregate = AgentReduceT(temp_storage, d_in, reduction_op).ConsumeTiles(
        num_items,
        even_share,
        queue,
        Int2Type<ChainedPolicyT::ActivePolicy::ReducePolicy::GRID_MAPPING>());

    // Output result
    if (threadIdx.x == 0)
        d_out[blockIdx.x] = block_aggregate;
}


/**
 * Reduce a single tile kernel entry point (single-block).  Can be used to aggregate privatized threadblock reductions from a previous multi-block reduction pass.
 */
template <
    typename                ChainedPolicyT,             ///< Chained tuning policy
    typename                InputIteratorT,             ///< Random-access input iterator type for reading input items \iterator
    typename                OutputIteratorT,            ///< Output iterator type for recording the reduced aggregate \iterator
    typename                OffsetT_NV,                    ///< Signed integer type for global offsets
    typename                ReductionOpT,               ///< Binary reduction functor type having member <tt>T operator()(const T &a, const T &b)</tt>
    typename                T>                          ///< Data element type that is convertible to the \p value type of \p InputIteratorT
__launch_bounds__ (int(ChainedPolicyT::ActivePolicy::SingleTilePolicy::BLOCK_THREADS), 1)
__global__ void DeviceReduceSingleTileKernel(
    InputIteratorT          d_in,                       ///< [in] Pointer to the input sequence of data items
    OutputIteratorT         d_out,                      ///< [out] Pointer to the output aggregate
    OffsetT_NV                 num_items,                  ///< [in] Total number of input data items
    ReductionOpT            reduction_op,               ///< [in] Binary reduction functor
    T                       init)                       ///< [in] The initial value of the reduction
{
    // Thread block type for reducing input tiles
    typedef AgentReduce<
            typename ChainedPolicyT::ActivePolicy::SingleTilePolicy,
            InputIteratorT,
            OffsetT_NV,
            ReductionOpT>
        AgentReduceT;

    // Shared memory storage
    __shared__ typename AgentReduceT::TempStorage temp_storage;

    // Check if empty problem
    if (num_items == 0)
    {
        if (threadIdx.x == 0)
            *d_out = init;
        return;
    }

    // Consume input tiles
    T block_aggregate = AgentReduceT(temp_storage, d_in, reduction_op).ConsumeRange(
        OffsetT_NV(0),
        num_items);

    // Output result
    if (threadIdx.x == 0)
        *d_out = reduction_op(init, block_aggregate);
}


/// Normalize input iterator to segment offset
template <typename T, typename OffsetT_NV, typename IteratorT>
__device__ __forceinline__
void NormalizeReductionOutput(
    T &val,
    OffsetT_NV base_offset,
    IteratorT itr)
{}


/// Normalize input iterator to segment offset (specialized for arg-index)
template <typename KeyValuePairT, typename OffsetT_NV, typename WrappedIteratorT>
__device__ __forceinline__
void NormalizeReductionOutput(
    KeyValuePairT &val,
    OffsetT_NV base_offset,
    ArgIndexInputIterator<WrappedIteratorT, OffsetT_NV> itr)
{
    val.key -= base_offset;
}


/**
 * Segmented reduction (one block per segment)
 */
template <
    typename                ChainedPolicyT,             ///< Chained tuning policy
    typename                InputIteratorT,             ///< Random-access input iterator type for reading input items \iterator
    typename                OutputIteratorT,            ///< Output iterator type for recording the reduced aggregate \iterator
    typename                OffsetT_NV,                    ///< Signed integer type for global offsets
    typename                ReductionOpT,               ///< Binary reduction functor type having member <tt>T operator()(const T &a, const T &b)</tt>
    typename                T>                          ///< Data element type that is convertible to the \p value type of \p InputIteratorT
__launch_bounds__ (int(ChainedPolicyT::ActivePolicy::ReducePolicy::BLOCK_THREADS))
__global__ void DeviceSegmentedReduceKernel(
    InputIteratorT          d_in,                       ///< [in] Pointer to the input sequence of data items
    OutputIteratorT         d_out,                      ///< [out] Pointer to the output aggregate
    int                     *d_begin_offsets,           ///< [in] %Device-accessible pointer to the sequence of beginning offsets of length \p num_segments, such that <tt>d_begin_offsets[i]</tt> is the first element of the <em>i</em><sup>th</sup> data segment in <tt>d_keys_*</tt> and <tt>d_values_*</tt>
    int                     *d_end_offsets,             ///< [in] %Device-accessible pointer to the sequence of ending offsets of length \p num_segments, such that <tt>d_end_offsets[i]-1</tt> is the last element of the <em>i</em><sup>th</sup> data segment in <tt>d_keys_*</tt> and <tt>d_values_*</tt>.  If <tt>d_end_offsets[i]-1</tt> <= <tt>d_begin_offsets[i]</tt>, the <em>i</em><sup>th</sup> is considered empty.
    int                     num_segments,               ///< [in] The number of segments that comprise the sorting data
    ReductionOpT            reduction_op,               ///< [in] Binary reduction functor 
    T                       init)                       ///< [in] The initial value of the reduction
{
    // Thread block type for reducing input tiles
    typedef AgentReduce<
            typename ChainedPolicyT::ActivePolicy::ReducePolicy,
            InputIteratorT,
            OffsetT_NV,
            ReductionOpT>
        AgentReduceT;

    // Shared memory storage
    __shared__ typename AgentReduceT::TempStorage temp_storage;

    OffsetT_NV segment_begin   = d_begin_offsets[blockIdx.x];
    OffsetT_NV segment_end     = d_end_offsets[blockIdx.x];

    // Check if empty problem
    if (segment_begin == segment_end)
    {
        if (threadIdx.x == 0)
            d_out[blockIdx.x] = init;
        return;
    }

    // Consume input tiles
    T block_aggregate = AgentReduceT(temp_storage, d_in, reduction_op).ConsumeRange(
        segment_begin,
        segment_end);

    // Normalize as needed
    NormalizeReductionOutput(block_aggregate, segment_begin, d_in);

    if (threadIdx.x == 0)
        d_out[blockIdx.x] = reduction_op(init, block_aggregate);;
}




/******************************************************************************
 * Policy
 ******************************************************************************/

template <
    typename T,                 ///< Data type
    typename OffsetT_NV,           ///< Signed integer type for global offsets
    typename ReductionOpT>      ///< Binary reduction functor type having member <tt>T operator()(const T &a, const T &b)</tt> 
struct DeviceReducePolicy
{
    //------------------------------------------------------------------------------
    // Constants
    //------------------------------------------------------------------------------

    enum
    {
        // Whether this is for ArgMin or ArgMax
        IS_ARG_OP = Equals<ReductionOpT, ArgMin>::VALUE || Equals<ReductionOpT, ArgMax>::VALUE,

        // Relative size of T type to a 4-byte word
        SCALE_FACTOR_4B = (sizeof(T) + 3) / 4,

        // Relative size of T type to a 1-byte word
        SCALE_FACTOR_1B = sizeof(T),
    };

    //------------------------------------------------------------------------------
    // Architecture-specific tuning policies
    //------------------------------------------------------------------------------

    /// SM13
    struct Policy130 : ChainedPolicy<130, Policy130, Policy130>
    {
        // ReducePolicy
        typedef AgentReducePolicy<
                128,                                ///< Threads per thread block
                CUB_MAX(1, 8 / SCALE_FACTOR_4B),    ///< Items per thread per tile of input
                2,                                  ///< Number of items per vectorized load
                BLOCK_REDUCE_RAKING,                ///< Cooperative block-wide reduction algorithm to use
                LOAD_DEFAULT,                       ///< Cache load modifier
                GRID_MAPPING_EVEN_SHARE>            ///< How to map tiles of input onto thread blocks
            ReducePolicy;

        // SingleTilePolicy
        typedef ReducePolicy SingleTilePolicy;

        // SegmentedReducePolicy
        typedef ReducePolicy SegmentedReducePolicy;
    };


    /// SM20
    struct Policy200 : ChainedPolicy<200, Policy200, Policy130>
    {
        // ReducePolicy1B (GTX 580: 158.1 GB/s @ 192M 1B items)
        typedef AgentReducePolicy<
                192,                                ///< Threads per thread block
                CUB_MAX(1, 24 / SCALE_FACTOR_1B),   ///< Items per thread per tile of input
                4,                                  ///< Number of items per vectorized load
                BLOCK_REDUCE_RAKING,                ///< Cooperative block-wide reduction algorithm to use
                LOAD_DEFAULT,                       ///< Cache load modifier
                (sizeof(T) == 1) ?                  ///< How to map tiles of input onto thread blocks
                    GRID_MAPPING_EVEN_SHARE :
                    GRID_MAPPING_DYNAMIC>
            ReducePolicy1B;

        // ReducePolicy4B (GTX 580: 178.9 GB/s @ 48M 4B items)
        typedef AgentReducePolicy<
                128,                                ///< Threads per thread block
                CUB_MAX(1, 8 / SCALE_FACTOR_4B),    ///< Items per thread per tile of input
                4,                                  ///< Number of items per vectorized load
                BLOCK_REDUCE_RAKING,                ///< Cooperative block-wide reduction algorithm to use
                LOAD_DEFAULT,                       ///< Cache load modifier
                GRID_MAPPING_DYNAMIC>               ///< How to map tiles of input onto thread blocks
            ReducePolicy4B;

        // ReducePolicy
        typedef typename If<(sizeof(T) < 4),
            ReducePolicy1B,
            ReducePolicy4B>::Type ReducePolicy;

        // SingleTilePolicy
        typedef ReducePolicy SingleTilePolicy;

        // SegmentedReducePolicy
        typedef ReducePolicy SegmentedReducePolicy;
    };


    /// SM30
    struct Policy300 : ChainedPolicy<300, Policy300, Policy200>
    {
        // ReducePolicy (GTX670: 154.0 @ 48M 4B items)
        typedef AgentReducePolicy<
                256,                                ///< Threads per thread block
                CUB_MAX(1, 20 / SCALE_FACTOR_4B),    ///< Items per thread per tile of input
                2,                                  ///< Number of items per vectorized load
                BLOCK_REDUCE_WARP_REDUCTIONS,       ///< Cooperative block-wide reduction algorithm to use
                LOAD_DEFAULT,                       ///< Cache load modifier
                GRID_MAPPING_EVEN_SHARE>            ///< How to map tiles of input onto thread blocks
            ReducePolicy;

        // SingleTilePolicy
        typedef ReducePolicy SingleTilePolicy;

        // SegmentedReducePolicy
        typedef ReducePolicy SegmentedReducePolicy;
    };


    /// SM35
    struct Policy350 : ChainedPolicy<350, Policy350, Policy300>
    {
        // ReducePolicy1B (GTX Titan: 228.7 GB/s @ 192M 1B items)
        typedef AgentReducePolicy<
                128,                                ///< Threads per thread block
                CUB_MAX(1, 24 / SCALE_FACTOR_1B),   ///< Items per thread per tile of input
                4,                                  ///< Number of items per vectorized load
                BLOCK_REDUCE_WARP_REDUCTIONS,       ///< Cooperative block-wide reduction algorithm to use
                LOAD_LDG,                           ///< Cache load modifier
                GRID_MAPPING_DYNAMIC>               ///< How to map tiles of input onto thread blocks
            ReducePolicy1B;

        // ReducePolicy4B types (GTX Titan: 255.1 GB/s @ 48M 4B items)
        typedef AgentReducePolicy<
                256,                                ///< Threads per thread block
                CUB_MAX(1, 20 / SCALE_FACTOR_4B),   ///< Items per thread per tile of input
                4,                                  ///< Number of items per vectorized load
                BLOCK_REDUCE_WARP_REDUCTIONS,       ///< Cooperative block-wide reduction algorithm to use
                LOAD_LDG,                           ///< Cache load modifier
                GRID_MAPPING_DYNAMIC>               ///< How to map tiles of input onto thread blocks
            ReducePolicy4B;

        // ReducePolicy
        typedef typename If<(sizeof(T) < 4),
            ReducePolicy1B,
            ReducePolicy4B>::Type ReducePolicy;

        // SingleTilePolicy
        typedef ReducePolicy SingleTilePolicy;

        // SegmentedReducePolicy
        typedef ReducePolicy SegmentedReducePolicy;
    };


    /// MaxPolicy
    typedef Policy350 MaxPolicy;

};



/******************************************************************************
 * Single-problem dispatch
 ******************************************************************************/

/**
 * Utility class for dispatching the appropriately-tuned kernels for device-wide reduction
 */
template <
    typename InputIteratorT,    ///< Random-access input iterator type for reading input items \iterator
    typename OutputIteratorT,   ///< Output iterator type for recording the reduced aggregate \iterator
    typename OffsetT_NV,           ///< Signed integer type for global offsets
    typename ReductionOpT>      ///< Binary reduction functor type having member <tt>T operator()(const T &a, const T &b)</tt> 
struct DispatchReduce :
    DeviceReducePolicy<
        typename std::iterator_traits<InputIteratorT>::value_type,
        OffsetT_NV,
        ReductionOpT>
{
    //------------------------------------------------------------------------------
    // Constants
    //------------------------------------------------------------------------------

    // Data type of input iterator
    typedef typename std::iterator_traits<InputIteratorT>::value_type T;


    //------------------------------------------------------------------------------
    // Problem state
    //------------------------------------------------------------------------------

    void                *d_temp_storage;                ///< [in] %Device-accessible allocation of temporary storage.  When NULL, the required allocation size is written to \p temp_storage_bytes and no work is done.
    size_t              &temp_storage_bytes;            ///< [in,out] Reference to size in bytes of \p d_temp_storage allocation
    InputIteratorT      d_in;                           ///< [in] Pointer to the input sequence of data items
    OutputIteratorT     d_out;                          ///< [out] Pointer to the output aggregate
    OffsetT_NV             num_items;                      ///< [in] Total number of input items (i.e., length of \p d_in)
    ReductionOpT        reduction_op;                   ///< [in] Binary reduction functor 
    T                   init;                           ///< [in] The initial value of the reduction
    cudaStream_t        stream;                         ///< [in] CUDA stream to launch kernels within.  Default is stream<sub>0</sub>.
    bool                debug_synchronous;              ///< [in] Whether or not to synchronize the stream after every kernel launch to check for errors.  Also causes launch configurations to be printed to the console.  Default is \p false.
    int                 ptx_version;                    ///< [in] PTX version

    //------------------------------------------------------------------------------
    // Constructor
    //------------------------------------------------------------------------------

    /// Constructor
    CUB_RUNTIME_FUNCTION __forceinline__
    DispatchReduce(
        void*                   d_temp_storage,
        size_t                  &temp_storage_bytes,
        InputIteratorT          d_in,
        OutputIteratorT         d_out,
        OffsetT_NV                 num_items,
        ReductionOpT            reduction_op,
        T                       init,
        cudaStream_t            stream,
        bool                    debug_synchronous,
        int                     ptx_version)
    :
        d_temp_storage(d_temp_storage),
        temp_storage_bytes(temp_storage_bytes),
        d_in(d_in),
        d_out(d_out),
        num_items(num_items),
        reduction_op(reduction_op),
        init(init),
        stream(stream),
        debug_synchronous(debug_synchronous),
        ptx_version(ptx_version)
    {}


    //------------------------------------------------------------------------------
    // Small-problem (single tile) invocation
    //------------------------------------------------------------------------------

    /// Invoke a single block block to reduce in-core
    template <
        typename                ActivePolicyT,          ///< Umbrella policy active for the target device
        typename                SingleTileKernelT>      ///< Function type of cub::DeviceReduceSingleTileKernel
    CUB_RUNTIME_FUNCTION __forceinline__
    cudaError_t InvokeSingleTile(
        SingleTileKernelT       single_tile_kernel)     ///< [in] Kernel function pointer to parameterization of cub::DeviceReduceSingleTileKernel
    {
#ifndef CUB_RUNTIME_ENABLED

        // Kernel launch not supported from this device
        return CubDebug(cudaErrorNotSupported );
#else
        cudaError error = cudaSuccess;
        do
        {
            // Return if the caller is simply requesting the size of the storage allocation
            if (d_temp_storage == NULL)
            {
                temp_storage_bytes = 1;
                break;
            }

            // Log single_reduce_sweep_kernel configuration
            if (debug_synchronous) _CubLog("Invoking DeviceReduceSingleTileKernel<<<1, %d, 0, %lld>>>(), %d items per thread\n",
                ActivePolicyT::SingleTilePolicy::BLOCK_THREADS,
                (long long) stream,
                ActivePolicyT::SingleTilePolicy::ITEMS_PER_THREAD);

            // Invoke single_reduce_sweep_kernel
            single_tile_kernel<<<1, ActivePolicyT::SingleTilePolicy::BLOCK_THREADS, 0, stream>>>(
                d_in,
                d_out,
                num_items,
                reduction_op,
                init);

            // Check for failure to launch
            if (CubDebug(error = cudaPeekAtLastError())) break;

            // Sync the stream if specified to flush runtime errors
            if (debug_synchronous && (CubDebug(error = SyncStream(stream)))) break;
        }
        while (0);

        return error;

#endif // CUB_RUNTIME_ENABLED
    }


    //------------------------------------------------------------------------------
    // Normal problem size invocation (two-pass)
    //------------------------------------------------------------------------------

    /// Invoke two-passes to reduce
    template <
        typename                ActivePolicyT,              ///< Umbrella policy active for the target device
        typename                ReduceKernelT,              ///< Function type of cub::DeviceReduceKernel
        typename                SingleTileKernelT,          ///< Function type of cub::DeviceReduceSingleTileKernel
        typename                FillAndResetDrainKernelT>   ///< Function type of cub::FillAndResetDrainKernel
    CUB_RUNTIME_FUNCTION __forceinline__
    cudaError_t InvokePasses(
        ReduceKernelT               reduce_kernel,          ///< [in] Kernel function pointer to parameterization of cub::DeviceReduceKernel
        SingleTileKernelT           single_tile_kernel,     ///< [in] Kernel function pointer to parameterization of cub::DeviceReduceSingleTileKernel
        FillAndResetDrainKernelT    prepare_drain_kernel)   ///< [in] Kernel function pointer to parameterization of cub::FillAndResetDrainKernel
    {
#ifndef CUB_RUNTIME_ENABLED

        // Kernel launch not supported from this device
        return CubDebug(cudaErrorNotSupported );
#else

        cudaError error = cudaSuccess;
        do
        {
            // Get device ordinal
            int device_ordinal;
            if (CubDebug(error = cudaGetDevice(&device_ordinal))) break;

            // Get SM count
            int sm_count;
            if (CubDebug(error = cudaDeviceGetAttribute (&sm_count, cudaDevAttrMultiProcessorCount, device_ordinal))) break;

            // Init regular kernel configuration
            KernelConfig reduce_config;
            if (CubDebug(error = reduce_config.Init<typename ActivePolicyT::ReducePolicy>(reduce_kernel))) break;
            int reduce_device_occupancy = reduce_config.sm_occupancy * sm_count;

            // Even-share work distribution
            int max_blocks = reduce_device_occupancy * CUB_SUBSCRIPTION_FACTOR(ptx_version);
            GridEvenShare<OffsetT_NV> even_share(num_items, max_blocks, reduce_config.tile_size);

            // Temporary storage allocation requirements
            void* allocations[2];
            size_t allocation_sizes[2] =
            {
                max_blocks * sizeof(T),       // bytes needed for privatized block reductions
                GridQueue<int>::AllocationSize()    // bytes needed for grid queue descriptor
            };

            // Alias the temporary allocations from the single storage blob (or compute the necessary size of the blob)
            if (CubDebug(error = AliasTemporaries(d_temp_storage, temp_storage_bytes, allocations, allocation_sizes))) break;
            if (d_temp_storage == NULL)
            {
                // Return if the caller is simply requesting the size of the storage allocation
                return cudaSuccess;
            }

            // Alias the allocation for the privatized per-block reductions
            T *d_block_reductions = (T*) allocations[0];

            // Alias the allocation for the grid queue descriptor
            GridQueue<OffsetT_NV> queue(allocations[1]);

            // Get grid size for device_reduce_sweep_kernel
            int reduce_grid_size;
            if (ActivePolicyT::ReducePolicy::GRID_MAPPING == GRID_MAPPING_EVEN_SHARE)
            {
                // Work is distributed evenly
                reduce_grid_size = even_share.grid_size;
            }
            else if (ActivePolicyT::ReducePolicy::GRID_MAPPING == GRID_MAPPING_DYNAMIC)
            {
                // Work is distributed dynamically
                int num_tiles       = (num_items + reduce_config.tile_size - 1) / reduce_config.tile_size;
                reduce_grid_size    = (num_tiles < reduce_device_occupancy) ?
                                        num_tiles :                 // Not enough to fill the device with threadblocks
                                        reduce_device_occupancy;    // Fill the device with threadblocks

                // Prepare the dynamic queue descriptor if necessary
                if (debug_synchronous) _CubLog("Invoking prepare_drain_kernel<<<1, 1, 0, %lld>>>()\n", (long long) stream);

                // Invoke prepare_drain_kernel
                prepare_drain_kernel<<<1, 1, 0, stream>>>(queue, num_items);

                // Check for failure to launch
                if (CubDebug(error = cudaPeekAtLastError())) break;

                // Sync the stream if specified to flush runtime errors
                if (debug_synchronous && (CubDebug(error = SyncStream(stream)))) break;
            }
            else
            {
                error = CubDebug(cudaErrorNotSupported ); break;
            }

            // Log device_reduce_sweep_kernel configuration
            if (debug_synchronous) _CubLog("Invoking DeviceReduceKernel<<<%d, %d, 0, %lld>>>(), %d items per thread, %d SM occupancy\n",
                reduce_grid_size,
                ActivePolicyT::ReducePolicy::BLOCK_THREADS,
                (long long) stream,
                ActivePolicyT::ReducePolicy::ITEMS_PER_THREAD,
                reduce_config.sm_occupancy);

            // Invoke DeviceReduceKernel
            reduce_kernel<<<reduce_grid_size, ActivePolicyT::ReducePolicy::BLOCK_THREADS, 0, stream>>>(
                d_in,
                d_block_reductions,
                num_items,
                even_share,
                queue,
                reduction_op);

            // Check for failure to launch
            if (CubDebug(error = cudaPeekAtLastError())) break;

            // Sync the stream if specified to flush runtime errors
            if (debug_synchronous && (CubDebug(error = SyncStream(stream)))) break;

            // Log single_reduce_sweep_kernel configuration
            if (debug_synchronous) _CubLog("Invoking DeviceReduceSingleTileKernel<<<1, %d, 0, %lld>>>(), %d items per thread\n",
                ActivePolicyT::SingleTilePolicy::BLOCK_THREADS,
                (long long) stream,
                ActivePolicyT::SingleTilePolicy::ITEMS_PER_THREAD);

            // Invoke DeviceReduceSingleTileKernel
            single_tile_kernel<<<1, ActivePolicyT::SingleTilePolicy::BLOCK_THREADS, 0, stream>>>(
                d_block_reductions,
                d_out,
                reduce_grid_size,
                reduction_op,
                init);

            // Check for failure to launch
            if (CubDebug(error = cudaPeekAtLastError())) break;

            // Sync the stream if specified to flush runtime errors
            if (debug_synchronous && (CubDebug(error = SyncStream(stream)))) break;
        }
        while (0);

        return error;

#endif // CUB_RUNTIME_ENABLED

    }


    //------------------------------------------------------------------------------
    // Chained policy invocation
    //------------------------------------------------------------------------------

    /// Invocation
    template <typename ActivePolicyT>
    CUB_RUNTIME_FUNCTION __forceinline__
    cudaError_t Invoke()
    {
        typedef typename ActivePolicyT::SingleTilePolicy    SingleTilePolicyT;
        typedef typename DispatchReduce::MaxPolicy          MaxPolicyT;

        // Force kernel code-generation in all compiler passes
        if (num_items <= (SingleTilePolicyT::BLOCK_THREADS * SingleTilePolicyT::ITEMS_PER_THREAD))
        {
            // Small, single tile size
            return InvokeSingleTile<ActivePolicyT>(
                DeviceReduceSingleTileKernel<MaxPolicyT, InputIteratorT, OutputIteratorT, OffsetT_NV, ReductionOpT, T>);
        }
        else
        {
            // Regular size
            return InvokePasses<ActivePolicyT>(
                DeviceReduceKernel<typename DispatchReduce::MaxPolicy, InputIteratorT, T*, OffsetT_NV, ReductionOpT>,
                DeviceReduceSingleTileKernel<MaxPolicyT, T*, OutputIteratorT, OffsetT_NV, ReductionOpT, T>,
                FillAndResetDrainKernel<OffsetT_NV>);
        }
    }


    //------------------------------------------------------------------------------
    // Dispatch entrypoints
    //------------------------------------------------------------------------------

    /**
     * Internal dispatch routine for computing a device-wide reduction
     */
    CUB_RUNTIME_FUNCTION __forceinline__
    static cudaError_t Dispatch(
        void            *d_temp_storage,                    ///< [in] %Device-accessible allocation of temporary storage.  When NULL, the required allocation size is written to \p temp_storage_bytes and no work is done.
        size_t          &temp_storage_bytes,                ///< [in,out] Reference to size in bytes of \p d_temp_storage allocation
        InputIteratorT  d_in,                               ///< [in] Pointer to the input sequence of data items
        OutputIteratorT d_out,                              ///< [out] Pointer to the output aggregate
        OffsetT_NV         num_items,                          ///< [in] Total number of input items (i.e., length of \p d_in)
        ReductionOpT    reduction_op,                       ///< [in] Binary reduction functor 
        T               init,                               ///< [in] The initial value of the reduction
        cudaStream_t    stream,                             ///< [in] <b>[optional]</b> CUDA stream to launch kernels within.  Default is stream<sub>0</sub>.
        bool            debug_synchronous)                  ///< [in] <b>[optional]</b> Whether or not to synchronize the stream after every kernel launch to check for errors.  Also causes launch configurations to be printed to the console.  Default is \p false.
    {
        typedef typename DispatchReduce::MaxPolicy MaxPolicyT;

        cudaError error = cudaSuccess;
        do
        {
            // Get PTX version
            int ptx_version;
            if (CubDebug(error = PtxVersion(ptx_version))) break;

            // Create dispatch functor
            DispatchReduce dispatch(
                d_temp_storage, temp_storage_bytes,
                d_in, d_out, num_items, reduction_op, init,
                stream, debug_synchronous, ptx_version);

            // Dispatch to chained policy
            if (CubDebug(error = MaxPolicyT::Invoke(ptx_version, dispatch))) break;
        }
        while (0);

        return error;
    }
};



/******************************************************************************
 * Segmented dispatch
 ******************************************************************************/

/**
 * Utility class for dispatching the appropriately-tuned kernels for device-wide reduction
 */
template <
    typename InputIteratorT,    ///< Random-access input iterator type for reading input items \iterator
    typename OutputIteratorT,   ///< Output iterator type for recording the reduced aggregate \iterator
    typename OffsetT_NV,           ///< Signed integer type for global offsets
    typename ReductionOpT>      ///< Binary reduction functor type having member <tt>T operator()(const T &a, const T &b)</tt> 
struct DispatchSegmentedReduce :
    DeviceReducePolicy<
        typename std::iterator_traits<InputIteratorT>::value_type,
        OffsetT_NV,
        ReductionOpT>
{
    //------------------------------------------------------------------------------
    // Constants
    //------------------------------------------------------------------------------

    // Data type of input iterator
    typedef typename std::iterator_traits<InputIteratorT>::value_type T;


    //------------------------------------------------------------------------------
    // Problem state
    //------------------------------------------------------------------------------

    void                *d_temp_storage;        ///< [in] %Device-accessible allocation of temporary storage.  When NULL, the required allocation size is written to \p temp_storage_bytes and no work is done.
    size_t              &temp_storage_bytes;    ///< [in,out] Reference to size in bytes of \p d_temp_storage allocation
    InputIteratorT      d_in;                   ///< [in] Pointer to the input sequence of data items
    OutputIteratorT     d_out;                  ///< [out] Pointer to the output aggregate
    OffsetT_NV             num_segments;           ///< [in] The number of segments that comprise the sorting data
    OffsetT_NV             *d_begin_offsets;       ///< [in] %Device-accessible pointer to the sequence of beginning offsets of length \p num_segments, such that <tt>d_begin_offsets[i]</tt> is the first element of the <em>i</em><sup>th</sup> data segment in <tt>d_keys_*</tt> and <tt>d_values_*</tt>
    OffsetT_NV             *d_end_offsets;         ///< [in] %Device-accessible pointer to the sequence of ending offsets of length \p num_segments, such that <tt>d_end_offsets[i]-1</tt> is the last element of the <em>i</em><sup>th</sup> data segment in <tt>d_keys_*</tt> and <tt>d_values_*</tt>.  If <tt>d_end_offsets[i]-1</tt> <= <tt>d_begin_offsets[i]</tt>, the <em>i</em><sup>th</sup> is considered empty.
    ReductionOpT        reduction_op;           ///< [in] Binary reduction functor 
    T                   init;                   ///< [in] The initial value of the reduction
    cudaStream_t        stream;                 ///< [in] CUDA stream to launch kernels within.  Default is stream<sub>0</sub>.
    bool                debug_synchronous;      ///< [in] Whether or not to synchronize the stream after every kernel launch to check for errors.  Also causes launch configurations to be printed to the console.  Default is \p false.
    int                 ptx_version;            ///< [in] PTX version

    //------------------------------------------------------------------------------
    // Constructor
    //------------------------------------------------------------------------------

    /// Constructor
    CUB_RUNTIME_FUNCTION __forceinline__
    DispatchSegmentedReduce(
        void*                   d_temp_storage,
        size_t                  &temp_storage_bytes,
        InputIteratorT          d_in,
        OutputIteratorT         d_out,
        OffsetT_NV                 num_segments,
        OffsetT_NV                 *d_begin_offsets,
        OffsetT_NV                 *d_end_offsets,
        ReductionOpT            reduction_op,
        T                       init,
        cudaStream_t            stream,
        bool                    debug_synchronous,
        int                     ptx_version)
    :
        d_temp_storage(d_temp_storage),
        temp_storage_bytes(temp_storage_bytes),
        d_in(d_in),
        d_out(d_out),
        num_segments(num_segments),
        d_begin_offsets(d_begin_offsets),
        d_end_offsets(d_end_offsets),
        reduction_op(reduction_op),
        init(init),
        stream(stream),
        debug_synchronous(debug_synchronous),
        ptx_version(ptx_version)
    {}



    //------------------------------------------------------------------------------
    // Chained policy invocation
    //------------------------------------------------------------------------------

    /// Invocation
    template <
        typename                        ActivePolicyT,                  ///< Umbrella policy active for the target device
        typename                        DeviceSegmentedReduceKernelT>   ///< Function type of cub::DeviceSegmentedReduceKernel
    CUB_RUNTIME_FUNCTION __forceinline__
    cudaError_t InvokePasses(
        DeviceSegmentedReduceKernelT    segmented_reduce_kernel)        ///< [in] Kernel function pointer to parameterization of cub::DeviceSegmentedReduceKernel
    {
#ifndef CUB_RUNTIME_ENABLED

        // Kernel launch not supported from this device
        return CubDebug(cudaErrorNotSupported );
#else
        cudaError error = cudaSuccess;
        do
        {
            // Return if the caller is simply requesting the size of the storage allocation
            if (d_temp_storage == NULL)
            {
                temp_storage_bytes = 1;
                return cudaSuccess;
            }

            // Init kernel configuration
            KernelConfig segmented_reduce_config;
            if (CubDebug(error = segmented_reduce_config.Init<typename ActivePolicyT::SegmentedReducePolicy>(segmented_reduce_kernel))) break;

            // Log device_reduce_sweep_kernel configuration
            if (debug_synchronous) _CubLog("Invoking SegmentedDeviceReduceKernel<<<%d, %d, 0, %lld>>>(), %d items per thread, %d SM occupancy\n",
                num_segments,
                ActivePolicyT::SegmentedReducePolicy::BLOCK_THREADS,
                (long long) stream,
                ActivePolicyT::SegmentedReducePolicy::ITEMS_PER_THREAD,
                segmented_reduce_config.sm_occupancy);

            // Invoke DeviceReduceKernel
            segmented_reduce_kernel<<<num_segments, ActivePolicyT::SegmentedReducePolicy::BLOCK_THREADS, 0, stream>>>(
                d_in,
                d_out,
                d_begin_offsets,
                d_end_offsets,
                num_segments,
                reduction_op,
                init);

            // Check for failure to launch
            if (CubDebug(error = cudaPeekAtLastError())) break;

            // Sync the stream if specified to flush runtime errors
            if (debug_synchronous && (CubDebug(error = SyncStream(stream)))) break;
        }
        while (0);

        return error;

#endif // CUB_RUNTIME_ENABLED

    }


    /// Invocation
    template <typename ActivePolicyT>
    CUB_RUNTIME_FUNCTION __forceinline__
    cudaError_t Invoke()
    {
        typedef typename DispatchSegmentedReduce::MaxPolicy MaxPolicyT;

        // Force kernel code-generation in all compiler passes
        return InvokePasses<ActivePolicyT>(
            DeviceSegmentedReduceKernel<MaxPolicyT, InputIteratorT, OutputIteratorT, OffsetT_NV, ReductionOpT, T>);
    }


    //------------------------------------------------------------------------------
    // Dispatch entrypoints
    //------------------------------------------------------------------------------

    /**
     * Internal dispatch routine for computing a device-wide reduction
     */
    CUB_RUNTIME_FUNCTION __forceinline__
    static cudaError_t Dispatch(
        void            *d_temp_storage,                    ///< [in] %Device-accessible allocation of temporary storage.  When NULL, the required allocation size is written to \p temp_storage_bytes and no work is done.
        size_t          &temp_storage_bytes,                ///< [in,out] Reference to size in bytes of \p d_temp_storage allocation
        InputIteratorT  d_in,                               ///< [in] Pointer to the input sequence of data items
        OutputIteratorT d_out,                              ///< [out] Pointer to the output aggregate
        int             num_segments,                       ///< [in] The number of segments that comprise the sorting data
        int             *d_begin_offsets,                   ///< [in] %Device-accessible pointer to the sequence of beginning offsets of length \p num_segments, such that <tt>d_begin_offsets[i]</tt> is the first element of the <em>i</em><sup>th</sup> data segment in <tt>d_keys_*</tt> and <tt>d_values_*</tt>
        int             *d_end_offsets,                     ///< [in] %Device-accessible pointer to the sequence of ending offsets of length \p num_segments, such that <tt>d_end_offsets[i]-1</tt> is the last element of the <em>i</em><sup>th</sup> data segment in <tt>d_keys_*</tt> and <tt>d_values_*</tt>.  If <tt>d_end_offsets[i]-1</tt> <= <tt>d_begin_offsets[i]</tt>, the <em>i</em><sup>th</sup> is considered empty.
        ReductionOpT    reduction_op,                       ///< [in] Binary reduction functor 
        T               init,                               ///< [in] The initial value of the reduction
        cudaStream_t    stream,                             ///< [in] <b>[optional]</b> CUDA stream to launch kernels within.  Default is stream<sub>0</sub>.
        bool            debug_synchronous)                  ///< [in] <b>[optional]</b> Whether or not to synchronize the stream after every kernel launch to check for errors.  Also causes launch configurations to be printed to the console.  Default is \p false.
    {
        typedef typename DispatchSegmentedReduce::MaxPolicy MaxPolicyT;

        if (num_segments <= 0)
            return cudaSuccess;

        cudaError error = cudaSuccess;
        do
        {
            // Get PTX version
            int ptx_version;
            if (CubDebug(error = PtxVersion(ptx_version))) break;

            // Create dispatch functor
            DispatchSegmentedReduce dispatch(
                d_temp_storage, temp_storage_bytes,
                d_in, d_out,
                num_segments, d_begin_offsets, d_end_offsets,
                reduction_op, init,
                stream, debug_synchronous, ptx_version);

            // Dispatch to chained policy
            if (CubDebug(error = MaxPolicyT::Invoke(ptx_version, dispatch))) break;
        }
        while (0);

        return error;
    }
};



}               // CUB namespace
CUB_NS_POSTFIX  // Optional outer namespace(s)


