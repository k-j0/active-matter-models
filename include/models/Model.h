#pragma once

#include <random>
#include <functional>
#include <thread>
#include "Vec.h"
#include "VecUtils.h"
#include "BinaryIO.h"

// Base, non-templated class exposing the APIs available to operate on the various models
class ModelBase {
    
    unsigned int seed;
    
protected:
    // thread-safe random number generator
    // note: depending on inconsistencies in thread scheduling, re-using the same seed might still produce different results even on the same hardware
    template<typename distribution_t = std::uniform_real_distribution<float>>
    float rand (distribution_t distribution) {
        static thread_local std::mt19937 gen(seed + std::hash<std::thread::id>{}(std::this_thread::get_id()));
        return distribution(gen);
    }
    
    float rand01 () {
        return rand(std::uniform_real_distribution(0.0f, 1.0f));
    }
    
public:
    ModelBase (unsigned int seed) : seed(seed) { }
    
    virtual ~ModelBase () { }
    virtual void update () = 0;
    virtual float getMSD () = 0;
    virtual void print () = 0;
    virtual void toBinary (std::vector<std::uint8_t>& data) = 0;
    
};


template<int D>
struct Particle {
    Vec<D> pos;
    Vec<D-1> rotation; // in 2D, theta; in 3D theta (lat) and phi (long)
};

// Templated base class from which the common models can derive, containing the particle data
template<int D>
class Model : public ModelBase {
    
public:

    struct Params {
        std::size_t particleCount = 1024;
        float periodicity = 500;
        bool startUniformly = true;
        unsigned int seed = 0;
    };
    
protected:
    std::size_t particleCount;
    std::vector<Particle<D>> particles;
    float periodicity; // negative to disable periodic domain
    
    Vec<D> randomLocation (float size);
    Vec<D-1> randomRotation ();
    
    virtual std::string getName () = 0;
    virtual void updateParticle (std::size_t i) = 0;
    
public:
    
    Model (Params params) : ModelBase(params.seed), particleCount(params.particleCount), periodicity(params.periodicity) {
        particles.reserve(particleCount);
        for (std::size_t i = 0; i < particleCount; ++i) {
            particles.push_back({
                params.startUniformly ? randomLocation(periodicity < 0 ? 500.0f : periodicity) : Vec<D>::Zero(),
                randomRotation()
            });
        }
    }
    
    virtual ~Model () { }
    
    virtual float getMSD () override;
    virtual void print () override;
    virtual void update () override;
    void toBinary (std::vector<std::uint8_t>& data) override;
    
};


template<>
Vec<2> Model<2>::randomLocation (float size) {
    return {
        (rand01() * 2 - 1.0f) * size,
        (rand01() * 2 - 1.0f) * size
    };
}

template<>
Vec<1> Model<2>::randomRotation () {
    return { rand01() * PI * 2.0f };
}

template<>
Vec<3> Model<3>::randomLocation (float size) {
    return {
        (rand01() * 2 - 1.0f) * size,
        (rand01() * 2 - 1.0f) * size,
        (rand01() * 2 - 1.0f) * size
    };
}

template<>
Vec<2> Model<3>::randomRotation () {
    return {
        rand01() * PI * 2.0f,
        rand01() * PI
    };
}

template<int D>
float Model<D>::getMSD () {
    float sum = 0.0f;
    
    #pragma omp parallel for reduction(+: sum)
    for (std::size_t i = 0; i < particleCount; ++i) {
        sum += particles[i].pos.lengthSqr();
    }
    
    return sum / particleCount;
}

template<int D>
void Model<D>::print () {
    std::printf("%s, %ld particles:\n", getName().c_str(), particleCount);
    for (std::size_t i = 0, sz = particleCount > 10 ? 10 : particleCount; i < sz; ++i) {
        std::printf("\t- (%ld) %s\n", i, particles[i].pos.toString().c_str());
    }
    if (particleCount > 10) std::printf("\t- ... (only first 10 shown)\n");
    std::printf("MSD: %f\n", getMSD());
    std::printf("\n");
}

template<int D>
void Model<D>::update () {
    #pragma omp parallel for
    for (std::size_t i = 0; i < particleCount; ++i) {
        
        // update single particle
        updateParticle(i);
        
        // ensure periodic domain
        if (periodicity > 0) {
            particles[i].pos.periodic(periodicity);
        }
    }
}

template<int D>
void Model<D>::toBinary (std::vector<std::uint8_t>& data) {
    
    // Header
    data.push_back('A');
    data.push_back('M');
    data.push_back('M');
    
    // Write the number of particles and the dimension
    BinIO::writeSimple<std::int32_t>(data, particleCount);
    BinIO::writeSimple<std::int32_t>(data, D);
    
    for (std::size_t i = 0; i < particleCount; ++i) {
        // for each particle, write the position and direction vectors
        BinIO::writeVec(data, particles[i].pos);
        BinIO::writeVec(data, VecUtils::toCartesian<D>(particles[i].rotation));
    }
    
    // Footer (not strictly required, but can help ensure the data read was valid)
    data.push_back(0);
    
}
