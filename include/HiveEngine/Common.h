//
// Created by calper on 4/19/19.
//

#ifndef HIVEENGINE_COMMON_H
#define HIVEENGINE_COMMON_H

#include <glm/glm.hpp>
#include <array>
#include <vector>
#include <assimp/mesh.h>

namespace HiveEngine {
    struct Point {
        glm::dvec3 position = {0.0, 0.0, 0.0};
        glm::vec4 color = {0.0, 0.0, 0.0, 0.0};
    };

    struct Line {
        Point a;
        Point b;
    };

    struct LineDescription {
        size_t id;
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

    class MassData {
    public:
        bool recalculate = true;
        glm::dvec3 position = {};
        double mass = 0.0;
        glm::dmat3 moment_of_inertia = {};
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
