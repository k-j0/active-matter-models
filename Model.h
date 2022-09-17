#pragma once

#include <random>
#include <functional>
#include <thread>

class Model {
    
    unsigned int seed;
    
protected:
    // thread-safe random number generator
    // note: depending on inconsistencies in thread scheduling, re-using the same seed might still produce different results even on the same hardware
    template<typename distribution_t = std::uniform_real_distribution<float>>
    float rand (distribution_t distribution) {
        static thread_local std::mt19937 gen(seed + std::hash<std::thread::id>{}(std::this_thread::get_id()));
        return distribution(gen);
    }
    
    float rand01 () {
        return rand(std::uniform_real_distribution(0.0f, 1.0f));
    }
    
public:
    Model (unsigned int seed) : seed(seed) { }
    
    virtual ~Model () { }
    virtual void update() = 0;
    virtual float getMSD() = 0;
    virtual void print() = 0;
    
};
