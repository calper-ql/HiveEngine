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
#include <string>
#include <filesystem>
#include <fstream>
#include <map>

#include <HiveEngine/Renderer/vk_mem_alloc.h>

namespace HiveEngineRenderer {

    class Directive;

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

        void run(){
            init_window();
            init_vulkan();
            main_loop();
            cleanup();
        }

        void register_directive(Directive* directive);
        VkFormat get_image_format();
        VkDevice get_device();
        std::map<std::string, VkShaderModule> get_shaders();
        VkExtent2D get_swap_chain_extent();
        std::vector<VkImageView> get_swap_chain_image_views();


        VkCommandPool get_command_pool();
        VmaAllocator get_allocator();

        void mark_resized();

    private:
        GLFWwindow* window;

        VkInstance instance;
        VkDebugUtilsMessengerEXT debug_messenger;
        VkSurfaceKHR surface;

        VkPhysicalDevice physical_device = VK_NULL_HANDLE;
        VkDevice device;

        VkQueue graphics_queue;
        VkQueue present_queue;

        SwapChainSupportDetails details;

        VkSwapchainKHR swap_chain;
        std::vector<VkImage> swap_chain_images;
        VkFormat swap_chain_image_format;
        VkExtent2D swap_chain_exent;

        std::vector<VkImageView> swap_chain_image_views;

        std::map<std::string, VkShaderModule> shaders;

        std::vector<Directive*> directives;
        VkCommandPool command_pool;

        uint8_t inflight_frame_count = 2;
        size_t currentFrame = 0;
        std::vector<VkSemaphore> imageAvailableSemaphores;
        std::vector<VkSemaphore> renderFinishedSemaphores;
        std::vector<VkFence> inFlightFences;

        VmaAllocator allocator;

        bool frame_buffer_resized;
        bool inited = false;

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
        void create_swap_chain();
        void create_image_views();
        void create_allocator();
        void load_shaders();
        void init_directives_frame_buffer();
        void create_command_pool();
        void init_directives_command_buffer();
        void create_sync_objects();

        void draw_frame();
        void recreate_swap_chain();
        void cleanup_swap_chain();
    };
}

#endif //HIVEENGINE_RENDERER_CONTEXT_H
