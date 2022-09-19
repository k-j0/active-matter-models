#pragma once

#include "models/Model.h"

template<int D>
class ActiveBrownianMotion : public Model<D> {
    
    float sqrt2Dr;
    
protected:
    std::string getName () override { return "Active Brownian Motion"; }
    void updateParticle (std::size_t i) override;
    
public:
    ActiveBrownianMotion (float angularDiffusion, typename Model<D>::Params params);
    
};



template<int D>
ActiveBrownianMotion<D>::ActiveBrownianMotion (float angularDiffusion, typename Model<D>::Params params) :
    Model<D>(params), sqrt2Dr(std::sqrt(2.0f * angularDiffusion)) { }

template<int D>
void ActiveBrownianMotion<D>::updateParticle (std::size_t i) {
    
    // apply white noise to rotation
    for (int d = 0; d < D-1; ++d) {
        this->particles[i].rotation.set(d, this->particles[i].rotation.get(d) + sqrt2Dr*this->rand(std::normal_distribution<float>(0.0f, 1.0f)));
    }
    
    // keep running
    Vec<D> direction = VecUtils::toCartesian<D>(this->particles[i].rotation);
    this->particles[i].pos += direction;
    
}
