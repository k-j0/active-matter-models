#pragma once

#include "Vec.h"

// Utilities for handling vectors of specific dimensionality shared amongst the different models
class VecUtils {
    VecUtils()=delete;
    VecUtils(const VecUtils&)=delete;
    VecUtils(VecUtils&&)=delete;
public:
    
    // Converts a (set of) angles to a unit direction vector
    template<int D>
    static Vec<D> toCartesian (const Vec<D-1>& rotation);
    
    // Converts a unit direction vector (assumed normalized!) to a (set of) angles
    template<int D>
    static Vec<D-1> toSpherical (const Vec<D>& direction);
    
};




template<>
Vec<2> VecUtils::toCartesian<2> (const Vec<1>& rotation) {
    return {
        std::cos(rotation.X()),
        std::sin(rotation.X())
    };
}
template<>
Vec<1> VecUtils::toSpherical<2> (const Vec<2>& direction) {
    return { std::atan2(direction.Y(), direction.X()) };
}


template<>
Vec<3> VecUtils::toCartesian<3> (const Vec<2>& rotation) {
    // spherical coordinates (r = 1) to cartesian coordinates
    float theta = rotation.X(); // 0..2pi
    float phi = rotation.Y(); // 0..pi
    float sinPhi = std::sin(phi);
    return {
        std::cos(theta) * sinPhi,
        std::sin(theta) * sinPhi,
        std::cos(phi)
    };
}
template<>
Vec<2> VecUtils::toSpherical<3> (const Vec<3>& direction) {
    return {
        std::atan2(direction.Y(), direction.X()),
        std::acos(direction.Z())
    };
}
