
#include <fstream>
#include "Arguments.h"
#include "ModelFactory.h"

int main (int argc, char** argv) {
    
    // Read console args
    ModelBase* model = nullptr;
    std::size_t iterations;
    std::string outputFile;
    {
        Arguments args(argc, argv);
        unsigned int d = args.read<int>("dim", 2);
        switch (d) {
            case 2: model = ModelFactory::build<2>(args); break;
            case 3: model = ModelFactory::build<3>(args); break;
            default:
                std::printf("Invalid dimension %d, only 2D and 3D are supported.\n", d);
                exit(1);
        }
        iterations = args.read<int>("iter", 1000);
        outputFile = args.read<std::string>("out", "results/out.bin");
    }
    
    std::printf("Starting...\n\n");
    
    // Run the selected model for the given number of timesteps
    std::size_t progressCheck = iterations / 100;
    if (progressCheck == 0) progressCheck = 1;
    std::vector<std::uint8_t> binaryData;
    for (std::size_t i = 0; i < iterations; ++i) {
        model->update();
        model->toBinary(binaryData);
        if (i % progressCheck == 0) {
            std::printf("%ld %%...\r", i * 100 / iterations);
            std::fflush(stdout);
        }
    }
    std::printf("100 %%.  \n\n");
    model->print();
    
    // Export binary results
    model->toBinary(binaryData);
    std::ofstream file(outputFile, std::ios::binary);
	file.write(reinterpret_cast<const char*>(&binaryData[0]), binaryData.size());
	file.close();
    
    delete model;
    return 0;
}
