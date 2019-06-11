//
// Created by calper on 4/19/19.
//

#include <HiveEngine/Common.h>

namespace HiveEngine {
    // FORCE CONSTRUCTOR
    Force::Force(const glm::dvec3 &leverage, const glm::dvec3 &force, bool is_relative) : leverage(leverage),
                                                                                          force(force),
                                                                                          is_relative(is_relative) {}

    Force::Force() {
        leverage = glm::dvec3(0.0f, 0.0f, 0.0f);
        force = glm::dvec3(0.0f, 0.0f, 0.0f);
        is_relative = false;
    }

    EntityStepOutput::EntityStepOutput() {
        force = glm::dvec3(0.0f, 0.0f, 0.0f);
        torque = glm::dvec3(0.0f, 0.0f, 0.0f);
        moment_of_inertia = glm::dmat3(1.0f);
        central_mass = glm::dvec3(0.0f, 0.0f, 0.0f);
        mass = 0.0f;
    }

    CentralMass::CentralMass() {
        position = glm::dvec3(0.0f, 0.0f, 0.0f);
        mass = 0.0f;
        moment_of_inertia = glm::dmat3(0.0f);
    }

    AABB::AABB() {
        min = glm::dvec3(0.0f, 0.0f, 0.0f);
        max = glm::dvec3(0.0f, 0.0f, 0.0f);
    }

    bool AABB::collides(AABB other) {
        if (max.x < other.min.x || min.x > other.max.x) return false;
        if (max.y < other.min.y || min.y > other.max.y) return false;
        if (max.z < other.min.z || min.z > other.max.z) return false;
        return true;
    }
}