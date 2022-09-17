#pragma once

#include <random>
#include <functional>
#include <thread>

class Model {
    
    unsigned int seed;
    
protected:
    // thread-safe random number generator
    float rand01 () {
        static thread_local std::mt19937 gen(seed + std::hash<std::thread::id>{}(std::this_thread::get_id()));
        return std::uniform_real_distribution<float>(0.0f, 1.0f)(gen);
    }
    
public:
    Model (unsigned int seed) : seed(seed) { }
    
    virtual ~Model () { }
    virtual void update() = 0;
    
};
