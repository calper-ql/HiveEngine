//
// Created by calper on 4/19/19.
//

#ifndef HIVEENGINE_UTILITIES_H
#define HIVEENGINE_UTILITIES_H

#include <glm/glm.hpp>
#include <cmath>
#include <vector>
#include <string>
#include <map>

#include <HiveEngine/Common.h>

#include <assimp/scene.h>

namespace HiveEngine {
    glm::dmat3 generate_rotation_matrix(char axis, float angle); // angle is radians

    // https://stackoverflow.com/questions/29184311/how-to-rotate-a-skinned-models-bones-in-c-using-assimp
    glm::mat4 ai_matrix_to_glm(aiMatrix4x4& from);

    double ai_get_node_radius(aiScene* scene, aiNode* node);

    std::string dvec3_to_str(glm::dvec3 value);

    std::string mat3_to_str(glm::mat3 value);

    bool check_deep_coll(glm::dvec3 p1, glm::dvec3 p2, glm::dvec3 v1, glm::dvec3 v2, double r1, double r2, double &t,
                    unsigned ticks_per_second);

    glm::dvec3 triangle_centroid(glm::dvec3 a, glm::dvec3 b, glm::dvec3 c);

    double triangle_area(glm::dvec3 a, glm::dvec3 b, glm::dvec3 c);

    double ai_mesh_area(aiScene* scene, aiMesh* mesh);

    glm::dvec3 ai_vec3d_to_glm(aiVector3D vec);

    glm::mat3 ai_mesh_moment_of_inertia(aiMesh* mesh);

    std::string search_path(std::string folder_path, std::string filename);

    std::vector<Line> generate_grid_lines_basic(unsigned count);

    std::vector<Line> generate_target_mark(glm::dvec3 pos, double radius, glm::vec4 color);

    glm::dmat3 calculate_moment_of_inertia(glm::dvec3 point, double mass);
}


#endif //HIVEENGINE_UTILITIES_H
