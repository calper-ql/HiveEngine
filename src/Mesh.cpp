//
// Created by calpe on 7/31/2019.
//

#include <HiveEngine/Mesh.h>
#include <HiveEngine/Utilities.h>

namespace HiveEngine {

    double Mesh::calculate_surface_area() {
        double total = 0;

        for (int i = 0; i < indices.size(); ++i) {
            auto t_area = triangle_area(vertices[indices[i].x], vertices[indices[i].y], vertices[indices[i].z]);
            total += t_area;
        }

        return 0;
    }

    Mesh::Mesh() {

    }

    Mesh::Mesh(aiScene* scene, aiMesh *ai_mesh) {
        from_ai_mesh(scene, ai_mesh);
    }

    void Mesh::from_ai_mesh(aiScene* scene, aiMesh *ai_mesh) {
        if(nullptr)
            return;

        vertices.clear();
        normals.clear();
        uvs.clear();
        indices.clear();

        vertices.resize(ai_mesh->mNumVertices);
        normals.resize(ai_mesh->mNumVertices);
        uvs.resize(ai_mesh->mNumVertices);
        for (int i = 0; i < ai_mesh->mNumVertices; ++i) {
            vertices[i] = ai_vec3d_to_glm(ai_mesh->mVertices[i]);
            normals[i] = ai_vec3d_to_glm(ai_mesh->mNormals[i]);
            if(ai_mesh->mTextureCoords[0])
                uvs[i] = {ai_mesh->mTextureCoords[0][i].x, ai_mesh->mTextureCoords[0][i].y, ai_mesh->mTextureCoords[0][i].z};
        }

        indices.resize(ai_mesh->mNumFaces);
        for (int i = 0; i < ai_mesh->mNumFaces; ++i) {
            if(ai_mesh->mFaces[i].mNumIndices == 3){
                indices[i] = {ai_mesh->mFaces[i].mIndices[0], ai_mesh->mFaces[i].mIndices[1], ai_mesh->mFaces[i].mIndices[2]};
            }
        }

        __surface_area_value = calculate_surface_area();
        calculate_moment_of_inertia();
    }

    double Mesh::surface_area(bool recalculate) {
        if(recalculate) __surface_area_value = calculate_surface_area();
        return __surface_area_value;
    }

    void Mesh::calculate_moment_of_inertia() {
        double total_area = surface_area();

        glm::mat3 moi = {};
        glm::dvec3 com = {};

        for (auto face : indices) {
            glm::dvec3 a = vertices[face.x];
            glm::dvec3 b = vertices[face.y];
            glm::dvec3 c = vertices[face.z];
            glm::dvec3 centroid = triangle_centroid(a, b, c);
            auto area = triangle_area(a, b, c);
            auto area_ratio = area/total_area;

            com += centroid * (area_ratio);

            moi[0][0] += (area_ratio) * ((centroid.y * centroid.y) + (centroid.z * centroid.z));
            moi[1][1] += (area_ratio) * ((centroid.x * centroid.x) + (centroid.z * centroid.z));
            moi[2][2] += (area_ratio) * ((centroid.x * centroid.x) + (centroid.y * centroid.y));

            moi[0][1] = -area_ratio * centroid.x * centroid.y;
            moi[1][0] = -area_ratio * centroid.x * centroid.y;

            moi[0][2] = -area_ratio * centroid.x * centroid.z;
            moi[2][0] = -area_ratio * centroid.x * centroid.z;

            moi[1][2] = -area_ratio * centroid.y * centroid.z;
            moi[2][1] = -area_ratio * centroid.y * centroid.z;
        }

    }

    glm::mat3 Mesh::get_moment_of_inertia() {
        return moment_of_inertia;
    }

    glm::dvec3 Mesh::get_center_of_mass() {
        return center_of_mass;
    }

    std::vector<Mesh> ai_scene_to_meshes(aiScene *scene) {
        std::vector<Mesh> meshes;
        for (int i = 0; i < scene->mNumMeshes; ++i) {
            meshes.emplace_back(scene, scene->mMeshes[i]);
        }
        return meshes;
    }
}