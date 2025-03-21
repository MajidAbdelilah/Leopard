#pragma once

#include <cstddef>
#include <thread>
#include <vector>
#include <functional>
#include <mutex>
#include <algorithm>
#include <atomic>
#include <condition_variable>
#include <iostream>

template<typename T>
class Lp_parallel_vector: public std::vector<T>
{
public:
    Lp_parallel_vector(): std::vector<T>() {
        num_thread = std::thread::hardware_concurrency();
    };
    ~Lp_parallel_vector() {
        // Only join threads that are joinable (have been started)
        for(size_t i = 0; i < num_thread; i++) {
            if(threads[i].joinable()) {
                threads[i].join();
            }
        }
    };
    // criticall part of the class for sycl compatibilty
    void assign(size_t count, const T& value) {
        this->clear();
        this->resize(count, value);
    }

    template< class InputIt >
    void assign( InputIt first, InputIt last ) {
        this->clear();
        this->insert(this->begin(), first, last);
    }

    void assign( std::initializer_list<T> ilist )
    {
        this->clear();
        this->insert(this->begin(), ilist.begin(), ilist.end());
    }

    T& at( size_t pos )
    {
        return std::vector<T>::at(pos);
    }

    const T& at( size_t pos ) const
    {
        return std::vector<T>::at(pos);
    }

    typename std::vector<T>::reference operator[]( size_t pos )
    {
        return std::vector<T>::operator[](pos);
    }

    typename std::vector<T>::const_reference operator[]( size_t pos ) const
    {
        return std::vector<T>::operator[](pos);
    }

    T& front()
    {
        return std::vector<T>::front();
    }

    const T& front() const
    {
        return std::vector<T>::front();
    }

    T& back()
    {
        return std::vector<T>::back();
    }

    const T& back() const
    {
        return std::vector<T>::back();
    }

    T* data()
    {
        return std::vector<T>::data();
    }

    const T* data() const
    {
        return std::vector<T>::data();
    }

    size_t size() const
    {
        return std::vector<T>::size();
    }

    typename std::vector<T>::iterator begin()
    {
        return std::vector<T>::begin();
    }

    typename std::vector<T>::const_iterator begin() const
   {
        return std::vector<T>::begin();
   }

    typename std::vector<T>::const_iterator cbegin() const
    {
        return std::vector<T>::cbegin();
    }

    typename std::vector<T>::iterator end()
    {
        return std::vector<T>::end();
    }

    typename std::vector<T>::const_iterator end() const
    {
        return std::vector<T>::end();
    }

    typename std::vector<T>::const_iterator cend() const
    {
        return std::vector<T>::cend();
    }

    typename std::vector<T>::reverse_iterator rbegin()
    {
        return std::vector<T>::rbegin();
    }

    typename std::vector<T>::const_reverse_iterator rbegin() const
    {
        return std::vector<T>::rbegin();
    }

    typename std::vector<T>::const_reverse_iterator crbegin() const
    {
        return std::vector<T>::crbegin();
    }

    typename std::vector<T>::reverse_iterator rend()
    {
        return std::vector<T>::rend();
    }

    typename std::vector<T>::const_reverse_iterator rend() const
    {
        return std::vector<T>::rend();
    }

    typename std::vector<T>::const_reverse_iterator crend() const
    {
        return std::vector<T>::crend();
    }

    bool empty() const
    {
        return std::vector<T>::empty();
    }

    typename std::vector<T>::size_type max_size() const
    {
        return std::vector<T>::max_size();
    }

    void reserve( typename std::vector<T>::size_type new_cap )
    {
        std::vector<T>::reserve(new_cap);
    }

    typename std::vector<T>::size_type capacity() const
    {
        return std::vector<T>::capacity();
    }

    void shrink_to_fit()
    {
        std::vector<T>::shrink_to_fit();
    }



    // criticall part of the class for sycl compatibilty

    Lp_parallel_vector(size_t num_elements) : std::vector<T>(num_elements) {
        num_thread = std::thread::hardware_concurrency();
    };
    
    Lp_parallel_vector(const Lp_parallel_vector& other) : std::vector<T>(other) {
        num_thread = other.num_thread;
        // Don't copy threads as they can't be copied
    };
    Lp_parallel_vector& operator=(const Lp_parallel_vector& other) {
        if(this != &other) {
            std::vector<T>::operator=(other);
            num_thread = other.num_thread;
            // Don't copy threads as they can't be copied
        }
        return *this;
    }
    Lp_parallel_vector(const std::vector<T>& other) : std::vector<T>(other) {
        num_thread = std::thread::hardware_concurrency();
    };
    
