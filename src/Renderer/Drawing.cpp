//
// Created by calper on 5/7/19.
//

#include <HiveEngine/Renderer/Drawing.h>
#include <HiveEngine/HiveEngine.h>
#include <HiveEngine/Renderer/Context.h>

namespace HiveEngine::Renderer {

    Drawing::Drawing(Context* context) {
        if(context == nullptr) {
            spdlog::error("Drawing was passed a nullptr context...");
            process_error();
        }
        this->context = context;
        this->id = context->register_drawing(this);
        update_window_size(context->get_window_size());
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

    Context *Drawing::get_context() {
        return nullptr;
    }

    void Drawing::draw() {

    }

    size_t Drawing::get_id() {
        return id;
    }

    void Drawing::update_window_size(glm::uvec2 new_size) {
        this->window_size = new_size;
    }

    Drawing::~Drawing() {
        context->remove_drawing(this);
    }

    glm::uvec2 Drawing::get_window_size() {
        return window_size;
    }

}