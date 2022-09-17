#pragma once

#include "Model.h"
#include "Vec.h"
#include <vector>

template<int D>
class RunAndTumble : public Model {
    
    struct Particle {
        Vec<D> pos;
        Vec<D> dir; // always normalized
    };
    std::vector<Particle> particles;
    
    float flipProbability;
    
private:
    Vec<D> randomDirection ();
    
public:
    RunAndTumble (std::size_t particleCount, float flipProbability, unsigned int seed);
    void update () override;
    float getMSD () override;
    void print () override;
    
};



template<int D>
RunAndTumble<D>::RunAndTumble (std::size_t particleCount, float flipProbability, unsigned int seed) :
        Model(seed),
        flipProbability(flipProbability)
{
    particles.reserve(particleCount);
    for (std::size_t i = 0; i < particleCount; ++i) {
        particles.push_back({
            Vec<D>::Zero(),
            randomDirection()
        });
    }
}

template<>
Vec<2> RunAndTumble<2>::randomDirection () {
    float theta = rand01() * PI * 2.0f;
    return { std::cos(theta), std::sin(theta) };
}

template<>
Vec<3> RunAndTumble<3>::randomDirection () {
    // random number on cylindrical axial projection, i.e. uniform on the unit sphere
    float theta = rand01() * PI * 2.0f;
    float z = rand01() * 2.0f - 1.0f;
    float sqrt1Mz2 = std::sqrt(1.0f - z*z);
    return {
        sqrt1Mz2 * std::cos(theta),
        sqrt1Mz2 * std::sin(theta),
        z
    };
}

template<int D>
void RunAndTumble<D>::update () {
    
    std::size_t sz = particles.size();
    
    #pragma omp parallel for
    for (std::size_t i = 0; i < sz; ++i) {
        
        // tumble with probability flipProbability
        float r = rand01();
        if (r < flipProbability) {
            // Set direction to new value; the velocity v0 is considered to always be 1
            particles[i].dir = randomDirection();
        }
        
        // keep running
        particles[i].pos += particles[i].dir;
        
    }
}

template<int D>
float RunAndTumble<D>::getMSD () {
    float sum = 0.0f;
    std::size_t sz = particles.size();
    
    #pragma omp parallel for reduction(+: sum)
    for (std::size_t i = 0; i < sz; ++i) {
        sum += particles[i].pos.lengthSqr();
    }
    
    return sum / sz;
}

template<int D>
void RunAndTumble<D>::print () {
    std::size_t size = particles.size();
    std::printf("Run and Tumble, %ld particles:\n", size);
    for (std::size_t i = 0, sz = size > 10 ? 10 : size; i < sz; ++i) {
        std::printf("\t- (%ld) %s, %s\n", i, particles[i].pos.toString().c_str(), particles[i].dir.toString().c_str());
    }
    if (size > 10) std::printf("\t- ... (only first 10 shown)\n");
    std::printf("MSD: %f\n", getMSD());
    std::printf("\n");
}