    Lp_parallel_vector& operator=(const std::vector<T>& other) {
        std::vector<T>::operator=(other);
        return *this;
    }
    Lp_parallel_vector(const std::initializer_list<T>& init) : std::vector<T>(init) {
        num_thread = std::thread::hardware_concurrency();
    };
    
    Lp_parallel_vector& operator=(const std::initializer_list<T>& init) {
        std::vector<T>::operator=(init);
        return *this;
    }

    void fill(T value) {
        for(size_t i = 0; i < this->num_thread; i++)
        {
            threads[i] = std::thread([this, value, i]() {
                for(size_t j = i; j < this->size(); j+= this->num_thread)
                    this->at(j) = value;
            });
        }
        for(size_t i = 0; i < this->num_thread; i++) {
            if(threads[i].joinable()) {
                threads[i].join();
            }
        }
    }

    void fill(T value, size_t size)
    {
        this->resize(size);
        fill(value);
    }

    void fill(std::function<T(T&, size_t)> func) {
        for(size_t i = 0; i < this->num_thread; i++)
        {
            threads[i] = std::thread([this, func, i]() {
                for(size_t j = i; j < this->size(); j+= this->num_thread)
                    this->at(j) = func(this->at(j), j);
            });
        }
        for(size_t i = 0; i < this->num_thread; i++) {
            if(threads[i].joinable()) {
                threads[i].join();
            }
        }
    }

    void fill(std::function<T(T&, size_t)> func, size_t size)
    {
        this->resize(size);
        fill(func);
    }


