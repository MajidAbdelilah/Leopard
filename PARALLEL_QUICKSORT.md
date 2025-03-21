# Parallel Quicksort Implementation

This document describes the parallel quicksort implementation in the Leopard library.

## Overview

The `Lp_sort` function implements a parallel quicksort algorithm that efficiently sorts elements in a `Lp_parallel_vector` using multiple threads. The implementation uses a task-based parallelism approach with a thread pool to distribute the workload across available CPU cores.

## Implementation Details

### Task Queue Approach

Instead of directly spawning threads for each recursive call, the implementation uses a task queue approach:

1. The algorithm starts by creating a thread pool with a number of threads equal to the hardware concurrency.
2. A task queue is initialized with the initial sorting range (0 to size-1).
3. Worker threads pick tasks from the queue and process them.
4. Each task involves partitioning a range and adding the resulting sub-ranges back to the queue.
5. Small ranges (< 1000 elements) are sorted sequentially using `std::sort` for efficiency.

### Thread Synchronization

The implementation uses several synchronization mechanisms:

1. A mutex to protect access to the shared task queue.
2. An atomic counter to track the number of active tasks.
3. A condition variable to notify threads when new tasks are available or all tasks are completed.

### Partitioning

The partitioning algorithm:

1. Selects a pivot element (middle element of the range).
2. Rearranges elements so that elements less than the pivot are on the left, and elements greater than the pivot are on the right.
3. Returns the index of the pivot after partitioning.

### Optimizations

1. **Sequential Fallback**: Small partitions are sorted using `std::sort` to avoid the overhead of task creation and synchronization.
2. **Work Stealing**: Idle threads can steal tasks from the queue, ensuring efficient load balancing.
3. **Memory Efficiency**: The algorithm works on a copy of the original vector and only copies the sorted result back at the end.

## Performance Considerations

The parallel quicksort implementation offers significant performance improvements over sequential sorting for large vectors, especially on multi-core systems. However, for small vectors (< 10,000 elements), the overhead of thread creation and synchronization may outweigh the benefits of parallelism.

## Thread Safety

The implementation ensures thread safety by:

1. Using proper synchronization mechanisms (mutex, atomic variables, condition variables).
2. Only joining threads that are joinable to prevent "Invalid argument" errors.
3. Properly handling thread creation and destruction.

## Usage Example

```cpp
// Create a parallel vector
Lp_parallel_vector<int> vec(10000);

// Fill it with random values
vec.fill([](int& val) { 
    val = std::rand() % 10000; 
    return val;
});

// Sort in ascending order
Lp_sort(vec, std::function<bool(int, int)>([](int a, int b) { return a < b; }));

// Sort in descending order
Lp_sort(vec, std::function<bool(int, int)>([](int a, int b) { return a > b; }));
```

## Future Improvements

Potential improvements to the parallel quicksort implementation:

1. **Adaptive Parallelism**: Dynamically adjust the parallelism level based on the vector size and available hardware.
2. **Better Pivot Selection**: Implement more sophisticated pivot selection strategies (e.g., median-of-three).
3. **Hybrid Algorithm**: Combine quicksort with other sorting algorithms like insertion sort for small partitions.
4. **Memory Locality**: Improve cache utilization by considering memory access patterns.
