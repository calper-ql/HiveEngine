//
// Created by calper on 5/3/19.
//

#ifndef HIVEENGINE_STANDARDDIRECTIVE_H
#define HIVEENGINE_STANDARDDIRECTIVE_H

#include <HiveEngine/Renderer/Directive.h>

namespace HiveEngine::Renderer {
    class Drawing;

    class StandardDirective : public Directive {
    public:
        VkRenderPass renderPass;
        std::vector<VkFramebuffer> swapChainFramebuffers;
        std::vector<VkCommandBuffer> commandBuffers;

        StandardDirective(Context *context);

        ~StandardDirective() override;

        void init_frame_buffers() override;

        void init_command_buffer() override;

        void cleanup() override;

        VkCommandBuffer get_command_buffer(uint8_t idx) override;

    };
}

#endif //HIVEENGINE_STANDARDDIRECTIVE_H
