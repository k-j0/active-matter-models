
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

float RunAndTumble::getMSD () {
    float sum = 0.0f;
    std::size_t sz = particles.size();
    
    #pragma omp parallel for reduction(+: sum)
    for (std::size_t i = 0; i < sz; ++i) {
        sum += particles[i].pos.lengthSqr();
    }
    
    return sum / sz;
}

void RunAndTumble::print () {
    std::size_t size = particles.size();
    std::printf("Run and tumble, %ld particles:\n", size);
    for (std::size_t i = 0, sz = size > 10 ? 10 : size; i < sz; ++i) {
        std::printf("\t- (%ld) %s, %s\n", i, particles[i].pos.toString().c_str(), particles[i].dir.toString().c_str());
    }
    if (size > 10) std::printf("\t- ... (only first 10 shown)\n");
    std::printf("MSD: %f\n", getMSD());
    std::printf("\n");
}
