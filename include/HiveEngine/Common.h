//
// Created by calper on 4/19/19.
//

#ifndef HIVEENGINE_COMMON_H
#define HIVEENGINE_COMMON_H

#include <glm/glm.hpp>
#include <array>

namespace HiveEngine {
    struct Point {
        glm::vec3 position = {0.0, 0.0, 0.0};
        glm::vec4 color = {0.0, 0.0, 0.0, 0.0};
    };

    struct Line {
        Point a;
        Point b;
    };

    class Force {
    public:
        glm::dvec3 leverage;
        glm::dvec3 force;
        bool is_relative;

        Force();

        Force(const glm::dvec3 &leverage, const glm::dvec3 &force, bool is_relative);
    };

    class EntityStepOutput {
    public:
        glm::dvec3 force;
        glm::dvec3 torque;
        glm::dmat3 moment_of_inertia;
        glm::dvec3 central_mass;
        double mass;

        EntityStepOutput();
    };

    class CentralMass {
    public:
        glm::dvec3 position;
        double mass;
        glm::dmat3 moment_of_inertia;

        CentralMass();
    };

    class AABB {
    public:
        glm::vec3 min{};
        glm::vec3 max{};

        AABB();

        bool collides(AABB other);
    };

    class DAABB {
    public:
        glm::dvec3 min{};
        glm::dvec3 max{};

        DAABB();

        bool collides(DAABB other);
        void add(glm::dvec3 point);
        void combine(DAABB other);

    };

}


#endif //HIVEENGINE_COMMON_H
