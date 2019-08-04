//
// Created by calper on 5/7/19.
//

#ifndef HIVEENGINE_DRAWING_H
#define HIVEENGINE_DRAWING_H

#include <HiveEngine/Renderer/Directive.h>
#include <HiveEngine/Renderer/vk_mem_alloc.h>
#include <HiveEngine/Common.h>
#include <cstddef>

namespace HiveEngine::Renderer {
    class Drawing {
    private:
        bool enabled = true;
        bool inited = false;
		bool parent_managed = false;
        Directive *directive;
        size_t id;

    public:
        Drawing(Directive *directive);

        virtual ~Drawing();

        virtual void init(VkRenderPass render_pass);

        virtual void draw(VkCommandBuffer cmd_buffer);

        virtual void cleanup();

        virtual void enable();

        virtual void disable();

        virtual bool is_enabled();

        virtual bool is_inited();

		virtual bool is_parent_managed();

		virtual void mark_parent_managed();

        virtual Context *get_context();

        virtual Directive *get_directive();

        void set_id(size_t id);

        size_t get_id();
    };
}

#endif //HIVEENGINE_DRAWING_H
