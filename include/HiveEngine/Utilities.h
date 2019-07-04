//
// Created by calper on 4/19/19.
//

#ifndef HIVEENGINE_UTILITIES_H
#define HIVEENGINE_UTILITIES_H

#include <glm/glm.hpp>
#include <cmath>
#include <vector>
#include <string>

#include <assimp/scene.h>

namespace HiveEngine {
    glm::mat3 generate_rotation_matrix(char axis, float angle); // angle is radians

    // https://stackoverflow.com/questions/29184311/how-to-rotate-a-skinned-models-bones-in-c-using-assimp
    glm::mat4 ai_matrix_to_glm(aiMatrix4x4* from);

    double ai_get_node_radius(aiScene* scene, aiNode* node);

    std::string dvec3_to_str(glm::dvec3 value);


}


#endif //HIVEENGINE_UTILITIES_H
