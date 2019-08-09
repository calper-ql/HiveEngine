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

    AABB::AABB() {
        min = glm::dvec3(0.0f, 0.0f, 0.0f);
        max = glm::dvec3(0.0f, 0.0f, 0.0f);
    }

    bool AABB::collides(AABB other) {
        return(min.x <= other.max.x && max.x >= other.min.x) &&
              (min.y <= other.max.y && max.y >= other.min.y) &&
              (min.z <= other.max.z && max.z >= other.min.z);
    }

    DAABB::DAABB() {
        min = glm::dvec3(0.0f, 0.0f, 0.0f);
        max = glm::dvec3(0.0f, 0.0f, 0.0f);
    }

    bool DAABB::collides(DAABB other) {
		return(min.x <= other.max.x && max.x >= other.min.x) &&
			(min.y <= other.max.y && max.y >= other.min.y) &&
			(min.z <= other.max.z && max.z >= other.min.z);
    }

    void DAABB::add(glm::dvec3 point) {
        if(min.x > point.x) min.x = point.x;
        if(min.y > point.y) min.y = point.y;
        if(min.z > point.z) min.z = point.z;
        if(max.x < point.x) max.x = point.x;
        if(max.y < point.y) max.y = point.y;
        if(max.z < point.z) max.z = point.z;
    }

    void DAABB::combine(DAABB other) {
        if(min.x > other.min.x) min.x = other.min.x;
        if(min.y > other.min.y) min.y = other.min.y;
        if(min.z > other.min.z) min.z = other.min.z;

        if(max.x < other.max.x) max.x = other.max.x;
        if(max.y < other.max.y) max.y = other.max.y;
        if(max.z < other.max.z) max.z = other.max.z;
    }

}