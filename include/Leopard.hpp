/** 
 * @file Leopard.hpp
 * @author Abdelilah
 * @brief 
 * @version 0.1
 * @date 2025-03-19
 * 
 * @copyright Copyright (c) 2025
 * 

    اذن نحن لدينا تيبس و نريد جمعنا في كونتاينر لنقوم بعمل حوسبة باراليل لهذه التيبس 
    اذن
    ما علينا فعله هو جمعها في كونتاينر فيكتور و عمله كتيب خاص بنا 
    template <typename T>
    using parallel_vector = std::vector<T>;
    ثم علينا عمل اوفرلود للعمليات الاساسية
    parallel_vector<int> vec1 = {1, 2, 3, 4, 5};
    parallel_vector<int> vec2 = {6, 7, 8, 9, 10};
    parallel_vector<int> vec3 = vec1 + vec2;
    و ما شابه
    و من اجل التيبس الغير اصلية يجب ان يضع المستعمل اوفرلودس خاصة بها.
 */

#include <vector>
#include <thread>
#include <deque>
#include <chrono>
#include <iostream>
#include <memory>
#include <typeindex>
#include <typeinfo>


enum command_type {
    ADD,
    SUB,
    MUL,
    DIV,
    LT,
    GT,
    LE,
    GE,
    EQ,
    NE,
    AND,
    OR,
    NOT,
    BIT_AND,
    BIT_OR,
    BIT_XOR,
    BIT_NOT,
    LEFT_SHIFT,
    RIGHT_SHIFT
};

// Forward declarations
class Leopard;

// Base command class for type erasure with type information
struct command_base {
    command_type type;
    std::type_index type_info;
    
    command_base(command_type t, const std::type_info& ti) 
        : type(t), type_info(ti) {}
    virtual ~command_base() = default;
};

// Templated command derived class
template <typename T>
struct command : public command_base {
    void* left;
    void* right;
    
    command(command_type t, void* l, void* r) 
        : command_base(t, typeid(T)), left(l), right(r) {}
};

// Forward declare the parallel_vector template
template <typename T>
class parallel_vector;

// Declare the Leopard class first with the necessary methods for parallel_vector
class Leopard {
public:
    // Template function to add commands with automatic type deduction
    template <typename T>
    void add_command(command_type type, void* left, void* right) {
        auto cmd = std::make_shared<command<T>>(type, left, right);
        commands.push_back(cmd);
    }
    
    Leopard() {
        thread_count = std::thread::hardware_concurrency();
        for (int i = 0; i < thread_count; i++) {
            threads[i] = std::thread([this, i]() {
                std::cout << "Thread " << i << " started" << std::endl;
                while (true) {
                    std::shared_ptr<command_base> cmd;
                    if (commands.empty()) {
                        std::this_thread::sleep_for(std::chrono::milliseconds(100));
                        continue;
                    }
                    cmd = commands.front();
                    commands.pop_front();
                    
                    // Use type_id to determine the command type at runtime
                    process_command_runtime(cmd);
                }
            });
        }
    }
    
    // Function to determine the type at runtime and process the command
    void process_command_runtime(std::shared_ptr<command_base> cmd) {
        // Use the stored type_info to determine the actual type
        if (cmd->type_info == typeid(int)) {
            process_command<int>(std::static_pointer_cast<command<int>>(cmd));
        }
        else if (cmd->type_info == typeid(float)) {
            process_command<float>(std::static_pointer_cast<command<float>>(cmd));
        }
        else if (cmd->type_info == typeid(double)) {
            process_command<double>(std::static_pointer_cast<command<double>>(cmd));
        }
        // Add more types as needed
    }
    
    // Function to process the next command in the queue
    template <typename T>
    void process_command(std::shared_ptr<command<T>> typed_cmd) {
        auto left_vec = static_cast<parallel_vector<T>*>(typed_cmd->left);
        auto right_vec = static_cast<parallel_vector<T>*>(typed_cmd->right);
        
        switch (typed_cmd->type) {
            case ADD:
                // Process addition for type T
                for (size_t i = 0; i < left_vec->size() && i < right_vec->size(); ++i) {
                    (*left_vec)[i] = (*left_vec)[i] + (*right_vec)[i];
                }
                break;
            case SUB:
                // Process subtraction for type T
                for (size_t i = 0; i < left_vec->size() && i < right_vec->size(); ++i) {
                    (*left_vec)[i] = (*left_vec)[i] - (*right_vec)[i];
                }
                break;
            case MUL:
                // Process multiplication for type T
                for (size_t i = 0; i < left_vec->size() && i < right_vec->size(); ++i) {
                    (*left_vec)[i] = (*left_vec)[i] * (*right_vec)[i];
                }
                break;
            case DIV:
                // Process division for type T
                for (size_t i = 0; i < left_vec->size() && i < right_vec->size(); ++i) {
                    if ((*right_vec)[i] != 0) { // Prevent division by zero
                        (*left_vec)[i] = (*left_vec)[i] / (*right_vec)[i];
                    }
                }
                break;
            default:
                break;
        }
    }
    
private:
    std::thread threads[512];
    int thread_count = 0;
    std::deque<std::shared_ptr<command_base>> commands;
};

// Now define the parallel_vector template with the Leopard class available
template <typename T>
class parallel_vector : public std::vector<T> {
public:
    parallel_vector(Leopard& leopard): std::vector<T>(), leopard_ref(&leopard) {}
    parallel_vector(Leopard& leopard, std::initializer_list<T> list) : std::vector<T>(list), leopard_ref(&leopard) {}
    
    // Operator overloads that automatically determine the type
    parallel_vector<T> operator+(const parallel_vector<T>& other) const {
        // Create a result vector
        parallel_vector<T> result(*leopard_ref);
        // Add the command to the Leopard's queue with type information
        leopard_ref->add_command<T>(ADD, const_cast<parallel_vector<T>*>(this), const_cast<parallel_vector<T>*>(&other));
        // Return the result vector (will be filled by Leopard when processing)
        return result;
    }
    
    parallel_vector<T> operator-(const parallel_vector<T>& other) const {
        parallel_vector<T> result(*leopard_ref);
        leopard_ref->add_command<T>(SUB, const_cast<parallel_vector<T>*>(this), const_cast<parallel_vector<T>*>(&other));
        return result;
    }
    
    parallel_vector<T> operator*(const parallel_vector<T>& other) const {
        parallel_vector<T> result(*leopard_ref);
        leopard_ref->add_command<T>(MUL, const_cast<parallel_vector<T>*>(this), const_cast<parallel_vector<T>*>(&other));
        return result;
    }
    
    parallel_vector<T> operator/(const parallel_vector<T>& other) const {
        parallel_vector<T> result(*leopard_ref);
        leopard_ref->add_command<T>(DIV, const_cast<parallel_vector<T>*>(this), const_cast<parallel_vector<T>*>(&other));
        return result;
    }
    
private:
    Leopard* leopard_ref;
};



