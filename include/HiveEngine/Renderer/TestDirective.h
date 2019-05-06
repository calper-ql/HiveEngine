//
// Created by calper on 5/3/19.
//

#ifndef HIVEENGINE_TESTDIRECTIVE_H
#define HIVEENGINE_TESTDIRECTIVE_H

#include <HiveEngine/Renderer/Directive.h>

namespace HiveEngineRenderer {
    class TestDirective : public Directive {
    public:
        VkRenderPass renderPass;
        VkPipelineLayout pipelineLayout;
        std::vector<VkFramebuffer> swapChainFramebuffers;
        std::vector<VkCommandBuffer> commandBuffers;
        VkPipeline graphicsPipeline;

        TestDirective(Context* context);
        ~TestDirective() override;
        void init_frame_buffers() override;
        void init_command_buffer() override;
        void cleanup() override;
        std::vector<VkCommandBuffer> get_command_buffer() override;
    };
}

#endif //HIVEENGINE_TESTDIRECTIVE_H
