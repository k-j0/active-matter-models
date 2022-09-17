
#include "RunAndTumble.h"

RunAndTumble::RunAndTumble (std::size_t particleCount, float flipProbability, unsigned int seed) :
        Model(seed),
        flipProbability(flipProbability)
{
    particles.reserve(particleCount);
    for (std::size_t i = 0; i < particleCount; ++i) {
        float theta = rand() * PI * 2.0f;
        particles.push_back({
            { 0, 0 },
            { std::cos(theta), std::sin(theta) }
        });
    }
}

void RunAndTumble::update () {
    
    std::size_t sz = particles.size();
    
    #pragma omp parallel for
    for (std::size_t i = 0; i < sz; ++i) {
        
        // tumble with probability flipProbability
        float r = rand();
        if (r < flipProbability) {
            // Set direction to new value; the valocity v0 is considered to always be 1
            float theta = rand() * PI * 2.0f;
            particles[i].dir = { std::cos(theta), std::sin(theta) };
        }
        
        // keep running
        particles[i].pos += particles[i].dir;
        
    }
}
