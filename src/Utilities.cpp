//
// Created by calper on 4/19/19.
//

#include <HiveEngine/Utilities.h>
#include <HiveEngine/PhysicalEntity.h>

namespace HiveEngine {

    glm::mat3 generate_rotation_matrix(char axis, float angle) {
        glm::mat3 mat(1.0);
        //mat[0][0] = 1.0f; mat[0][1] = 0.0f; mat[0][2] = 0.0f;
        //mat[1][0] = 0.0f; mat[1][1] = 1.0f; mat[1][2] = 0.0f;
        //mat[2][0] = 0.0f; mat[2][1] = 0.0f; mat[2][2] = 1.0f;

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

}