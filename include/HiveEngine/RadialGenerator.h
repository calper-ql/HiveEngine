//
// Created by calper on 4/19/19.
//

#ifndef HIVEENGINE_RADIALGENERATOR_H
#define HIVEENGINE_RADIALGENERATOR_H

#include <glm/glm.hpp>

namespace HiveEngine {
    class RadialGenerator {
    public:
        double radius;

        explicit RadialGenerator(double radius);

        virtual glm::dvec3 generate_vertex(glm::dvec3 v);

        virtual glm::dvec3 generate_normal(glm::dvec3 v);

        virtual glm::dvec2 generate_uv(glm::dvec3 v);

        virtual glm::dvec4 generate_color(glm::dvec3 v);
    };
}


#endif //HIVEENGINE_RADIALGENERATOR_H