    Lp_parallel_vector<T> operator+(const Lp_parallel_vector<T>& other)
    {
        Lp_parallel_vector<T> result;
        auto min_size = std::min(this->size(), other.size());
        result.resize(min_size);
        for(size_t i = 0; i < this->num_thread; i++)
        {
            threads[i] = std::thread([this, &result, &other, min_size, i]() {
                for(size_t j = i; j < min_size; j+= this->num_thread)
                    result[j] = this->at(j) + other[j];
            });
        }
        for(size_t i = 0; i < this->num_thread; i++) {
            if(threads[i].joinable()) {
                threads[i].join();
            }
        }
        return result;        
    };
    Lp_parallel_vector<T> operator-(const Lp_parallel_vector<T>& other)
    {
        Lp_parallel_vector<T> result;
        auto min_size = std::min(this->size(), other.size());
        result.resize(min_size);
        for(size_t i = 0; i < this->num_thread; i++)
        {
            threads[i] = std::thread([this, &result, &other, min_size, i]() {
                for(size_t j = i; j < min_size; j+= this->num_thread)
                    result[j] = this->at(j) - other[j];
            });
        }
        for(size_t i = 0; i < this->num_thread; i++) {
            if(threads[i].joinable()) {
                threads[i].join();
            }
        }
        return result;        
    };
    Lp_parallel_vector<T> operator*(const Lp_parallel_vector<T>& other)
    {
        Lp_parallel_vector<T> result;
        auto min_size = std::min(this->size(), other.size());
        result.resize(min_size);
        for(size_t i = 0; i < this->num_thread; i++)
        {
            threads[i] = std::thread([this, &result, &other, min_size, i]() {
                for(size_t j = i; j < min_size; j+= this->num_thread)
                    result[j] = this->at(j) * other[j];
            });
        }
        for(size_t i = 0; i < this->num_thread; i++) {
            if(threads[i].joinable()) {
                threads[i].join();
            }
        }
        return result;        
    };
    Lp_parallel_vector<T> operator/(const Lp_parallel_vector<T>& other)
    {
        Lp_parallel_vector<T> result;
        auto min_size = std::min(this->size(), other.size());
        result.resize(min_size);
        for(size_t i = 0; i < this->num_thread; i++)
        {
            threads[i] = std::thread([this, &result, &other, min_size, i]() {
                for(size_t j = i; j < min_size; j+= this->num_thread)
                    result[j] = this->at(j) / other[j];
            });
        }
        for(size_t i = 0; i < this->num_thread; i++) {
            if(threads[i].joinable()) {
                threads[i].join();
            }
        }
        return result;        
    };
    Lp_parallel_vector<T> operator&&(const Lp_parallel_vector<T>& other)
    {
        Lp_parallel_vector<T> result;
        auto min_size = std::min(this->size(), other.size());
        result.resize(min_size);
        for(size_t i = 0; i < this->num_thread; i++)
        {
            threads[i] = std::thread([this, &result, &other, min_size, i]() {
                for(size_t j = i; j < min_size; j+= this->num_thread)
                    result[j] = this->at(j) && other[j];
            });
        }
        for(size_t i = 0; i < this->num_thread; i++) {
            if(threads[i].joinable()) {
                threads[i].join();
            }
        }
        return result;        
    };
    Lp_parallel_vector<T> operator||(const Lp_parallel_vector<T>& other)
    {
        Lp_parallel_vector<T> result;
        auto min_size = std::min(this->size(), other.size());
        result.resize(min_size);
        for(size_t i = 0; i < this->num_thread; i++)
        {
            threads[i] = std::thread([this, &result, &other, min_size, i]() {
                for(size_t j = i; j < min_size; j+= this->num_thread)
                    result[j] = this->at(j) || other[j];
            });
        }
        for(size_t i = 0; i < this->num_thread; i++) {
            if(threads[i].joinable()) {
                threads[i].join();
            }
        }
        return result;        
    };
    Lp_parallel_vector<T> operator!()
    {
        Lp_parallel_vector<T> result;
        result.resize(this->size());
        for(size_t i = 0; i < this->num_thread; i++)
        {
            threads[i] = std::thread([this, &result, i]() {
                for(size_t j = i; j < this->size(); j+= this->num_thread)
                    result[j] = !this->at(j);
            });
        }
        for(size_t i = 0; i < this->num_thread; i++) {
            if(threads[i].joinable()) {
                threads[i].join();
            }
        }
        return result;        
    };
    Lp_parallel_vector<bool> operator==(const Lp_parallel_vector<T>& other)
    {
        Lp_parallel_vector<bool> result;
        auto min_size = std::min(this->size(), other.size());
        result.resize(min_size);
        for(size_t i = 0; i < this->num_thread; i++)
        {
            threads[i] = std::thread([this, &result, &other, min_size, i]() {
                for(size_t j = i; j < min_size; j+= this->num_thread)
                    result[j] = this->at(j) == other[j];
            });
        }
        for(size_t i = 0; i < this->num_thread; i++) {
            if(threads[i].joinable()) {
                threads[i].join();
            }
        }
        return result;        
    };
    Lp_parallel_vector<bool> operator==(const T& other)
    {
        Lp_parallel_vector<bool> result;
        result.resize(this->size());
        for(size_t i = 0; i < this->num_thread; i++)
        {
            threads[i] = std::thread([this, &result, other, i]() {
                for(size_t j = i; j < this->size(); j+= this->num_thread)
                    result[j] = this->at(j) == other;
            });
        }
        for(size_t i = 0; i < this->num_thread; i++) {
            if(threads[i].joinable()) {
                threads[i].join();
            }
        }
        return result;        
    };
    Lp_parallel_vector<bool> operator!=(const Lp_parallel_vector<T>& other)
    {
        Lp_parallel_vector<bool> result;
        auto min_size = std::min(this->size(), other.size());
        result.resize(min_size);
        for(size_t i = 0; i < this->num_thread; i++)
        {
            threads[i] = std::thread([this, &result, &other, min_size, i]() {
                for(size_t j = i; j < min_size; j+= this->num_thread)
                    result[j] = this->at(j) != other[j];
            });
        }
        for(size_t i = 0; i < this->num_thread; i++) {
            if(threads[i].joinable()) {
                threads[i].join();
            }
        }
        return result;        
    };
    Lp_parallel_vector<bool> operator!=(const T& other)
    {
        Lp_parallel_vector<bool> result;
        result.resize(this->size());
        for(size_t i = 0; i < this->num_thread; i++)
        {
            threads[i] = std::thread([this, &result, other, i]() {
                for(size_t j = i; j < this->size(); j+= this->num_thread)
                    result[j] = this->at(j) != other;
            });
        }
        for(size_t i = 0; i < this->num_thread; i++) {
            if(threads[i].joinable()) {
                threads[i].join();
            }
        }
        return result;        
    };
    Lp_parallel_vector<bool> operator<(const Lp_parallel_vector<T>& other)
    {
        Lp_parallel_vector<bool> result;
        auto min_size = std::min(this->size(), other.size());
        result.resize(min_size);
        for(size_t i = 0; i < this->num_thread; i++)
        {
            threads[i] = std::thread([this, &result, &other, min_size, i]() {
                for(size_t j = i; j < min_size; j+= this->num_thread)
                    result[j] = this->at(j) < other[j];
            });
        }
        for(size_t i = 0; i < this->num_thread; i++) {
            if(threads[i].joinable()) {
                threads[i].join();
            }
        }
        return result;        
    };
    Lp_parallel_vector<bool> operator<(const T& other)
    {
        Lp_parallel_vector<bool> result;
        result.resize(this->size());
        for(size_t i = 0; i < this->num_thread; i++)
        {
            threads[i] = std::thread([this, &result, other, i]() {
                for(size_t j = i; j < this->size(); j+= this->num_thread)
                    result[j] = this->at(j) < other;
            });
        }
        for(size_t i = 0; i < this->num_thread; i++) {
            if(threads[i].joinable()) {
                threads[i].join();
            }
        }
        return result;        
    };
    Lp_parallel_vector<bool> operator>(const Lp_parallel_vector<T>& other)
    {
        Lp_parallel_vector<bool> result;
        auto min_size = std::min(this->size(), other.size());
        result.resize(min_size);
        for(size_t i = 0; i < this->num_thread; i++)
        {
            threads[i] = std::thread([this, &result, &other, min_size, i]() {
                for(size_t j = i; j < min_size; j+= this->num_thread)
                    result[j] = this->at(j) > other[j];
            });
        }
        for(size_t i = 0; i < this->num_thread; i++) {
            if(threads[i].joinable()) {
                threads[i].join();
            }
        }
        return result;        
    };
    Lp_parallel_vector<bool> operator>(const T& other)
    {
        Lp_parallel_vector<bool> result;
        result.resize(this->size());
        for(size_t i = 0; i < this->num_thread; i++)
        {
            threads[i] = std::thread([this, &result, other, i]() {
                for(size_t j = i; j < this->size(); j+= this->num_thread)
                    result[j] = this->at(j) > other;
            });
        }
        for(size_t i = 0; i < this->num_thread; i++) {
            if(threads[i].joinable()) {
                threads[i].join();
            }
        }
        return result;        
    };
    Lp_parallel_vector<bool> operator<=(const Lp_parallel_vector<T>& other)
    {
        Lp_parallel_vector<bool> result;
        auto min_size = std::min(this->size(), other.size());
        result.resize(min_size);
        for(size_t i = 0; i < this->num_thread; i++)
        {
            threads[i] = std::thread([this, &result, &other, min_size, i]() {
                for(size_t j = i; j < min_size; j+= this->num_thread)
                    result[j] = this->at(j) <= other[j];
            });
        }
        for(size_t i = 0; i < this->num_thread; i++) {
            if(threads[i].joinable()) {
                threads[i].join();
            }
        }
        return result;        
    };
    Lp_parallel_vector<bool> operator<=(const T& other)
    {
        Lp_parallel_vector<bool> result;
        result.resize(this->size());
        for(size_t i = 0; i < this->num_thread; i++)
        {
            threads[i] = std::thread([this, &result, other, i]() {
                for(size_t j = i; j < this->size(); j+= this->num_thread)
                    result[j] = this->at(j) <= other;
            });
        }
        for(size_t i = 0; i < this->num_thread; i++) {
            if(threads[i].joinable()) {
                threads[i].join();
            }
        }
        return result;        
    };
    Lp_parallel_vector<bool> operator>=(const Lp_parallel_vector<T>& other)
    {
        Lp_parallel_vector<bool> result;
        auto min_size = std::min(this->size(), other.size());
        result.resize(min_size);
        for(size_t i = 0; i < this->num_thread; i++)
        {
            threads[i] = std::thread([this, &result, &other, min_size, i]() {
                for(size_t j = i; j < min_size; j+= this->num_thread)
                    result[j] = this->at(j) >= other[j];
            });
        }
        for(size_t i = 0; i < this->num_thread; i++) {
            if(threads[i].joinable()) {
                threads[i].join();
            }
        }
        return result;        
    };
    Lp_parallel_vector<bool> operator>=(const T& other)
    {
        Lp_parallel_vector<bool> result;
        result.resize(this->size());
        for(size_t i = 0; i < this->num_thread; i++)
        {
            threads[i] = std::thread([this, &result, other, i]() {
                for(size_t j = i; j < this->size(); j+= this->num_thread)
                    result[j] = this->at(j) >= other;
            });
        }
        for(size_t i = 0; i < this->num_thread; i++) {
            if(threads[i].joinable()) {
                threads[i].join();
            }
        }
        return result;        
    };

