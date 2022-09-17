
#include "Arguments.h"
#include "Model.h"

int main (int argc, char** argv) {
    
    std::string modelName;
    {
        Arguments args(argc, argv);
        modelName = args.read<std::string>("model");
    }
    
    Model* model = nullptr;
    
    
    
    if (model) delete model;
    return 0;
}
