//
// Created by calper on 5/11/19.
//

#ifndef HIVEENGINE_IMAGEDRAWING_H
#define HIVEENGINE_IMAGEDRAWING_H

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
        int texture_index = -2;
    };

    struct ImageDescription {
        size_t orientation;
        size_t itdesc1;
        size_t itdesc2;
    };

    ImageOrientation create_aligned_image_orientation(glm::vec3 position, float width, float height);

    class ImageDrawing : public Drawing {
    public:
        HiveEngine::Buffer<HiveEngine::Texture> textures; // Textures
        HiveEngine::Buffer<ImageOrientation> imos; // Orientations
        HiveEngine::Buffer<ImageTriangleDescription> imtds; // Descriptions

        VmaAllocation orientation_allocation = nullptr;
        VkBuffer orientation_buffer = nullptr;

        VmaAllocation description_allocation = nullptr;
        VkBuffer description_buffer = nullptr;

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

        ImageDrawing(Directive *directive, HiveEngine::Texture texture);

        ~ImageDrawing();

        void init(VkRenderPass render_pass) override;

        void update();

        void draw(VkCommandBuffer cmd_buffer) override;

        void cleanup() override;

        ImageDescription add_image(int texture_index, glm::vec3 position, float width, float height);

        void remove_image(ImageDescription id);
    };
}

#endif //HIVEENGINE_IMAGEDRAWING_H
