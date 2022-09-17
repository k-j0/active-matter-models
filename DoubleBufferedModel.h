#pragma once

#include "Model.h"

template<int D>
class DoubleBufferedModel : public Model<D> {
    
protected:
    
    // double-buffered implementation, to allow for reading the data from the previous timestep unimpeded
    std::vector<Particle<D>> heldParticles;
    std::vector<Particle<D>>* particlesFront, * particlesBack;
    
    void swapBuffers () {
        auto* oldBack = particlesBack;
        particlesBack = particlesFront;
        particlesFront = oldBack;
    }
    
public:
    DoubleBufferedModel (std::size_t particleCount, float periodicity, unsigned int seed) : Model<D>(particleCount, periodicity, seed) {
        heldParticles = std::vector<Particle<D>>(particleCount);
        particlesFront = &this->particles;
        particlesBack = &heldParticles;
    }
    
    virtual float getMSD () override;
    virtual void print () override;
    
};


template<int D>
float DoubleBufferedModel<D>::getMSD () {
    float sum = 0.0f;
    
    #pragma omp parallel for reduction(+: sum)
    for (std::size_t i = 0; i < this->particleCount; ++i) {
        sum += (*particlesFront)[i].pos.lengthSqr();
    }
    
    return sum / this->particleCount;
}

template<int D>
void DoubleBufferedModel<D>::print () {
    std::printf("%s, %ld particles:\n", this->getName().c_str(), this->particleCount);
    for (std::size_t i = 0, sz = this->particleCount > 10 ? 10 : this->particleCount; i < sz; ++i) {
        std::printf("\t- (%ld) %s\n", i, (*particlesFront)[i].pos.toString().c_str());
    }
    if (this->particleCount > 10) std::printf("\t- ... (only first 10 shown)\n");
    std::printf("MSD: %f\n", getMSD());
    std::printf("\n");
}
