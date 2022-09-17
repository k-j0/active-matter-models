#pragma once

#include "Model.h"
#include "Vec.h"
#include "VecUtils.h"
#include <vector>

template<int D>
class ActiveBrownianMotion : public Model {
    
    struct Particle {
        Vec<D> pos;
        Vec<D-1> angle; // in 2D, theta; in 3D theta (lat) and phi (long)
    };
    std::vector<Particle> particles;
    
    float sqrt2Dr;
    
private:
    Vec<D-1> randomAngle ();
    
public:
    ActiveBrownianMotion (std::size_t particleCount, float angularDiffusion, unsigned int seed);
    void update () override;
    float getMSD () override;
    void print () override;
    
};



template<int D>
ActiveBrownianMotion<D>::ActiveBrownianMotion (std::size_t particleCount, float angularDiffusion, unsigned int seed) :
        Model(seed),
        sqrt2Dr(std::sqrt(2.0f * angularDiffusion))
{
    particles.reserve(particleCount);
    for (std::size_t i = 0; i < particleCount; ++i) {
        particles.push_back({
            Vec<D>::Zero(),
            randomAngle()
        });
    }
}

template<>
Vec<1> ActiveBrownianMotion<2>::randomAngle () {
    return { rand01() * PI * 2.0f };
}

template<>
Vec<2> ActiveBrownianMotion<3>::randomAngle () {
    return {
        rand01() * PI * 2.0f,
        rand01() * PI
    };
}

template<int D>
void ActiveBrownianMotion<D>::update () {
    
    std::size_t sz = particles.size();
    
    #pragma omp parallel for
    for (std::size_t i = 0; i < sz; ++i) {
        
        // apply white noise to angles
        for (int d = 0; d < D-1; ++d) {
            particles[i].angle.set(d, particles[i].angle.get(d) + sqrt2Dr*rand(std::normal_distribution<float>(0.0f, 1.0f)));
        }
        
        // keep running
        Vec<D> direction = VecUtils::toCartesian<D>(particles[i].angle);
        particles[i].pos += direction;
        
    }
}

template<int D>
float ActiveBrownianMotion<D>::getMSD () {
    float sum = 0.0f;
    std::size_t sz = particles.size();
    
    #pragma omp parallel for reduction(+: sum)
    for (std::size_t i = 0; i < sz; ++i) {
        sum += particles[i].pos.lengthSqr();
    }
    
    return sum / sz;
}

template<int D>
void ActiveBrownianMotion<D>::print () {
    std::size_t size = particles.size();
    std::printf("Active Brownian Motion, %ld particles:\n", size);
    for (std::size_t i = 0, sz = size > 10 ? 10 : size; i < sz; ++i) {
        std::printf("\t- (%ld) %s\n", i, particles[i].pos.toString().c_str());
    }
    if (size > 10) std::printf("\t- ... (only first 10 shown)\n");
    std::printf("MSD: %f\n", getMSD());
    std::printf("\n");
}