    Lp_parallel_vector<T> operator&(Lp_parallel_vector<T>& other)
    {
        Lp_parallel_vector<T> result;
        auto min_size = std::min(this->size(), other.size());
        result.resize(min_size);
        for(size_t i = 0; i < this->num_thread; i++)
        {
            threads[i] = std::thread([this, &result, &other, min_size, i]() {
                for(size_t j = i; j < min_size; j+= this->num_thread)
                    result[j] = this->at(j) & other[j];
            });
        }
        for(size_t i = 0; i < this->num_thread; i++) {
            if(threads[i].joinable()) {
                threads[i].join();
            }
        }
        return result;        
    };
    Lp_parallel_vector<T> operator|(Lp_parallel_vector<T>& other)
    {
        Lp_parallel_vector<T> result;
        auto min_size = std::min(this->size(), other.size());
        result.resize(min_size);
        for(size_t i = 0; i < this->num_thread; i++)
        {
            threads[i] = std::thread([this, &result, &other, min_size, i]() {
                for(size_t j = i; j < min_size; j+= this->num_thread)
                    result[j] = this->at(j) | other[j];
            });
        }
        for(size_t i = 0; i < this->num_thread; i++) {
            if(threads[i].joinable()) {
                threads[i].join();
            }
        }
        return result;        
    };
    Lp_parallel_vector<T> operator^(Lp_parallel_vector<T>& other)
    {
        Lp_parallel_vector<T> result;
        auto min_size = std::min(this->size(), other.size());
        result.resize(min_size);
        for(size_t i = 0; i < this->num_thread; i++)
        {
            threads[i] = std::thread([this, &result, &other, min_size, i]() {
                for(size_t j = i; j < min_size; j+= this->num_thread)
                    result[j] = this->at(j) ^ other[j];
            });
        }
        for(size_t i = 0; i < this->num_thread; i++) {
            if(threads[i].joinable()) {
                threads[i].join();
            }
        }
        return result;        
    };
    Lp_parallel_vector<T> operator%(Lp_parallel_vector<T>& other)
    {
        Lp_parallel_vector<T> result;
        auto min_size = std::min(this->size(), other.size());
        result.resize(min_size);
        for(size_t i = 0; i < this->num_thread; i++)
        {
            threads[i] = std::thread([this, &result, &other, min_size, i]() {
                for(size_t j = i; j < min_size; j+= this->num_thread)
                    result[j] = this->at(j) % other[j];
            });
        }
        for(size_t i = 0; i < this->num_thread; i++) {
            if(threads[i].joinable()) {
                threads[i].join();
            }
        }
        return result;        
    };
    Lp_parallel_vector<T> operator<<(Lp_parallel_vector<T>& other)
    {
        Lp_parallel_vector<T> result;
        auto min_size = std::min(this->size(), other.size());
        result.resize(min_size);
        for(size_t i = 0; i < this->num_thread; i++)
        {
            threads[i] = std::thread([this, &result, &other, min_size, i]() {
                for(size_t j = i; j < min_size; j+= this->num_thread)
                    result[j] = this->at(j) << other[j];
            });
        }
        for(size_t i = 0; i < this->num_thread; i++) {
            if(threads[i].joinable()) {
                threads[i].join();
            }
        }
        return result;        
    };
    Lp_parallel_vector<T> operator>>(Lp_parallel_vector<T>& other)
    {
        Lp_parallel_vector<T> result;
        auto min_size = std::min(this->size(), other.size());
        result.resize(min_size);
        for(size_t i = 0; i < this->num_thread; i++)
        {
            threads[i] = std::thread([this, &result, &other, min_size, i]() {
                for(size_t j = i; j < min_size; j+= this->num_thread)
                    result[j] = this->at(j) >> other[j];
            });
        }
        for(size_t i = 0; i < this->num_thread; i++) {
            if(threads[i].joinable()) {
                threads[i].join();
            }
        }
        return result;        
    };
    Lp_parallel_vector<T> operator+(const T& other)
    {
        Lp_parallel_vector<T> result;
        result.resize(this->size());
        for(size_t i = 0; i < this->num_thread; i++)
        {
            threads[i] = std::thread([this, &result, other, i]() {
                for(size_t j = i; j < this->size(); j+= this->num_thread)
                    result[j] = this->at(j) + other;
            });
        }
        for(size_t i = 0; i < this->num_thread; i++) {
            if(threads[i].joinable()) {
                threads[i].join();
            }
        }
        return result;        
    };
    Lp_parallel_vector<T> operator-(const T& other)
    {
        Lp_parallel_vector<T> result;
        result.resize(this->size());
        for(size_t i = 0; i < this->num_thread; i++)
        {
            threads[i] = std::thread([this, &result, other, i]() {
                for(size_t j = i; j < this->size(); j+= this->num_thread)
                    result[j] = this->at(j) - other;
            });
        }
        for(size_t i = 0; i < this->num_thread; i++) {
            if(threads[i].joinable()) {
                threads[i].join();
            }
        }
        return result;        
    };
    Lp_parallel_vector<T> operator*(const T& other)
    {
        Lp_parallel_vector<T> result;
        result.resize(this->size());
        for(size_t i = 0; i < this->num_thread; i++)
        {
            threads[i] = std::thread([this, &result, other, i]() {
                for(size_t j = i; j < this->size(); j+= this->num_thread)
                    result[j] = this->at(j) * other;
            });
        }
        for(size_t i = 0; i < this->num_thread; i++) {
            if(threads[i].joinable()) {
                threads[i].join();
            }
        }
        return result;        
    };
    Lp_parallel_vector<T> operator/(const T& other)
    {
        Lp_parallel_vector<T> result;
        result.resize(this->size());
        for(size_t i = 0; i < this->num_thread; i++)
        {
            threads[i] = std::thread([this, &result, other, i]() {
                for(size_t j = i; j < this->size(); j+= this->num_thread)
                    result[j] = this->at(j) / other;
            });
        }
        for(size_t i = 0; i < this->num_thread; i++) {
            if(threads[i].joinable()) {
                threads[i].join();
            }
        }
        return result;        
    };
    Lp_parallel_vector<T> operator%(const T& other)
    {
        Lp_parallel_vector<T> result;
        result.resize(this->size());
        for(size_t i = 0; i < this->num_thread; i++)
        {
            threads[i] = std::thread([this, &result, other, i]() {
                for(size_t j = i; j < this->size(); j+= this->num_thread)
                    result[j] = this->at(j) % other;
            });
        }
        for(size_t i = 0; i < this->num_thread; i++) {
            if(threads[i].joinable()) {
                threads[i].join();
            }
        }
        return result;        
    };
    Lp_parallel_vector<T> operator&(const T& other)
    {
        Lp_parallel_vector<T> result;
        result.resize(this->size());
        for(size_t i = 0; i < this->num_thread; i++)
        {
            threads[i] = std::thread([this, &result, other, i]() {
                for(size_t j = i; j < this->size(); j+= this->num_thread)
                    result[j] = this->at(j) & other;
            });
        }
        for(size_t i = 0; i < this->num_thread; i++) {
            if(threads[i].joinable()) {
                threads[i].join();
            }
        }
        return result;        
    };
    Lp_parallel_vector<T> operator|(const T& other)
    {
        Lp_parallel_vector<T> result;
        result.resize(this->size());
        for(size_t i = 0; i < this->num_thread; i++)
        {
            threads[i] = std::thread([this, &result, other, i]() {
                for(size_t j = i; j < this->size(); j+= this->num_thread)
                    result[j] = this->at(j) | other;
            });
        }
        for(size_t i = 0; i < this->num_thread; i++) {
            if(threads[i].joinable()) {
                threads[i].join();
            }
        }
        return result;        
    };
    Lp_parallel_vector<T> operator^(const T& other)
    {
        Lp_parallel_vector<T> result;
        result.resize(this->size());
        for(size_t i = 0; i < this->num_thread; i++)
        {
            threads[i] = std::thread([this, &result, other, i]() {
                for(size_t j = i; j < this->size(); j+= this->num_thread)
                    result[j] = this->at(j) ^ other;
            });
        }
        for(size_t i = 0; i < this->num_thread; i++) {
            if(threads[i].joinable()) {
                threads[i].join();
            }
        }
        return result;        
    };
    Lp_parallel_vector<T> operator<<(const T& other)
    {
        Lp_parallel_vector<T> result;
        result.resize(this->size());
        for(size_t i = 0; i < this->num_thread; i++)
        {
            threads[i] = std::thread([this, &result, other, i]() {
                for(size_t j = i; j < this->size(); j+= this->num_thread)
                    result[j] = this->at(j) << other;
            });
        }
        for(size_t i = 0; i < this->num_thread; i++) {
            if(threads[i].joinable()) {
                threads[i].join();
            }
        }
        return result;        
    };
    Lp_parallel_vector<T> operator>>(const T& other)
    {
        Lp_parallel_vector<T> result;
        result.resize(this->size());
        for(size_t i = 0; i < this->num_thread; i++)
        {
            threads[i] = std::thread([this, &result, other, i]() {
                for(size_t j = i; j < this->size(); j+= this->num_thread)
                    result[j] = this->at(j) >> other;
            });
        }
        for(size_t i = 0; i < this->num_thread; i++) {
            if(threads[i].joinable()) {
                threads[i].join();
            }
        }
        return result;        
    };
    Lp_parallel_vector<T> operator~()
    {
        Lp_parallel_vector<T> result;
        result.resize(this->size());
        for(size_t i = 0; i < this->num_thread; i++)
        {
            threads[i] = std::thread([this, &result, i]() {
                for(size_t j = i; j < this->size(); j+= this->num_thread)
                    result[j] = ~this->at(j);
            });
        }
        for(size_t i = 0; i < this->num_thread; i++) {
            if(threads[i].joinable()) {
                threads[i].join();
            }
        }
        return result;        
    };

private:
    size_t num_thread;
    std::thread threads[128];
};

