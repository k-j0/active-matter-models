#pragma once

#include "DoubleBufferedModel.h"

template<int D>
class Vicsek : public DoubleBufferedModel<D> {
    
    float r2;
    float sqrt2Dr;
    
protected:
    std::string getName () override { return "Vicsek model"; }
    
public:
    Vicsek (std::size_t particleCount, float periodicity, float detectionRadius, float angularDiffusion, unsigned int seed);
    void update () override;
    
};



template<int D>
Vicsek<D>::Vicsek (std::size_t particleCount, float periodicity, float detectionRadius, float angularDiffusion, unsigned int seed) :
    DoubleBufferedModel<D>(particleCount, periodicity, seed), r2(detectionRadius*detectionRadius), sqrt2Dr(std::sqrt(2.0f * angularDiffusion)) { }

template<int D>
void Vicsek<D>::update () {
    
    // each step, read from front while writing to back, then swap so the latest state is always in the front buffer
    
    std::size_t sz = this->particlesFront->size();
    
    #pragma omp parallel for
    for (std::size_t i = 0; i < sz; ++i) {
        
        // find mean rotation from neighbours
        Vec<D> pos = (*this->particlesFront)[i].pos;
        Vec<D> direction = VecUtils::toCartesian<D>((*this->particlesFront)[i].rotation);
        unsigned int neighbourCount = 1;
        for (std::size_t j = 0; j < sz; ++j) {
            if (i == j) continue;
            Vec<D> otherPos = (*this->particlesFront)[j].pos;
            if ((pos-otherPos).lengthSqr() <= r2) { // distance squared <= radius squared
                ++neighbourCount;
                direction += VecUtils::toCartesian<D>((*this->particlesFront)[j].rotation);
            }
        }
        Vec<D-1> rotation;
        if (direction.normalize()) {
            rotation = VecUtils::toSpherical<D>(direction);
        } else { // if the average direction is exactly 0, just keep the previous direction for now
            rotation = (*this->particlesFront)[i].rotation;
        }
        
        // apply white noise to rotation
        for (int d = 0; d < D-1; ++d) {
            rotation.set(d, rotation.get(d) + sqrt2Dr*this->rand(std::normal_distribution<float>(0.0f, 1.0f)));
        }
        
        // update back buffer
        (*this->particlesBack)[i].rotation = rotation;
        direction = VecUtils::toCartesian<D>(rotation);
        (*this->particlesBack)[i].pos = pos + direction;
        
        // ensure periodic domain
        if (this->periodicity > 0) {
            (*this->particlesBack)[i].pos.periodic(this->periodicity);
        }
        
    }
    
    // bring back buffer to front for next timestep
    this->swapBuffers();
}
