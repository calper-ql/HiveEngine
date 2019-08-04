//
// Created by calper on 5/7/19.
//

#ifndef HIVEENGINE_AABBDRAWING_H
#define HIVEENGINE_AABBDRAWING_H

#include <HiveEngine/Renderer/Drawing.h>
#include <HiveEngine/Common.h>
#include <HiveEngine/Renderer/GPUBuffer.hpp>
#include <HiveEngine/Renderer/Camera.h>

namespace HiveEngine::Renderer {

    class AABBDrawing : public Drawing {
    public:
        Buffer<DAABB> daabb_buffer;
        Buffer<int> significance_buffer;
        Buffer<glm::vec3> proto_box_buffer;
        Buffer<CameraPackage> camera_buffer;

        GPUBuffer<DAABB> daabb_gpu_buffer;
        GPUBuffer<int> significance_gpu_buffer;
        GPUBuffer<glm::vec3> proto_box_gpu_buffer;
        GPUBuffer<CameraPackage> camera_gpu_buffer;
        
        VkPipeline graphicsPipeline;
        VkPipelineLayout pipelineLayout;

        std::array<VkVertexInputBindingDescription, 3> bindingDescriptions = {};
        std::array<VkVertexInputAttributeDescription, 4> attributeDescriptions = {};

        VkDescriptorPool descriptorPool;
        VkDescriptorSetLayout descriptorSetLayout;
        VkDescriptorSet descriptorSet;

        float line_width = 1;

        Camera *camera;

        AABBDrawing(Directive *directive, Camera *camera=nullptr);

        void init(VkRenderPass render_pass) override;

        void update();

        void draw(VkCommandBuffer cmd_buffer) override;

        void cleanup() override;
    };
}

#endif //HIVEENGINE_AABBDRAWING_H
