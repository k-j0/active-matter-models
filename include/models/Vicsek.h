#pragma once

#include "models/DoubleBufferedModel.h"

template<int D>
class Vicsek : public DoubleBufferedModel<D> {
    
    float r2;
    float sqrt2Dr;
    
protected:
    std::string getName () override { return "Vicsek model"; }
    void updateParticle (std::size_t i) override;
    
public:
    Vicsek (float detectionRadius, float angularDiffusion, typename Model<D>::Params params);
    
};



template<int D>
Vicsek<D>::Vicsek (float detectionRadius, float angularDiffusion, typename Model<D>::Params params) :
    DoubleBufferedModel<D>(params), r2(detectionRadius*detectionRadius), sqrt2Dr(std::sqrt(2.0f * angularDiffusion)) { }

template<int D>
void Vicsek<D>::updateParticle (std::size_t i) {
    
    // find mean rotation from neighbours
    Vec<D> pos = (*this->particlesFront)[i].pos;
    Vec<D> direction = VecUtils::toCartesian<D>((*this->particlesFront)[i].rotation);
    unsigned int neighbourCount = 1;
    for (std::size_t j = 0; j < this->particleCount; ++j) {
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
    
}
