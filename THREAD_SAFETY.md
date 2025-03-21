# Thread Safety Improvements

## Overview of Changes

This document outlines the changes made to improve thread safety in the Leopard library, particularly in the `Lp_parallel_vector` class.

## Issues Fixed

### 1. Invalid Argument Error in Thread Joining

The primary issue was that the destructor and various operator functions were attempting to join threads that had not been started, resulting in a `std::system_error` with the message "Invalid argument".

### 2. Thread Lifecycle Management

The code now properly manages thread lifecycles by checking if threads are joinable before attempting to join them.

## Changes Made

### 1. Destructor Fix

```cpp
~Lp_parallel_vector() {
    for(size_t i = 0; i < num_thread; i++)
        if(threads[i].joinable()) {
            threads[i].join();
        }
}
```

### 2. Lp_if_parallel Function Fix

```cpp
for(size_t i = 0; i < std::thread::hardware_concurrency(); i++) {
    if(threads[i].joinable()) {
        threads[i].join();
    }
}
```

### 3. Operator Function Fixes

All operator functions (addition, subtraction, multiplication, etc.) were updated to check if threads are joinable before joining:

```cpp
for(size_t i = 0; i < this->num_thread; i++) {
    if(threads[i].joinable()) {
        threads[i].join();
    }
}
```

### 4. Type Consistency

- Changed the type of `num_thread` from `int` to `size_t` for consistency with the size of the vector
- Updated all loop indices to use `size_t` instead of `auto i = 0` to avoid signedness comparison warnings

## Testing

Comprehensive tests were added to verify the fixes:

1. **Basic Tests**: Testing constructor, destructor, and basic operations
2. **Joinable Fix Tests**: Specific tests for the joinable() fixes
3. **Stress Tests**: Creating and destroying many vectors to ensure stability

## Results

The changes successfully resolved the "Invalid argument" error and improved the overall thread safety and stability of the library. The code now properly manages thread lifecycles and avoids attempting to join threads that have not been started.

## Future Improvements

1. Consider using a thread pool for better performance
2. Add more error handling and reporting
3. Implement additional synchronization mechanisms for complex operations
