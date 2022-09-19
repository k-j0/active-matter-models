#pragma once

#include "models/Model.h"

template<int D>
class RandomWalk : public Model<D> {
    
protected:
    std::string getName () override { return "Random Walk"; }
    void updateParticle (std::size_t i) override;
    
public:
    RandomWalk (typename Model<D>::Params params) : Model<D>(params) { }
    
};




template<int D>
void RandomWalk<D>::updateParticle (std::size_t i) {
    
    // change direction at random
    this->particles[i].rotation = this->randomRotation();
    
    // move forward
    Vec<D> direction = VecUtils::toCartesian<D>(this->particles[i].rotation);
    this->particles[i].pos += direction;
    
}
