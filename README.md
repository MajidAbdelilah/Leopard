# Leopard Library

A C++ library for parallel vector operations.

## Overview

Leopard is a C++ library that provides a parallel vector implementation (`Lp_parallel_vector`) for performing vector operations in parallel using multiple threads. It leverages C++ threading capabilities to distribute work across available CPU cores.

## Features

- Parallel vector operations (addition, subtraction, multiplication, division, etc.)
- Thread-safe implementation
- Automatic thread management
- Fill methods for initializing vectors
- Conditional parallel execution with `Lp_if_parallel`
- Parallel quicksort implementation with `Lp_sort`

## Parallel Quicksort

The library includes a parallel quicksort implementation (`Lp_sort`) that efficiently sorts elements in a `Lp_parallel_vector` using multiple threads. Key features include:

### Usage Example

```cpp
// Create a parallel vector
Lp_parallel_vector<int> vec(1000);

// Fill it with random values
vec.fill([](int& val) { 
    val = std::rand() % 1000; 
    return val;
});

// Sort in ascending order
Lp_sort(vec, std::function<bool(int, int)>([](int a, int b) { return a < b; }));
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
    Lp_parallel_vector<int> vec(1000);
    
    // Fill the vector with a value
    vec.fill(42);
    
    // Apply a function to each element
    vec.fill([](int& val) { return val * 2; });
    
    // Perform parallel operations
    Lp_parallel_vector<int> vec2(1000);
    vec2.fill(10);
    
    // Add two vectors
    Lp_parallel_vector<int> result = vec + vec2;
    
    // Conditional parallel execution
    vec[0] = 1; // Set condition
    Lp_if_parallel(vec, []() {
        // This will be executed in parallel if condition is met
        std::cout << "Executing in parallel!" << std::endl;
    });
    
    return 0;
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
