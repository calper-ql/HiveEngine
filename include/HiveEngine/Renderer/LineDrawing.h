//
// Created by calper on 5/7/19.
//

#ifndef HIVEENGINE_LINEDRAWING_H
#define HIVEENGINE_LINEDRAWING_H

#include <HiveEngine/Renderer/Drawing.h>
#include <random>

namespace HiveEngineRenderer {
    class LineDrawing : public Drawing {
    public:
        std::default_random_engine g; // TODO: remove this lol

        std::vector<HiveEngine::Point> points;
        VmaAllocation allocation;
        VkBuffer pointBuffer;
        VkPipeline graphicsPipeline;
        VkPipelineLayout pipelineLayout;

        float line_width = 1;

        LineDrawing(Directive *directive);

        void init(VkRenderPass render_pass) override;

        void update() override;

        void draw(VkCommandBuffer cmd_buffer) override;

        void cleanup() override;
    };
}

#endif //HIVEENGINE_LINEDRAWING_H
