
#include "RunAndTumble.h"

RunAndTumble::RunAndTumble (std::size_t particleCount, float flipProbability, unsigned int seed) :
        Model(seed),
        flipProbability(flipProbability)
{
    particles.reserve(particleCount);
    for (std::size_t i = 0; i < particleCount; ++i) {
        float theta = rand01() * PI * 2.0f;
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
        float r = rand01();
        if (r < flipProbability) {
            // Set direction to new value; the valocity v0 is considered to always be 1
            float theta = rand01() * PI * 2.0f;
            particles[i].dir = { std::cos(theta), std::sin(theta) };
        }
        
        // keep running
        particles[i].pos += particles[i].dir;
        
    }
}

void RunAndTumble::print () {
    std::size_t sz = particles.size();
    std::printf("Run and tumble, %ld particles:\n", sz);
    for (std::size_t i = 0; i < sz; ++i) {
        std::printf("\t- (%ld) %s, %s\n", i, particles[i].pos.toString().c_str(), particles[i].dir.toString().c_str());
    }
    std::printf("\n");
}
