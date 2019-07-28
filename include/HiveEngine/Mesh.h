//
// Created by calper on 4/20/19.
//

#ifndef HIVEENGINE_MESH_H
#define HIVEENGINE_MESH_H

#include <vector>
#include <glm/glm.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <HiveEngine/Texture.h>

namespace HiveEngine {
    class Mesh {
    public:
        Texture texture;

        std::vector<glm::vec3> vertices;
        std::vector<glm::vec3> normals;
        std::vector<glm::vec2> uvs;

        std::vector<glm::uvec3> vertex_indices;
        std::vector<glm::uvec3> normal_indices;
        std::vector<glm::uvec3> uv_indices;

        int material_index;

    public:



    private:

    };
}

#endif //HIVEENGINE_MESH_H
