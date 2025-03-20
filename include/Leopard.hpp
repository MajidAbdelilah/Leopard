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

class Leopard;

template <typename T>
class parallel_vector : public std::vector<T> {
    public:
        parallel_vector(Leopard& leopard): std::vector<T>() {}
        parallel_vector(Leopard& leopard, std::initializer_list<T> list) : std::vector<T>(list) {}
        
        
    private:
};




struct command {
    command_type type;
    // the left and right are parallel_vector<T> but we don't know the type
    void* left;
    void* right;
};

class Leopard {
    public:
        Leopard() {
            thread_count = std::thread::hardware_concurrency();
            for (int i = 0; i < thread_count; i++) {
                threads[i] = std::thread([this, i]() {
                    std::cout << "Thread " << i << " started" << std::endl;
                    while (true) {
                        command cmd;
                        if (commands.empty()) {
                            std::this_thread::sleep_for(std::chrono::milliseconds(100));
                            continue;
                        }
                        cmd = commands.front();
                        commands.pop_front();
                        switch (cmd.type) {
                            case ADD:
                                // add
                                break;
                            case SUB:
                                // sub
                                break;
                            case MUL:
                                // mul
                                break;
                            case DIV:
                                // div
                                break;
                            case LT:
                                // lt
                                break;
                            case GT:
                                // gt
                                break;
                            case LE:
                                // le
                                break;
                            case GE:
                                // ge
                                break;
                            case EQ:
                                // eq
                                break;
                            case NE:
                                // ne
                                break;
                            case AND:
                                // and
                                break;
                            case OR:
                                // or
                                break;
                            case NOT:
                                // not
                                break;
                            case BIT_AND:
                                // bit_and
                                break;
                            case BIT_OR:
                                // bit_or
                                break;
                            case BIT_XOR:
                                // bit_xor
                                break;
                            case BIT_NOT:
                                // bit_not
                                break;
                            case LEFT_SHIFT:
                                // left_shift
                                break;
                            case RIGHT_SHIFT:
                                // right_shift
                                break;
                        }
                    }
                });
            }
        }
        ~Leopard() {
            for (int i = 0; i < thread_count; i++) {
                threads[i].join();
            }
        }
    private:
        std::thread threads[512];
        int thread_count = 0;
        std::deque<command> commands;
};