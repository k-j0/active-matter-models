#pragma once

#include "models/DoubleBufferedModel.h"

// Note: this a version of the Boids model meant to represent low Reynolds number active matter, i.e. no inertia terms are taken into account

template<int D>
class Boids : public DoubleBufferedModel<D> {
    
public:
    
    struct Params {
        float detectionRadius = 25.0f;
        float separationRadius = 5.0f;
        float angularDiffusion = 0.02f;
        float separationCoeff = 1.0f;
        float alignmentCoeff = 0.2f;
        float cohesionCoeff = 0.2f;
        float adoptionRate = 0.25f; // 0..1
    };
    
private:
    
    Params params;
    float sqrt2Dr;
    
protected:
    std::string getName () override { return "Boids"; }
    
public:
    Boids (Params params, typename Model<D>::Params modelParams);
    void update () override;
    
};



template<int D>
Boids<D>::Boids (Boids<D>::Params params, typename Model<D>::Params modelParams) :
        DoubleBufferedModel<D>(modelParams),
        params(params),
        sqrt2Dr(std::sqrt(2.0f * params.angularDiffusion)) { }

template<int D>
void Boids<D>::update () {
    
    // each step, read from front while writing to back, then swap so the latest state is always in the front buffer
    
    std::size_t sz = this->particlesFront->size();
    
    #pragma omp parallel for
    for (std::size_t i = 0; i < sz; ++i) {
        
        // separation, alignment and cohesion steps
        Vec<D> pos = (*this->particlesFront)[i].pos;
        Vec<D> dir = VecUtils::toCartesian<D>((*this->particlesFront)[i].rotation);
        Vec<D> separation = Vec<D>::Zero();
        Vec<D> alignment = dir;
        Vec<D> cohesion = Vec<D>::Zero();
        std::size_t neighbourCount = 0;
        for (std::size_t j = 0; j < sz; ++j) {
            if (i == j) continue;
            const Vec<D>& otherPos = (*this->particlesFront)[j].pos;
            float distanceSqr = (pos - otherPos).lengthSqr();
            
            // separation step
            if (distanceSqr <= params.separationRadius * params.separationRadius) {
                Vec<D> delta = pos - otherPos;
                delta.normalize();
                delta *= std::sqrt(distanceSqr);
                separation += delta;
            }
            
            // alignment & cohesion steps
            if (distanceSqr <= params.detectionRadius * params.detectionRadius) {
                alignment += VecUtils::toCartesian<D>((*this->particlesFront)[j].rotation);
                cohesion += otherPos;
                ++neighbourCount;
            }
        }
        
        // Normalize the direction vectors
        if (!separation.isZero()) {
            separation.normalize();
        }
        if (!alignment.isZero()) {
            alignment.normalize();
        }
        if (!cohesion.isZero()) {
            cohesion *= 1.0f / neighbourCount;
            cohesion = cohesion - pos;
            cohesion.normalize();
        }
        
        Vec<D> targetDirection = separation * params.separationCoeff + alignment * params.alignmentCoeff + cohesion * params.cohesionCoeff;
        Vec<D> direction = Vec<D>::Lerp(dir, targetDirection, params.adoptionRate);
        Vec<D-1> rotation = VecUtils::toSpherical<D>(direction);
        
        // apply white noise to rotation
        for (int d = 0; d < D-1; ++d) {
            rotation.set(d, rotation.get(d) + sqrt2Dr*this->rand(std::normal_distribution<float>(0.0f, 1.0f)));
        }
        direction = VecUtils::toCartesian<D>(rotation);
        
        // update back buffer
        (*this->particlesBack)[i].rotation = rotation;
        (*this->particlesBack)[i].pos = pos + direction;
        
        // ensure periodic domain
        if (this->periodicity > 0) {
            (*this->particlesBack)[i].pos.periodic(this->periodicity);
        }
        
    }
    
    // bring back buffer to front for next timestep
    this->swapBuffers();
}
