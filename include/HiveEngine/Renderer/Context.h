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
#include <optional>
#include <set>

namespace HiveEngineRenderer {
    const int WIDTH = 800;
    const int HEIGHT = 600;

    struct QueueFamilyIndices {
        std::optional<uint32_t> graphics_family;
        std::optional<uint32_t> present_family;

        bool is_complete() {
            return graphics_family.has_value() && present_family.has_value();
        }
    };

    struct SwapChainSupportDetails {
        VkSurfaceCapabilitiesKHR capabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> presentModes;
    };


    class Context {
    public:
        std::vector<const char*> validation_layers = {"VK_LAYER_LUNARG_standard_validation"};
        const std::vector<const char*> deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
        };

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
        VkPhysicalDevice physical_device = VK_NULL_HANDLE;
        VkDevice device = VK_NULL_HANDLE;
        VkQueue graphics_queue;
        VkQueue present_queue;
        VkSurfaceKHR surface;



        void init_window();
        void init_vulkan();
        void main_loop();
        void cleanup();
        void create_instance();
        bool check_validation_layer_support();
        void setup_debug_messenger();
        void pick_physical_device();
        void create_logical_device();
        void create_surface();

    };
}

#endif //HIVEENGINE_RENDERER_CONTEXT_H
