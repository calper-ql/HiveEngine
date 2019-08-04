//
// Created by calper on 5/3/19.
//

#ifndef HIVEENGINE_DIRECTIVE_H
#define HIVEENGINE_DIRECTIVE_H

#include <HiveEngine/Renderer/Context.h>
#include <HiveEngine/Common.h>
#include <HiveEngine/Buffer.hpp>

namespace HiveEngine::Renderer {
    class Drawing;

    class Directive {
    private:
        Context *context;
        glm::uvec2 size = {};

    public:
        Buffer<Drawing*> drawings;

        Directive(Context *context);

        virtual ~Directive();

        virtual void init_frame_buffers();

        virtual void init_command_buffer();

        virtual void cleanup();

        virtual VkCommandBuffer get_command_buffer(uint8_t idx);

        Context *get_context();

        glm::uvec2 get_size();

        void register_drawing(Drawing* drawing);

        void erase_drawing(Drawing* drawing);
    };
}

#endif //HIVEENGINE_DIRECTIVE_H
