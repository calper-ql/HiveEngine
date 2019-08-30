//
// Created by calper on 5/11/19.
//

#include <HiveEngine/Renderer/Camera.h>
#include <HiveEngine/HiveEngine.h>
#include <iostream>

namespace HiveEngine::Renderer {

    float get_window_ratio(GLFWwindow *window) {
        int width;
        int height;
        glfwGetWindowSize(window, &width, &height);

        return width / (float) height;
    }

    Camera::Camera() {
        orientation = glm::quat(glm::vec3(0, 0, 0));
        set_perspective(PI_HALF, 1, 0.1, 1e12);
        traverse_modifier = 0.001;
        rotate_modifier = 1000.0;
    }

    void Camera::set_perspective(float fov, float ratio, float near_, float far_) {
        this->fov = fov;
        this->ratio = ratio;
        this->near_ = near_;
        this->far_ = far_;
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
        CameraPackage cp = get_package_no_perspective();
        cp.view = perspective * cp.view;
        return cp;
    }

    CameraPackage Camera::get_package_no_perspective(){
        this->perspective = glm::perspective(fov, ratio, near_, far_);
        CameraPackage cp = {};
        cp.apply = apply;
        cp.fov = fov;
        cp.view = glm::translate(glm::mat4_cast(orientation), position);
        cp.view_rot = glm::mat3_cast(orientation);
        cp.pos = position;
        cp.near_ = near_;
        cp.far_ = far_;
        return cp;
    }

    void Camera::get_user_input(GLFWwindow *window) {
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
            position += glm::dvec3(glm::row(glm::mat4_cast(orientation), 2)) * traverse_modifier;
        }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
            position -= glm::dvec3(glm::row(glm::mat4_cast(orientation), 2)) * traverse_modifier;
        }
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
            position += glm::dvec3(glm::row(glm::mat4_cast(orientation), 0)) * traverse_modifier;
        }
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
            position -= glm::dvec3(glm::row(glm::mat4_cast(orientation), 0)) * traverse_modifier;
        }
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
            position += glm::dvec3(glm::row(glm::mat4_cast(orientation), 1)) * traverse_modifier;
        }
        if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
            position -= glm::dvec3(glm::row(glm::mat4_cast(orientation), 1)) * traverse_modifier;
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
            if (m_press <= 4) {
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

        if(__mouse_wheel > 0.0) {
            if(glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS){
                fov *= 1.01;
            } else {
                traverse_modifier *=1.1;
            }
            __mouse_wheel = 0.0;
        }
        if(__mouse_wheel < 0.0) {
            if(glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS){
                fov *= 0.99;
            } else {
                traverse_modifier *= 0.9;
            }
            __mouse_wheel = 0.0;
        }

    }

    void Camera::set_ratio(float ratio) {
        this->ratio = ratio;
        this->perspective = glm::perspective(fov, ratio, near_, far_);
    }

    void Camera::enable() {apply = 1;}
    void Camera::disable() {apply = 0;}

    void Camera::set_as_mouse_wheel_callback(GLFWwindow *window) {
        glfwSetWindowUserPointer(window, this);
        glfwSetScrollCallback(window, camera_scroll_callback);
    }

    float Camera::get_fov() {
        return fov;
    }

    void camera_scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
        ((Camera*)glfwGetWindowUserPointer(window))->__mouse_wheel = yoffset;
    }

}