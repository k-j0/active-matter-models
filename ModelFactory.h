#pragma once

#include <string>
#include "Arguments.h"
#include "Model.h"
#include "RunAndTumble.h"
#include "ActiveBrownianMotion.h"

class ModelFactory {
    ModelFactory()=delete;
    ModelFactory(const ModelFactory&)=delete;
    ModelFactory(ModelFactory&&)=delete;
public:
    
    template<int D>
    static Model* build(Arguments& args) {
        
        std::string name = args.read<std::string>("model");
        std::size_t particleCount = args.read<int>("particles", 512);
        unsigned int seed = args.read<int>("seed", 0);
        
        if (name.compare("run-and-tumble") == 0) {
            return new RunAndTumble<D>(particleCount, args.read<float>("flip-prob", 0.1f), seed);
        }
        
        if (name.compare("active-brownian") == 0) {
            return new ActiveBrownianMotion<D>(particleCount, args.read<float>("angular-diffusion", 0.1f), seed);
        }
        
        std::printf("Invalid model name %s!\n", name.c_str());
        std::exit(1);
    }
};
