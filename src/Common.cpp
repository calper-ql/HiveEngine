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
        return !(max.z < other.min.z || min.z > other.max.z);
    }

    DAABB::DAABB() {
        min = glm::dvec3(0.0f, 0.0f, 0.0f);
        max = glm::dvec3(0.0f, 0.0f, 0.0f);
    }

    bool DAABB::collides(DAABB other) {
        if (max.x < other.min.x || min.x > other.max.x) return false;
        if (max.y < other.min.y || min.y > other.max.y) return false;
        return !(max.z < other.min.z || min.z > other.max.z);
    }

    DAABB_LINES DAABB::to_lines(glm::vec3 offset, glm::vec4 color) {
        DAABB_LINES lines;

        /////////////////////////////////////////////////
        lines.data[0].a.position = {min.x, min.y, min.z};
        lines.data[0].b.position = {max.x, min.y, min.z};

        lines.data[1].a.position = {min.x, max.y, min.z};
        lines.data[1].b.position = {max.x, max.y, min.z};

        lines.data[2].a.position = {min.x, min.y, max.z};
        lines.data[2].b.position = {max.x, min.y, max.z};

        lines.data[3].a.position = {min.x, max.y, max.z};
        lines.data[3].b.position = {max.x, max.y, max.z};


        /////////////////////////////////////////////////
        lines.data[4].a.position = {min.x, min.y, min.z};
        lines.data[4].b.position = {min.x, max.y, min.z};

        lines.data[5].a.position = {max.x, min.y, min.z};
        lines.data[5].b.position = {max.x, max.y, min.z};

        lines.data[6].a.position = {min.x, min.y, max.z};
        lines.data[6].b.position = {min.x, max.y, max.z};

        lines.data[7].a.position = {max.x, min.y, max.z};
        lines.data[7].b.position = {max.x, max.y, max.z};


        /////////////////////////////////////////////////
        lines.data[8].a.position = {min.x, min.y, min.z};
        lines.data[8].b.position = {min.x, min.y, max.z};

        lines.data[9].a.position = {min.x, max.y, min.z};
        lines.data[9].b.position = {min.x, max.y, max.z};

        lines.data[10].a.position = {max.x, min.y, min.z};
        lines.data[10].b.position = {max.x, min.y, max.z};

        lines.data[11].a.position = {max.x, max.y, min.z};
        lines.data[11].b.position = {max.x, max.y, max.z};

        for (auto & i : lines.data) {
            i.a.color = color;
            i.b.color = color;
            i.a.position += offset;
            i.b.position += offset;
        }

        return lines;
    }
}