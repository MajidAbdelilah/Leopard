# Leopard Library

A C++ library for parallel vector operations.

## Overview

Leopard is a C++ library that provides a parallel vector implementation (`Lp_parallel_vector`) for performing vector operations in parallel using multiple threads. It leverages C++ threading capabilities to distribute work across available CPU cores.

## Features

- Parallel vector operations (addition, subtraction, multiplication, division, etc.)
- Thread-safe implementation
- Automatic thread management
- Fill methods for initializing vectors
- Enhanced comparison operators returning boolean vectors
- Improved conditional parallel execution with `Lp_if_parallel`
- Parallel quicksort implementation with `Lp_sort`

## Parallel Quicksort

The library includes a parallel quicksort implementation (`Lp_sort`) that efficiently sorts elements in a `Lp_parallel_vector` using multiple threads. Key features include:

### Usage Example

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

## Enhanced Comparison Operators

The library now provides enhanced comparison operators that return boolean vectors (`Lp_parallel_vector<bool>`) instead of vectors of the original type. This allows for more intuitive and efficient conditional operations.

Key improvements:

1. All comparison operators (`==`, `!=`, `<`, `>`, `<=`, `>=`) now return `Lp_parallel_vector<bool>`
2. Added scalar comparison operators to compare each element with a single value
3. Results can be directly used with `Lp_if_parallel` for conditional execution

### Usage Example

```cpp
// Create a parallel vector
Lp_parallel_vector<int> vec(1000);
vec.fill(42);

// Compare with scalar value (returns a boolean vector)
Lp_parallel_vector<bool> result = vec == 42;

// Use with Lp_if_parallel
Lp_if_parallel(result, [](size_t index) {
    std::cout << "Element at index " << index << " equals 42" << std::endl;
});

// Combine operations
Lp_if_parallel(vec > 40 && vec < 50, [](size_t index) {
    std::cout << "Element at index " << index << " is between 40 and 50" << std::endl;
});
```

## Improved Lp_if_parallel

The `Lp_if_parallel` function has been enhanced to provide more flexibility and information during parallel execution:

1. Now accepts a function that takes the current index as a parameter (`std::function<void(size_t)>`)
2. Passes by value instead of reference for better compatibility with temporary vectors
3. Works seamlessly with the new boolean vectors from comparison operators

### Usage Example

```cpp
Lp_parallel_vector<int> vec(1000);
vec.fill(42);

// Execute function only for elements that are equal to 42
Lp_if_parallel(vec == 42, [&vec](size_t index) {
    std::cout << "Element at index " << index << ": " << vec[index] << std::endl;
});

// Execute function only for elements that are not equal to 0
Lp_if_parallel(!vec, [](size_t index) {
    std::cout << "Non-zero element found at index " << index << std::endl;
});
```

## Thread Safety

The library ensures thread safety by:

1. Only joining threads that are joinable
2. Properly managing thread lifecycles
3. Using appropriate thread synchronization

## Usage Example

```cpp
#include "include/Leopard.hpp"
#include <iostream>

int main() {
   // Create a parallel vector
    Lp_parallel_vector<int> tvec(1000);
    
    // Fill the vector with a value
    tvec.fill(42);
    
    // Apply a function to each element
    tvec.fill([](int& val, size_t index) { (void)index; return val * 2 ; });
    
    // Perform parallel operations
    Lp_parallel_vector<int> tvec2(1000);
    tvec2.fill(10);
    
    // Add two vectors
    Lp_parallel_vector<int> result = tvec + tvec2;
    
    // Conditional parallel execution
    tvec[0] = 0; // Set condition
    tvec[15] = 0; // Set condition
    Lp_if_parallel(tvec == 0, [tvec](size_t index) {
        // This will be executed in parallel if condition is met
        std::cout << "Executing in parallel!" << std::endl;
        std::cout << tvec[index] << " at index " <<index << std::endl;  
    });
}
```

## Building

```bash
mkdir build
cd build
cmake ..
make
```

## Testing

The library includes comprehensive tests to ensure thread safety and correct functionality:

1. Basic constructor and destructor tests
2. Fill method tests
3. Thread safety tests
4. Joinable thread tests
5. Stress tests

## License

[Specify your license here]
