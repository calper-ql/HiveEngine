//
// Created by calper on 5/11/19.
//

#include <HiveEngine/Renderer/Camera.h>

namespace HiveEngine::Renderer {

    float get_window_ratio(GLFWwindow *window) {
        int width;
        int height;
        glfwGetWindowSize(window, &width, &height);

        return width / (float) height;
    }

    HiveEngine::Renderer::Camera::Camera() {
        orientation = glm::quat(glm::vec3(0, 0, 0));
        set_perspective(90, 1, 0.1, 1000.0);
        traverse_modifier = 0.01;
        rotate_modifier = 1000.0;
    }

    void Camera::set_perspective(float fov, float ratio, float near_, float far_) {
        this->perspective = glm::perspective(fov, ratio, near_, far_);
    }

    void Camera::set_position(glm::vec3 position) {
        this->position = position;
    }

    void Camera::rotate(float angle, float x, float y, float z) {
        glm::quat a = orientation;
        auto b = glm::angleAxis(angle * 1.0f, glm::vec3(x, y, z) * a);
        orientation = a * b;
    }

    void Camera::yaw(float angle) {
        this->rotate(angle, 0.0, 1.0, 0.0);
    }

    void Camera::pitch(float angle) {
        this->rotate(angle, 1.0, 0.0, 0.0);
    }

    void Camera::roll(float angle) {
        this->rotate(angle, 0.0, 0.0, 1.0);
    }

    CameraPackage Camera::get_package() {
        CameraPackage cp = {};
        cp.apply = 1;
        //cp.position = position;
        //cp.view = perspective * glm::translate(glm::mat4_cast(orientation), position);
        cp.view = perspective * glm::translate(glm::mat4_cast(orientation), position);
        return cp;
    }

    void Camera::get_user_input(GLFWwindow *window) {
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
            position += glm::vec3(glm::row(glm::mat4_cast(orientation), 2)) * traverse_modifier;
        }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
            position -= glm::vec3(glm::row(glm::mat4_cast(orientation), 2)) * traverse_modifier;
        }
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
            position += glm::vec3(glm::row(glm::mat4_cast(orientation), 0)) * traverse_modifier;
        }
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
            position -= glm::vec3(glm::row(glm::mat4_cast(orientation), 0)) * traverse_modifier;
        }
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
            position += glm::vec3(glm::row(glm::mat4_cast(orientation), 1)) * traverse_modifier;
        }
        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
            position -= glm::vec3(glm::row(glm::mat4_cast(orientation), 1)) * traverse_modifier;
        }
    }

    void Camera::get_user_movement(GLFWwindow *window) {
        if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
            roll(-1.0 / rotate_modifier);
        }
        if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
            roll(1.0 / rotate_modifier);
        }

        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
            double x, y;
            glfwGetCursorPos(window, &x, &y);
            if (m_press == 0) {
                m_press++;
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            } else {
                double dx = x - last_m_pos.x;
                double dy = y - last_m_pos.y;
                pitch(-dy / rotate_modifier);
                yaw(dx / rotate_modifier);
            }
            last_m_pos.x = x;
            last_m_pos.y = y;
        }
        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            m_press = 0;
        }
    }


}