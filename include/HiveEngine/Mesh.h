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
#include <HiveEngine/Common.h>

namespace HiveEngine {

    class Mesh {
    public:
        std::vector<glm::vec3> vertices;
        std::vector<glm::vec3> normals;
        std::vector<glm::vec3> uvs;

        std::vector<glm::uvec3> indices;

        int material_index;

        double calculate_surface_area();
        double __surface_area_value = 0;

        glm::dmat3 moment_of_inertia;
        glm::dvec3 center_of_mass = {};

        void calculate_moment_of_inertia();
    public:
        Mesh();

        Mesh(aiScene* scene, aiMesh* ai_mesh);

        void from_ai_mesh(aiScene* scene, aiMesh* ai_mesh);

        double surface_area(bool recalculate=false);

        glm::dmat3 get_moment_of_inertia();

        glm::dvec3 get_center_of_mass();

    };

    std::vector<Mesh> ai_scene_to_meshes(aiScene* scene);
}

#endif //HIVEENGINE_MESH_H
