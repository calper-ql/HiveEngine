//
// Created by calper on 5/7/19.
//

#ifndef HIVEENGINE_DRAWING_H
#define HIVEENGINE_DRAWING_H

#include <HiveEngine/Common.h>
#include <HiveEngine/Renderer/Context.h>
#include <cstddef>

namespace HiveEngine::Renderer {
    class Drawing {
    private:
        bool enabled = true;
        glm::uvec2 window_size;
        size_t id;
        Context* context = nullptr;

    public:
        Drawing(Context* context);

        virtual ~Drawing();

        virtual void draw();

        virtual void enable();

        virtual void disable();

        virtual bool is_enabled();

        virtual Context *get_context();

        size_t get_id();

        virtual void update_window_size(glm::uvec2 new_size);

        glm::uvec2 get_window_size();
    };

}

#endif //HIVEENGINE_DRAWING_H
