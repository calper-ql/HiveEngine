//
// Created by calper on 5/7/19.
//

#ifndef HIVEENGINE_LINEDRAWING_H
#define HIVEENGINE_LINEDRAWING_H

#include <HiveEngine/Renderer/Drawing.h>
#include <HiveEngine/Buffer.hpp>
#include <HiveEngine/Renderer/Camera.h>

namespace HiveEngine::Renderer {
    struct LineDescription {
        size_t id;
    };

    class LineDrawing : public Drawing {
    public:
        HiveEngine::Buffer<HiveEngine::Line> line_buffer;

        VmaAllocation point_allocation = nullptr;
        VkBuffer vk_point_buffer = nullptr;

        VmaAllocation state_allocation = nullptr;
        VkBuffer vk_state_buffer = nullptr;

        VmaAllocation camera_allocation = nullptr;
        VkBuffer vk_camera_buffer = nullptr;

        VkPipeline graphicsPipeline;
        VkPipelineLayout pipelineLayout;

        std::array<VkVertexInputBindingDescription, 1> bindingDescriptions = {};
        std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions = {};

        VkDescriptorPool descriptorPool;
        VkDescriptorSetLayout descriptorSetLayout;
        VkDescriptorSet descriptorSet;

        float line_width = 1;

        Camera *camera;

        LineDrawing(Directive *directive, Camera *camera=nullptr);

        void init(VkRenderPass render_pass) override;

        void update();

        void draw(VkCommandBuffer cmd_buffer) override;

        void cleanup() override;

        LineDescription add_line(glm::vec3 a, glm::vec4 ac, glm::vec3 b, glm::vec4 bc);

        LineDescription add_line(Line line);

        void refresh_line(LineDescription ld, Line new_line);

        void remove_line(LineDescription ld);
    };
}

#endif //HIVEENGINE_LINEDRAWING_H
