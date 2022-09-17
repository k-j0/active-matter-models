#pragma once

#include <string>
#include "Arguments.h"
#include "Model.h"
#include "RunAndTumble.h"
#include "ActiveBrownianMotion.h"
#include "Vicsek.h"

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
        unsigned int seed = args.read<int>("seed", 0);
        
        if (name.compare("run-and-tumble") == 0) {
            return new RunAndTumble<D>(particleCount, periodicity, args.read<float>("flip-prob", 0.1f), seed);
        }
        
        if (name.compare("active-brownian") == 0) {
            return new ActiveBrownianMotion<D>(particleCount, periodicity, args.read<float>("angular-diffusion", 0.02f), seed);
        }
        
        if (name.compare("vicsek") == 0) {
            return new Vicsek<D>(particleCount, periodicity, args.read<float>("detection-radius", 25.0f), args.read<float>("angular-diffusion", 0.02f), seed);
        }
        
        std::printf("Invalid model name %s!\n", name.c_str());
        std::exit(1);
    }
};
