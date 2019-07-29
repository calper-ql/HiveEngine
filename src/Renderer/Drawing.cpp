//
// Created by calper on 5/7/19.
//

#include <HiveEngine/Renderer/Drawing.h>

namespace HiveEngine::Renderer {

    Drawing::Drawing(Directive *directive) {
        if (directive == nullptr) {
            throw std::runtime_error("failure, drawing was passed a nullptr directive");
        }
        this->directive = directive;
        //id = directive->register_drawing(this);
    }

    Drawing::~Drawing(){
        //directive
    }

    void Drawing::enable() {
        enabled = true;
    }

    void Drawing::disable() {
        enabled = false;
    }

    bool Drawing::is_enabled() {
        return enabled;
    }

    bool Drawing::is_inited() {
        return inited;
    }

    void Drawing::init(VkRenderPass render_pass) {
        inited = true;
    }

    Context *Drawing::get_context() {
        return directive->get_context();
    }

    Directive *Drawing::get_directive() {
        return directive;
    }

    void Drawing::cleanup() {
        inited = false;
    }

    void Drawing::draw(VkCommandBuffer cmd_buffer) {

    }

}