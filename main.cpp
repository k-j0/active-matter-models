
#include "Arguments.h"
#include "ModelFactory.h"

int main (int argc, char** argv) {
    
    // Read console args
    std::string modelName;
    std::size_t iterations;
    std::size_t particleCount;
    unsigned int seed;
    {
        Arguments args(argc, argv);
        modelName = args.read<std::string>("model");
        iterations = args.read<int>("iter", 1000);
        particleCount = args.read<int>("particles", 512);
        seed = args.read<int>("seed", 0);
    }
    
    std::printf("Starting...\n\n");
    
    Model* model = ModelFactory::build(modelName, particleCount, seed);
    
    // Run the selected model for the given number of timesteps
    for (std::size_t i = 0; i < iterations; ++i) {
        model->update();
        if (i % (iterations / 100) == 0) {
            std::printf("%ld %%...\r", i * 100 / iterations);
            std::fflush(stdout);
        }
    }
    std::printf("100 %%.  \n\n");
    
    delete model;
    return 0;
}
