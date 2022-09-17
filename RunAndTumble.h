#pragma once

#include "Model.h"
#include "Vec.h"
#include <vector>

class RunAndTumble : public Model {
    
    struct Particle {
        Vec<2> pos;
        Vec<2> dir; // always normalized
    };
    std::vector<Particle> particles;
    
    float flipProbability;
    
public:
    RunAndTumble (std::size_t particleCount, float flipProbability, unsigned int seed);
    void update () override;
    float getMSD () override;
    void print () override;
    
};
