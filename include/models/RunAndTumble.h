#pragma once

#include "models/Model.h"

template<int D>
class RunAndTumble : public Model<D> {
    
    float flipProbability;
    
protected:
    std::string getName () override { return "Run & Tumble"; }
    
public:
    RunAndTumble (float flipProbability, typename Model<D>::Params params);
    void update () override;
    
};



template<int D>
RunAndTumble<D>::RunAndTumble (float flipProbability, typename Model<D>::Params params) :
    Model<D>(params), flipProbability(flipProbability) { }

template<int D>
void RunAndTumble<D>::update () {
    
    #pragma omp parallel for
    for (std::size_t i = 0; i < this->particleCount; ++i) {
        
        // tumble with probability flipProbability
        float r = this->rand01();
        if (r < flipProbability) {
            // Set direction to new value; the velocity v0 is considered to always be 1
            this->particles[i].rotation = this->randomRotation();
        }
        
        // keep running
        Vec<D> direction = VecUtils::toCartesian<D>(this->particles[i].rotation);
        this->particles[i].pos += direction;
        
        // ensure periodic domain
        if (this->periodicity > 0) {
            this->particles[i].pos.periodic(this->periodicity);
        }
    }
}
