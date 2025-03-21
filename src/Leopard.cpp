#include "../include/Leopard.hpp"
#include <iostream>
#include <chrono>

// Function to test thread safety by creating and destroying many vectors
void stress_test_thread_safety(int iterations) {
    std::cout << "Running thread safety stress test with " << iterations << " iterations..." << std::endl;
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < iterations; i++) {
        // Create a vector with a small size
        Lp_parallel_vector<int> test_vec(100);
        
        // Fill it with some values
        test_vec.fill(i);
        
        // Apply a function to it
        test_vec.fill([](int& val) { return val * 2; });
        
        // Test Lp_if_parallel
        if (i % 10 == 0) { // Only do this occasionally to save time
            test_vec[0] = 1; // Set first element to true
            Lp_if_parallel(test_vec, []() { /* Do nothing */ });
        }
        
        // Vector will be destroyed at end of loop iteration
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    std::cout << "Stress test completed in " << elapsed.count() << " seconds" << std::endl;
}

// Function to specifically test the joinable() fixes
void test_joinable_fixes() {
    std::cout << "\nTesting joinable() fixes..." << std::endl;
    
    // Test 1: Create a vector but don't start any threads
    std::cout << "Test 1: Creating vector without starting threads..." << std::endl;
    {
        Lp_parallel_vector<int> vec(10);
        // Destructor will be called here, should not cause issues
    }
    std::cout << "Test 1 passed!" << std::endl;
    
    // Test 2: Create a vector, start threads with fill, then destroy
    std::cout << "Test 2: Creating vector, filling it, then destroying..." << std::endl;
    {
        Lp_parallel_vector<int> vec(10);
        vec.fill(42);
        // Destructor will be called here, should join threads properly
    }
    std::cout << "Test 2 passed!" << std::endl;
    
    // Test 3: Create a vector, start threads with operator, then destroy
    std::cout << "Test 3: Creating vector, using operator, then destroying..." << std::endl;
    {
        Lp_parallel_vector<int> vec1(10);
        Lp_parallel_vector<int> vec2(10);
        vec1.fill(1);
        vec2.fill(2);
        Lp_parallel_vector<int> vec3 = vec1 + vec2;
        // Destructor will be called here for all vectors
    }
    std::cout << "Test 3 passed!" << std::endl;
    
    // Test 4: Create a vector, use Lp_if_parallel, then destroy
    std::cout << "Test 4: Creating vector, using Lp_if_parallel, then destroying..." << std::endl;
    {
        Lp_parallel_vector<int> vec(10);
        vec[0] = 1; // Set first element to true
        Lp_if_parallel(vec, []() { /* Do nothing */ });
        // Destructor will be called here
    }
    std::cout << "Test 4 passed!" << std::endl;
    
    std::cout << "All joinable() fix tests passed!" << std::endl;
}

int main()
{
    // Test basic constructor and destructor
    std::cout << "Testing basic constructor and destructor..." << std::endl;
    Lp_parallel_vector<int> vec(100000);
    
    // Test fill method
    std::cout << "Testing fill method..." << std::endl;
    vec.fill(42);
    std::cout << "First element: " << vec[0] << std::endl;
    std::cout << "Last element: " << vec[vec.size()-1] << std::endl;
    
    // Test fill with function
    std::cout << "Testing fill with function..." << std::endl;
    vec.fill([](int& val) { return val * 2; });
    std::cout << "First element after function: " << vec[0] << std::endl;
    std::cout << "Last element after function: " << vec[vec.size()-1] << std::endl;
    
    // Test Lp_if_parallel with a smaller vector
    std::cout << "Testing Lp_if_parallel..." << std::endl;
    Lp_parallel_vector<int> small_vec(10);
    small_vec[0] = 1; // Set first element to true
    Lp_if_parallel(small_vec, []() { std::cout << "Hello from parallel function!" << std::endl; });
    
    // Run specific test for joinable fixes
    test_joinable_fixes();
    
    // Run stress test for thread safety
    stress_test_thread_safety(1000);
    
    std::cout << "All tests completed successfully!" << std::endl;
    return 0;
}