template<typename T>
static void Lp_if_parallel(Lp_parallel_vector<T> vec, std::function<void(size_t)> func)
{
    std::thread threads[128];
    for(size_t i = 0; i < std::thread::hardware_concurrency(); i++)
    {
        threads[i] = std::thread([&vec, i, &func]() {
            for(size_t j = i; j < vec.size(); j+= std::thread::hardware_concurrency())
                if(vec[j])
                    func(j);
        });
    }
    for(size_t i = 0; i < std::thread::hardware_concurrency(); i++) {
        if(threads[i].joinable()) {
            threads[i].join();
        }
    }
}
template<typename T>
static void Lp_if_single_threaded(Lp_parallel_vector<T>& vec, std::function<void(size_t)> func)
{
    for(size_t j = 0; j < vec.size(); j++)
        if(vec[j])
            func(j);
}
template<typename T>
void Lp_sequential_quicksort(std::vector<T>& arr, size_t low, size_t high, std::function<bool(T, T)> comp) {
    if (low >= high) return;
    
    // Choose pivot (middle element)
    T pivot = arr[low + (high - low) / 2];
    
    // Partition
    size_t i = low;
    size_t j = high;
    
    while (true) {
        while (i < arr.size() && comp(arr[i], pivot)) i++;
        while (j > 0 && comp(pivot, arr[j])) j--;
        
        if (i >= j) break;
        
        std::swap(arr[i], arr[j]);
        i++;
        j--;
    }
    
    // Recursively sort sub-arrays
    if (low < j)
        Lp_sequential_quicksort(arr, low, j, comp);
    
    if (i < high)
        Lp_sequential_quicksort(arr, i, high, comp);
}

