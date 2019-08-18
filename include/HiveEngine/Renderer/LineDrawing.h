//
// Created by calper on 5/7/19.
//

#ifndef HIVEENGINE_LINEDRAWING_H
#define HIVEENGINE_LINEDRAWING_H

#include <HiveEngine/Renderer/Drawing.h>
#include <HiveEngine/Buffer.hpp>
#include <HiveEngine/Renderer/Camera.h>
#include <HiveEngine/Renderer/GPUBuffer.hpp>
#include <HiveEngine/Common.h>

namespace HiveEngine::Renderer {

    class LineDrawing : public Drawing {
    public:
        Buffer<HiveEngine::Line> line_buffer;
        GPUBuffer<Line> line_gpu_buffer;

        Buffer<CameraPackage> camera_buffer;
        GPUBuffer<CameraPackage> camera_gpu_buffer;

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

    class TargetMarker {
    public:
        LineDrawing* ld;
        std::vector<LineDescription> descs;

        explicit TargetMarker(LineDrawing* ld){ this->ld = ld; }
        ~TargetMarker(){ for(auto desc: descs) ld->remove_line(desc); }

        void set(glm::dvec3 pos, double radius, glm::vec4 color){
            for(auto desc: descs) ld->remove_line(desc);
            descs.clear();

            std::vector<Line> lines;
            auto r = radius;

            Line a, b, c;
            a.a.color = color;
            a.b.color = color;
            a.a.position= pos + glm::dvec3(r, 0, 0);
            a.b.position= pos + glm::dvec3(-r, 0, 0);
            b.a.color = color;
            b.b.color = color;
            b.a.position= pos + glm::dvec3(0, r, 0);
            b.b.position= pos + glm::dvec3(0, -r, 0);
            c.a.color = color;
            c.b.color = color;
            c.a.position= pos + glm::dvec3(0, 0, r);
            c.b.position= pos + glm::dvec3(0, 0, -r);


            descs.push_back(ld->add_line(a));
            descs.push_back(ld->add_line(b));
            descs.push_back(ld->add_line(c));
        }
    };
}

#endif //HIVEENGINE_LINEDRAWING_H
