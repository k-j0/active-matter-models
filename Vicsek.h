#pragma once

#include "Model.h"
#include "Vec.h"
#include "VecUtils.h"
#include <vector>

template<int D>
class Vicsek : public Model {
    
    struct Particle {
        Vec<D> pos;
        Vec<D-1> rotation; // in 2D, theta; in 3D theta (lat) and phi (long)
    };
    // double-buffered implementation, to allow for reading the data from the previous timestep unimpeded
    std::vector<Particle> particlesBuffer1, particlesBuffer2;
    std::vector<Particle>* particlesFront, * particlesBack;
    
    float r2;
    float sqrt2Dr;
    
private:
    Vec<D-1> randomAngle ();
    
    void swapBuffers () {
        auto* oldBack = particlesBack;
        particlesBack = particlesFront;
        particlesFront = oldBack;
    }
    
public:
    Vicsek (std::size_t particleCount, float detectionRadius, float angularDiffusion, unsigned int seed);
    void update () override;
    float getMSD () override;
    void print () override;
    
};



template<int D>
Vicsek<D>::Vicsek (std::size_t particleCount, float detectionRadius, float angularDiffusion, unsigned int seed) :
        Model(seed),
        r2(detectionRadius*detectionRadius),
        sqrt2Dr(std::sqrt(2.0f * angularDiffusion))
{
    particlesBuffer1.reserve(particleCount);
    particlesBuffer2.reserve(particleCount);
    for (std::size_t i = 0; i < particleCount; ++i) {
        particlesBuffer1.push_back({
            Vec<D>::Zero(),
            randomAngle()
        });
        particlesBuffer2.push_back({});
    }
    particlesFront = &particlesBuffer1;
    particlesBack = &particlesBuffer2;
}

template<>
Vec<1> Vicsek<2>::randomAngle () {
    return { rand01() * PI * 2.0f };
}

template<>
Vec<2> Vicsek<3>::randomAngle () {
    return {
        rand01() * PI * 2.0f,
        rand01() * PI
    };
}

template<int D>
void Vicsek<D>::update () {
    
    // each step, read from front while writing to back, then swap so the latest state is always in the front buffer
    
    std::size_t sz = particlesFront->size();
    
    #pragma omp parallel for
    for (std::size_t i = 0; i < sz; ++i) {
        
        // find mean rotation from neighbours
        Vec<D> pos = (*particlesFront)[i].pos;
        Vec<D> direction = VecUtils::toCartesian<D>((*particlesFront)[i].rotation);
        unsigned int neighbourCount = 1;
        for (std::size_t j = 0; j < sz; ++j) {
            if (i == j) continue;
            Vec<D> otherPos = (*particlesFront)[j].pos;
            if ((pos-otherPos).lengthSqr() <= r2) { // distance squared <= radius squared
                ++neighbourCount;
                direction += VecUtils::toCartesian<D>((*particlesFront)[j].rotation);
            }
        }
        Vec<D-1> rotation;
        if (direction.normalize()) {
            rotation = VecUtils::toSpherical<D>(direction);
        } else { // if the average direction is exactly 0, just keep the previous direction for now
            rotation = (*particlesFront)[i].rotation;
        }
        
        // apply white noise to rotation
        for (int d = 0; d < D-1; ++d) {
            rotation.set(d, rotation.get(d) + sqrt2Dr*rand(std::normal_distribution<float>(0.0f, 1.0f)));
        }
        
        // update back buffer
        (*particlesBack)[i].rotation = rotation;
        direction = VecUtils::toCartesian<D>(rotation);
        (*particlesBack)[i].pos = pos + direction;
        
    }
    
    // bring back buffer to front for next timestep
    swapBuffers();
}

template<int D>
float Vicsek<D>::getMSD () {
    float sum = 0.0f;
    std::size_t sz = particlesFront->size();
    
    #pragma omp parallel for reduction(+: sum)
    for (std::size_t i = 0; i < sz; ++i) {
        sum += (*particlesFront)[i].pos.lengthSqr();
    }
    
    return sum / sz;
}

template<int D>
void Vicsek<D>::print () {
    std::size_t size = particlesFront->size();
    std::printf("Vicsek model, %ld particles:\n", size);
    for (std::size_t i = 0, sz = size > 10 ? 10 : size; i < sz; ++i) {
        std::printf("\t- (%ld) %s\n", i, (*particlesFront)[i].pos.toString().c_str());
    }
    if (size > 10) std::printf("\t- ... (only first 10 shown)\n");
    std::printf("MSD: %f\n", getMSD());
    std::printf("\n");
}
