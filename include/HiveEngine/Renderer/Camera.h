//
// Created by calper on 5/11/19.
//

#ifndef HIVEENGINE_CAMERA_H
#define HIVEENGINE_CAMERA_H

#include <glm/glm.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_access.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <GLFW/glfw3.h>

namespace HiveEngine::Renderer {
    float get_window_ratio(GLFWwindow *window);

    struct CameraPackage {
        glm::mat4 view;
        int apply = 1;
    };

    class Camera {
    private:
        glm::vec3 position;
        glm::quat orientation;
        glm::mat4 perspective;

        int m_press = 0;
        glm::ivec2 last_m_pos;

    public:
        float traverse_modifier;
        float rotate_modifier;

        Camera();

        void set_perspective(float fov, float ratio, float near, float far);

        void set_position(glm::vec3 position);

        void rotate(float angle, float x, float y, float z);

        void yaw(float angle);

        void pitch(float angle);

        void roll(float angle);

        CameraPackage get_package();

        void get_user_input(GLFWwindow *window);

        void get_user_movement(GLFWwindow *window);
    };

}

#endif //HIVEENGINE_CAMERA_H
