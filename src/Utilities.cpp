//
// Created by calper on 4/19/19.
//

#include <HiveEngine/Utilities.h>
#include <HiveEngine/PhysicalEntity.h>
#include <filesystem>

namespace HiveEngine {

    glm::mat3 generate_rotation_matrix(char axis, float angle) {
        glm::mat3 mat(1.0);

        float c = cosf(angle);
        float s = sinf(angle);

        if (axis == 'x' || axis == 'X') {
            mat[1][1] = c;
            mat[2][1] = -s;
            mat[1][2] = s;
            mat[2][2] = c;
        } else if (axis == 'y' || axis == 'Y') {
            mat[0][0] = c;
            mat[2][0] = s;
            mat[0][2] = -s;
            mat[2][2] = c;
        } else if (axis == 'z' || axis == 'Z') {
            mat[0][0] = c;
            mat[0][1] = s;
            mat[1][0] = -s;
            mat[1][1] = c;
        }

        return mat;
    }


    std::string dvec3_to_str(glm::dvec3 value) {
        std::string str;
        str.append("(");
        str.append(std::to_string(value.x));
        str.append(", ");
        str.append(std::to_string(value.y));
        str.append(", ");
        str.append(std::to_string(value.z));
        str.append(")");
        return str;
    }

    glm::mat4 ai_matrix_to_glm(aiMatrix4x4 &from) {
        glm::mat4 to = glm::mat4(1.0);
        to[0][0] = from.a1; to[1][0] = from.a2; to[2][0] = from.a3; to[3][0] = from.a4;
        to[0][1] = from.b1; to[1][1] = from.b2; to[2][1] = from.b3; to[3][1] = from.b4;
        to[0][2] = from.c1; to[1][2] = from.c2; to[2][2] = from.c3; to[3][2] = from.c4;
        to[0][3] = from.d1; to[1][3] = from.d2; to[2][3] = from.d3; to[3][3] = from.d4;
        return to;
    }

    double ai_get_node_radius(aiScene* scene, aiNode *node) {
        double radius = 0.0;

        for (int i = 0; i < node->mNumMeshes; ++i) {
            for (int j = 0; j < scene->mMeshes[node->mMeshes[i]]->mNumVertices; ++j) {
                double n_r = scene->mMeshes[node->mMeshes[i]]->mVertices[j].Length();
                if(n_r > radius) radius = n_r;
            }
        }

        return radius;
    }

    // Thanks Gamasutra! https://www.gamasutra.com/view/feature/131424/pool_hall_lessons_fast_accurate_.php?page=2
    inline bool
    check_deep_coll(glm::dvec3 p1, glm::dvec3 p2, glm::dvec3 v1, glm::dvec3 v2, double r1, double r2, double &t,
                    unsigned ticks_per_second) {
        v1 /= (double) ticks_per_second;
        v2 /= (double) ticks_per_second;

        glm::dvec3 C = p2 - p1;
        double sum_radii = r1 + r2;
        double dist = glm::length(C) - sum_radii;
        glm::dvec3 move_vec = v1 - v2;

        if (glm::length(move_vec) < dist) return false;

        glm::dvec3 N = glm::normalize(move_vec);
        double D = glm::dot(N, C);

        if (D <= 0) return false;

        double length_c = glm::length(C);
        double F = (length_c * length_c) - (D * D);

        double sum_radii_squared = sum_radii * sum_radii;

        if (F >= sum_radii_squared) return false;

        t = sum_radii_squared - F;

        if (t < 0) return false;

        double distance = D - glm::sqrt(t);

        double mag = glm::length(move_vec);

        return mag >= distance;

    }

    std::string mat3_to_str(glm::mat3 value) {
        std::string s;
        s.append(dvec3_to_str(value[0]));
        s.append("\n");
        s.append(dvec3_to_str(value[1]));
        s.append("\n");
        s.append(dvec3_to_str(value[2]));
        s.append("\n");
        return s;
    }

    glm::dvec3 triangle_centroid(glm::dvec3 a, glm::dvec3 b, glm::dvec3 c) {
        return (a+b+c)/3.0;
    }

    double triangle_area(glm::dvec3 a, glm::dvec3 b, glm::dvec3 c) {
        auto e1 = b - a;
        auto e2 = c - a;
        auto e3 = glm::cross(e1, e2);
        return 0.5 * glm::length(e3);
    }

    glm::dvec3 ai_vec3d_to_glm(aiVector3D vec) {
        glm::dvec3 rval;
        rval.x = vec.x;
        rval.y = vec.y;
        rval.z = vec.z;
        return rval;
    }

    double ai_mesh_area(aiMesh *mesh) {
        double area = 0;
        for (int i = 0; i < mesh->mNumFaces; ++i) {
            auto face = mesh->mFaces[i];
            if(face.mNumIndices == 3){
                glm::dvec3 a = ai_vec3d_to_glm(mesh->mVertices[face.mIndices[0]]);
                glm::dvec3 b = ai_vec3d_to_glm(mesh->mVertices[face.mIndices[0]]);
                glm::dvec3 c = ai_vec3d_to_glm(mesh->mVertices[face.mIndices[0]]);
                area += triangle_area(a, b, c);
            }
        }
        return area;
    }

    glm::mat3 ai_mesh_moment_of_inertia(aiMesh* mesh){
        double total_area = ai_mesh_area(mesh);

        glm::mat3 moi = {};

        for (int i = 0; i < mesh->mNumFaces; ++i) {
            auto face = mesh->mFaces[i];
            if(face.mNumIndices == 3){
                glm::dvec3 a = ai_vec3d_to_glm(mesh->mVertices[face.mIndices[0]]);
                glm::dvec3 b = ai_vec3d_to_glm(mesh->mVertices[face.mIndices[0]]);
                glm::dvec3 c = ai_vec3d_to_glm(mesh->mVertices[face.mIndices[0]]);
                glm::dvec3 centroid = triangle_centroid(a, b, c);
                auto area = triangle_area(a, b, c);
                auto area_ratio = area/total_area;

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

        return moi;

    }

    std::string search_path(std::string folder_path, std::string filename){
        
        return "";
    }
}
