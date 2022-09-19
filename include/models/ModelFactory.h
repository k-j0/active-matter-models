#pragma once

#include <string>
#include "Arguments.h"
#include "models/Model.h"
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
        std::size_t particleCount = args.read<int>("particles", 512);
        int periodicity = args.read<int>("periodic-size", 500); // -1 for non-periodic domain
        bool startUniformly = args.read<bool>("uniform-start", true);
        unsigned int seed = args.read<int>("seed", 0);
        
        if (name.compare("run-and-tumble") == 0) {
            return new RunAndTumble<D>(particleCount, periodicity, startUniformly, args.read<float>("flip-prob", 0.1f), seed);
        }
        
        if (name.compare("active-brownian") == 0) {
            return new ActiveBrownianMotion<D>(particleCount, periodicity, startUniformly, args.read<float>("angular-diffusion", 0.02f), seed);
        }
        
        if (name.compare("vicsek") == 0) {
            return new Vicsek<D>(particleCount, periodicity, startUniformly, args.read<float>("detection-radius", 25.0f), args.read<float>("angular-diffusion", 0.02f), seed);
        }
        
        if (name.compare("boids") == 0) {
            typename Boids<D>::Params params;
            params.detectionRadius = args.read<float>("detection-radius", 25.0f);
            params.separationRadius = args.read<float>("separation-radius", 5.0f);
            params.angularDiffusion = args.read<float>("angular-diffusion", 0.02f);
            params.separationCoeff = args.read<float>("separation", 1.0f);
            params.alignmentCoeff = args.read<float>("alignment", 0.2f);
            params.cohesionCoeff = args.read<float>("cohesion", 0.2f);
            params.adoptionRate = args.read<float>("rate", 0.25f);
            return new Boids<D>(particleCount, periodicity, startUniformly, params, seed);
        }
        
        std::printf("Invalid model name %s!\n", name.c_str());
        std::exit(1);
    }
};
