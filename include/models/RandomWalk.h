#pragma once

#include "models/Model.h"

template<int D>
class RandomWalk : public Model<D> {
    
protected:
    std::string getName () override { return "Random Walk"; }
    
public:
    RandomWalk (typename Model<D>::Params params) : Model<D>(params) { }
    void update () override;
    
};




template<int D>
void RandomWalk<D>::update () {
    
    #pragma omp parallel for
    for (std::size_t i = 0; i < this->particleCount; ++i) {
        
        // change direction at random
        this->particles[i].rotation = this->randomRotation();
        
        // move forward
        Vec<D> direction = VecUtils::toCartesian<D>(this->particles[i].rotation);
        this->particles[i].pos += direction;
        
        // ensure periodic domain
        if (this->periodicity > 0) {
            this->particles[i].pos.periodic(this->periodicity);
        }
    }
}