template<typename T>
void Lp_parallel_quicksort(std::vector<T>& arr, size_t low, size_t high, std::function<bool(T, T)> comp, 
                          size_t depth, std::vector<std::thread>& threads, size_t& thread_count, size_t max_threads) {
    if (low < high) {
        // Choose pivot (middle element)
        T pivot = arr[low + (high - low) / 2];
        
        // Partition
        size_t i = low;
        // Need to use signed type for j to handle the case when j becomes negative
        std::ptrdiff_t j = static_cast<std::ptrdiff_t>(high);
        
        while (i <= static_cast<size_t>(j)) {
            while (i < arr.size() && comp(arr[i], pivot)) i++;
            while (j >= 0 && comp(pivot, arr[j])) j--;
            
            if (i <= static_cast<size_t>(j)) {
                std::swap(arr[i], arr[j]);
                i++;
                j--;
            }
        }
        
        // Decide whether to use parallel or sequential sorting for sub-arrays
        bool use_parallel = (depth > 0) && (thread_count < max_threads);
        std::cout << "Decided to use parallel sorting: " << use_parallel << std::endl;
        if (use_parallel && j >= 0 && low < static_cast<size_t>(j)) {
            std::cout << "Sorting left part in a new thread" << std::endl;
            // Sort left part in a new thread
            thread_count++;
            size_t j_pos = static_cast<size_t>(j);
            threads[thread_count-1] = std::thread([&arr, low, j_pos, comp, depth, &threads, &thread_count, max_threads]() {
                Lp_parallel_quicksort(arr, low, j_pos, comp, depth-1, threads, thread_count, max_threads);
            });
        } else if (j >= 0 && low < static_cast<size_t>(j)) {
            std::cout << "Sorting left part sequentially" << std::endl;
            // Sort left part sequentially
            Lp_sequential_quicksort(arr, low, static_cast<size_t>(j), comp);
        }
        
        if (i < high) {
            std::cout << "Sorting right part in the current thread" << std::endl;
            // Always sort right part in the current thread
            Lp_parallel_quicksort(arr, i, high, comp, depth-1, threads, thread_count, max_threads);
        }
    }
}



