
#include "Arguments.h"
#include "ModelFactory.h"

int main (int argc, char** argv) {
    
    // Read console args
    Model* model = nullptr;
    std::size_t iterations;
    {
        Arguments args(argc, argv);
        model = ModelFactory::build(args);
        iterations = args.read<int>("iter", 1000);
    }
    
    std::printf("Starting...\n\n");
    
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
