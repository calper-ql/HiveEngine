//
// Created by calper on 4/19/19.
//

#ifndef HIVEENGINE_UTILITIES_H
#define HIVEENGINE_UTILITIES_H

#include <glm/glm.hpp>
#include <cmath>
#include <vector>
#include <string>


namespace HiveEngine {
    class Entity;

    glm::dmat3 generate_rotation_matrix(char axis, double angle); // angle is radians
    std::pair<std::vector<glm::vec3>, std::vector<glm::vec3>> generate_entity_line_description(Entity *e,
                                                                                               glm::dvec3 scale); // generates lines for renderer, scales each dim, 0.0 -> 1.0
    std::pair<std::vector<glm::vec3>, std::vector<glm::vec3>>
    generate_target_line_description(glm::dvec3 vec, double radius, glm::dvec3 scale, glm::dvec3 color);

    std::string dvec3_to_str(glm::dvec3 value);
}


#endif //HIVEENGINE_UTILITIES_H
