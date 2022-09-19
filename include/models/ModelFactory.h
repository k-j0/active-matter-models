#pragma once

#include <string>
#include "Arguments.h"
#include "models/Model.h"
#include "models/RandomWalk.h"
#include "models/RunAndTumble.h"
#include "models/ActiveBrownianMotion.h"
#include "models/Vicsek.h"
#include "models/Boids.h"

class ModelFactory {
    ModelFactory()=delete;
    ModelFactory(const ModelFactory&)=delete;
    ModelFactory(ModelFactory&&)=delete;
public:
    
    template<int D>
    static ModelBase* build(Arguments& args) {
        
        std::string name = args.read<std::string>("model");
        typename Model<D>::Params params;
        params.particleCount = args.read<int>("particles", 512);
        params.periodicity = args.read<int>("periodic-size", 500); // -1 for non-periodic domain
        params.startUniformly = !args.read<bool>("non-uniform-start", false);
        params.seed = args.read<int>("seed", 0);
        
        if (name.compare("random-walk") == 0) {
            return new RandomWalk<D>(params);
        }
        
        if (name.compare("run-and-tumble") == 0) {
            return new RunAndTumble<D>(args.read<float>("flip-prob", 0.1f), params);
        }
        
        if (name.compare("active-brownian") == 0) {
            return new ActiveBrownianMotion<D>(args.read<float>("angular-diffusion", 0.02f), params);
        }
        
        if (name.compare("vicsek") == 0) {
            return new Vicsek<D>(args.read<float>("detection-radius", 25.0f), args.read<float>("angular-diffusion", 0.02f), params);
        }
        
        if (name.compare("boids") == 0) {
            typename Boids<D>::Params boidsParams;
            boidsParams.detectionRadius = args.read<float>("detection-radius", 25.0f);
            boidsParams.separationRadius = args.read<float>("separation-radius", 5.0f);
            boidsParams.angularDiffusion = args.read<float>("angular-diffusion", 0.02f);
            boidsParams.separationCoeff = args.read<float>("separation", 1.0f);
            boidsParams.alignmentCoeff = args.read<float>("alignment", 0.2f);
            boidsParams.cohesionCoeff = args.read<float>("cohesion", 0.2f);
            boidsParams.adoptionRate = args.read<float>("rate", 0.25f);
            return new Boids<D>(boidsParams, params);
        }
        
        std::printf("Invalid model name %s!\n", name.c_str());
        std::exit(1);
    }
};
