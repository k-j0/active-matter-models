#pragma once

#include <random>
#include <functional>

class Model {
    
protected:
    // random number generator between 0 and 1
    decltype(std::bind(std::uniform_real_distribution<float>(0.0f, 1.0f), std::mt19937())) rand01;
    
public:
    Model (unsigned int seed) :
        rand01(std::bind(std::uniform_real_distribution<float>(0.0f, 1.0f), std::mt19937(seed)))
    { }
    
    virtual ~Model () { }
    virtual void update() = 0;
    
};
