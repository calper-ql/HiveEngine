//
// Created by calper on 4/19/19.
//

#include <HiveEngine/RadialGenerator.h>

#define PI 3.14159265359

namespace HiveEngine {
    RadialGenerator::RadialGenerator(double radius) {
        this->radius = radius;
    }

    glm::dvec3 RadialGenerator::generate_vertex(glm::dvec3 v) {
        return v * (radius / glm::length(v));
    }

    glm::dvec3 RadialGenerator::generate_normal(glm::dvec3 v) {
        return glm::normalize(v * (radius / glm::length(v)));
    }

    glm::dvec2 RadialGenerator::generate_uv(glm::dvec3 vec) {
        auto n = -glm::normalize(vec);
        auto u = 0.5 + atan2(n.y, n.x) / (2.0 * PI);
        auto v = 0.5 - asin(-n.z) / PI;
        return {u, v};
    }

    glm::dvec4 RadialGenerator::generate_color(glm::dvec3 v) {
        return glm::dvec4(glm::abs(v / glm::length(v)), 1.0);
    }
}