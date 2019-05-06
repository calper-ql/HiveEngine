//
// Created by calper on 5/3/19.
//

#ifndef HIVEENGINE_DIRECTIVE_H
#define HIVEENGINE_DIRECTIVE_H

#include <HiveEngine/Renderer/Context.h>

namespace HiveEngineRenderer {
    class Directive {
    private:
        Context* context;

    public:
        Directive(Context* context);
        virtual ~Directive();
        virtual void init_frame_buffers();
        virtual void init_command_buffer();
        virtual void cleanup();
        virtual std::vector<VkCommandBuffer> get_command_buffer();
        Context* get_context();
    };
}

#endif //HIVEENGINE_DIRECTIVE_H