// Parallel quicksort implementation using a thread pool
template<typename T>
void Lp_sort(Lp_parallel_vector<T>& vec, std::function<bool(T, T)> comp)
{
    // Check if the vector is empty or has only one element
    if (vec.size() <= 1) {
        return; // Already sorted
    }
    
    // Create a copy of the vector data to work with
    std::vector<T> arr(vec.begin(), vec.end());
    
    // Get number of hardware threads
    size_t num_threads = std::thread::hardware_concurrency();
    
    // Create a thread pool
    std::vector<std::thread> thread_pool;
    
    // Create a mutex for thread synchronization
    std::mutex mutex;
    
    // Create a queue of tasks (ranges to sort)
    std::vector<std::pair<size_t, size_t>> task_queue;
    task_queue.push_back({0, arr.size() - 1});
    
    // Create an atomic counter for active tasks
    std::atomic<size_t> active_tasks(1);
    
    // Create a condition variable for synchronization
    std::condition_variable cv;
    
    // Function to process tasks from the queue
    auto process_tasks = [&]() {
        while (true) {
            // Get a task from the queue
            std::pair<size_t, size_t> task;
            {
                std::unique_lock<std::mutex> lock(mutex);
                
                // Wait until there's a task or all tasks are done
                cv.wait(lock, [&]() {
                    return !task_queue.empty() || active_tasks.load() == 0;
                });
                
                // If all tasks are done, exit
                if (task_queue.empty() && active_tasks.load() == 0) {
                    break;
                }
                
                // Get a task from the queue
                if (!task_queue.empty()) {
                    task = task_queue.back();
                    task_queue.pop_back();
                } else {
                    continue;
                }
            }
            
            // Process the task
            size_t low = task.first;
            size_t high = task.second;
            
            // If the range is small, use sequential sort
            if (high - low < 1000) {
                std::sort(arr.begin() + low, arr.begin() + high + 1, comp);
                active_tasks--;
                cv.notify_all();
                continue;
            }
            
            // Partition the array
            size_t pivot_idx;
            
            // Simple partitioning
            {
                // Choose pivot (middle element)
                T pivot = arr[low + (high - low) / 2];
                
                // Initialize indices
                size_t i = low;
                size_t j = high;
                
                // Partition the array
                while (true) {
                    // Find element on left that should be on right
                    while (i < arr.size() && comp(arr[i], pivot)) i++;
                    
                    // Find element on right that should be on left
                    while (j > 0 && comp(pivot, arr[j])) j--;
                    
                    // If indices crossed, break
                    if (i >= j) {
                        pivot_idx = j;
                        break;
                    }
                    
                    // Swap elements
                    std::swap(arr[i], arr[j]);
                    i++;
                    j--;
                }
            }
            
            // Add new tasks to the queue
            {
                std::lock_guard<std::mutex> lock(mutex);
                
                // Add left sub-array to the queue
                if (pivot_idx > 0 && low < pivot_idx) {
                    task_queue.push_back({low, pivot_idx});
                    active_tasks++;
                }
                
                // Add right sub-array to the queue
                if (pivot_idx < high) {
                    task_queue.push_back({pivot_idx + 1, high});
                    active_tasks++;
                }
            }
            
            // Decrement active tasks counter
            active_tasks--;
            
            // Notify waiting threads
            cv.notify_all();
        }
    };
    
    // Start worker threads
    for (size_t i = 0; i < num_threads; i++) {
        thread_pool.push_back(std::thread(process_tasks));
    }
    
    // Wait for all threads to finish
    for (auto& thread : thread_pool) {
        if (thread.joinable()) {
            thread.join();
        }
    }
    
    // Copy the sorted data back to the original vector
    for (size_t i = 0; i < vec.size(); i++) {
        vec[i] = arr[i];
    }
}