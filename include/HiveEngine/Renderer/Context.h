//
// Created by calper on 5/1/19.
//

#ifndef HIVEENGINE_RENDERER_CONTEXT_H
#define HIVEENGINE_RENDERER_CONTEXT_H

#include <vulkan/vulkan.hpp>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <vector>

namespace HiveEngineRenderer {
    const int WIDTH = 800;
    const int HEIGHT = 600;


    class Context {
    public:
        std::vector<const char*> validation_layers = {"VK_LAYER_LUNARG_standard_validation"};

        void run(){
            init_window();
            init_vulkan();
            main_loop();
            cleanup();
        }

    private:
        VkInstance instance;
        GLFWwindow* window;
        VkDebugUtilsMessengerEXT debug_messenger;

        void init_window();
        void init_vulkan();
        void main_loop();
        void cleanup();
        void create_instance();
        bool check_validation_layer_support();
        void setup_debug_messenger();

    };
}

#endif //HIVEENGINE_RENDERER_CONTEXT_H
