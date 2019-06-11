//
// Created by calper on 5/11/19.
//

#ifndef HIVEENGINE_GLYPHDRAWING_H
#define HIVEENGINE_GLYPHDRAWING_H

#include <HiveEngine/Renderer/Drawing.h>
#include <HiveEngine/Buffer.hpp>
#include <HiveEngine/Texture.h>

namespace HiveEngineRenderer {
    struct ImageOrientation {
        glm::vec3 f0;
        glm::vec2 f0uv;
        glm::vec3 f1;
        glm::vec2 f1uv;
        glm::vec3 f2;
        glm::vec2 f2uv;
        glm::vec3 f3;
        glm::vec2 f3uv;
        glm::vec3 f4;
        glm::vec2 f4uv;
        glm::vec3 f5;
        glm::vec2 f5uv;
    };

    struct ImageTriangleDescription {
        int texture_index = -1;
    };

    struct ImageColorDescription {
        glm::vec4 color = {1.0, 0.0, 1.0, 1.0};
    };

    struct ImageDescription {
        size_t orientation;
        size_t itdesc1;
        size_t itdesc2;
    };

    ImageOrientation create_aligned_image_orientation(glm::vec3 position, float width, float height);

    class GlyphDrawing : public Drawing {
    public:
        HiveEngine::Buffer<ImageOrientation> imos; // Orientations
        HiveEngine::Buffer<ImageTriangleDescription> imtds; // Descriptions
        HiveEngine::Buffer<ImageColorDescription> icds; // Descriptions

        VmaAllocation orientation_allocation = nullptr;
        VkBuffer orientation_buffer = nullptr;

        VmaAllocation description_allocation = nullptr;
        VkBuffer description_buffer = nullptr;

        VmaAllocation color_allocation = nullptr;
        VkBuffer color_buffer = nullptr;

        VkPipeline graphicsPipeline;
        VkPipelineLayout pipelineLayout;

        VkDescriptorPool descriptorPool;
        VkDescriptorSetLayout descriptorSetLayout;
        VkDescriptorSet descriptorSet;

        HiveEngine::Texture texture;
        VkImage textureImage = nullptr;
        VmaAllocation textureAllocation = nullptr;

        VkImageView imageView;
        VkSampler textureSampler;

        bool image_pushed = false;

        GlyphDrawing(Directive *directive, HiveEngine::Texture texture);

        ~GlyphDrawing();

        void init(VkRenderPass render_pass) override;

        void update();

        void draw(VkCommandBuffer cmd_buffer) override;

        void cleanup() override;

        ImageDescription add_image(glm::vec3 position, float width, float height, glm::vec4 color);

        ImageDescription add_image_lower_left(glm::vec3 position, float width, float height, glm::vec4 color);

        void remove_image(ImageDescription id);
    };
}

#endif //HIVEENGINE_GLYPHDRAWING_H
