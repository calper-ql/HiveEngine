//
// Created by calper on 4/19/19.
//

#include <HiveEngine/Utilities.h>
#include <HiveEngine/PhysicalEntity.h>

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

    glm::mat4 ai_matrix_to_glm(aiMatrix4x4 *from) {

        glm::mat4 to;
        to[0][0] = (float) from->a1;
        to[0][1] = (float) from->b1;
        to[0][2] = (float) from->c1;
        to[0][3] = (float) from->d1;
        to[1][0] = (float) from->a2;
        to[1][1] = (float) from->b2;
        to[1][2] = (float) from->c2;
        to[1][3] = (float) from->d2;
        to[2][0] = (float) from->a3;
        to[2][1] = (float) from->b3;
        to[2][2] = (float) from->c3;
        to[2][3] = (float) from->d3;
        to[3][0] = (float) from->a4;
        to[3][1] = (float) from->b4;
        to[3][2] = (float) from->c4;
        to[3][3] = (float) from->d4;
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

}