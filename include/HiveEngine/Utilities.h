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
    class PhysicalEntity;

    glm::mat3 generate_rotation_matrix(char axis, float angle); // angle is radians


    std::string dvec3_to_str(glm::dvec3 value);
}


#endif //HIVEENGINE_UTILITIES_H
