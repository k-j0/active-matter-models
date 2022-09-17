#pragma once

#include <string>
#include "Model.h"

class ModelFactory {
    ModelFactory()=delete;
    ModelFactory(const ModelFactory&)=delete;
    ModelFactory(ModelFactory&&)=delete;
public:
    
    static Model* build(std::string name, std::size_t particleCount, unsigned int seed) {
        std::printf("Invalid model name %s!\n", name.c_str());
        std::exit(1);
    }
};
