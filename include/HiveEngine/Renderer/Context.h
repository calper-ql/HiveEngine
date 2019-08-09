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
#include <HiveEngine/Texture.h>

namespace HiveEngine::Renderer {

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

        // VK_LAYER_LUNARG_standard_validation
        std::vector<const char *> validation_layers = {"VK_LAYER_LUNARG_standard_validation"};
        const std::vector<const char *> deviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

        void register_directive(Directive *directive);

        VkFormat get_image_format();

        VkDevice get_device();

        std::map<std::string, VkShaderModule> get_shaders();

        VkExtent2D get_swap_chain_extent();

        std::vector<VkImageView> get_swap_chain_image_views();

        size_t get_swap_chain_size();

        VkCommandPool get_command_pool();

        VmaAllocator get_allocator();

        void mark_resized();

        GLFWwindow *get_window();

        void wait_device();

        void init_window();

        void init_vulkan();

        void main_loop();

        void cleanup();

        void copy_texture_to_image(HiveEngine::Texture texture, VkImage image, VkImageLayout layout);

        void transition_image_layout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);

        VkCommandBuffer begin_instant_command();

        void end_instant_command(VkCommandBuffer cmd);

		void recreate_swap_chain();

        VkImageView get_depth_image_view();

        VkFormat find_depth_format();

    private:
        GLFWwindow *window;

        VkInstance instance;
        VkDebugUtilsMessengerEXT debug_messenger;
        VkSurfaceKHR surface;

        VkPhysicalDevice physical_device = VK_NULL_HANDLE;
        VkDevice device = nullptr;

        VkQueue graphics_queue;
        VkQueue present_queue;

        SwapChainSupportDetails details;

		VkSwapchainKHR swap_chain = nullptr;
        std::vector<VkImage> swap_chain_images;
        VkFormat swap_chain_image_format;
        VkExtent2D swap_chain_exent;

        std::vector<VkImageView> swap_chain_image_views;

        std::map<std::string, VkShaderModule> shaders;

        std::vector<Directive *> directives;
        VkCommandPool command_pool;

        uint8_t inflight_frame_count = 2;
        size_t currentFrame = 0;
        std::vector<VkSemaphore> imageAvailableSemaphores;
        std::vector<VkSemaphore> renderFinishedSemaphores;
        std::vector<VkFence> inFlightFences;

        VkImage depth_image = nullptr;
        VmaAllocation depth_image_allocation = nullptr;
        VkImageView depth_image_view = nullptr;

        VmaAllocator allocator;

        bool frame_buffer_resized;
        bool inited = false;

        void create_instance();

        bool check_validation_layer_support();

        void setup_debug_messenger();

        void pick_physical_device();

        void create_logical_device();

        void create_surface();

        void create_swap_chain();

        void create_image_views();

        void create_allocator();

        void create_depth_resources();

        void load_shaders();

        void init_directives_frame_buffer();

        void create_command_pool();

        void init_directives_command_buffer();

        void create_sync_objects();

        void draw_frame();
        
        void cleanup_swap_chain();

        VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);


    };
}

#endif //HIVEENGINE_RENDERER_CONTEXT_H
