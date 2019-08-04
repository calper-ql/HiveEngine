//
// Created by calper on 5/3/19.
//

#include <HiveEngine/Renderer/Directive.h>
#include <HiveEngine/Renderer/Drawing.h>

namespace HiveEngine::Renderer {

    Directive::Directive(Context *context) {
        if (context == nullptr) throw std::runtime_error("A Directive was passed a nullptr context!");
        this->context = context;
        context->register_directive(this);
    }

    void Directive::init_frame_buffers() {
        size.x = get_context()->get_swap_chain_extent().width;
        size.y = get_context()->get_swap_chain_extent().height;
    }

    Context *Directive::get_context() {
        return context;
    }

    Directive::~Directive() {

    }

    void Directive::init_command_buffer() {

    }

    VkCommandBuffer Directive::get_command_buffer(uint8_t idx) {
        return nullptr;
    }

    void Directive::cleanup() {
		
    }

    glm::uvec2 Directive::get_size(){
        return size;
    }

    void Directive::register_drawing(Drawing *drawing) {
        drawing->set_id(drawings.add(drawing));
		get_context()->recreate_swap_chain();
    }

    void Directive::erase_drawing(Drawing *drawing) {
		drawing->cleanup();
        drawings.remove(drawing->get_id());
    }
}