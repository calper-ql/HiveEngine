//
// Created by calper on 5/1/19.
//

#ifndef HIVEENGINE_RENDERER_CONTEXT_H
#define HIVEENGINE_RENDERER_CONTEXT_H

#include <HiveEngine/Renderer/glad/glad.h>
#include <GLFW/glfw3.h>


#include <iostream>
#include <stdexcept>
#include <vector>
#include <optional>
#include <set>
#include <string>
#include <filesystem>
#include <fstream>
#include <map>

#include <HiveEngine/Texture.h>
#include <HiveEngine/Buffer.hpp>
#include <glm/common.hpp>
#include <glm/fwd.hpp>

/*
        Shader extensions

        .vert - a vertex shader
        .tesc - a tessellation control shader
        .tese - a tessellation evaluation shader
        .geom - a geometry shader
        .frag - a fragment shader
        .comp - a compute shader

 */

namespace HiveEngine::Renderer{
    class Drawing;

    void context_resize_event(GLFWwindow* window, int width, int height);

    void context_maximize_event(GLFWwindow* window, int maximized);

    void context_position_event(GLFWwindow* window, int xpos, int ypos);

    GLenum get_glsl_shader_type(std::string path);

    class Context {
    public:
        std::map<std::string, GLuint> shaders;

        explicit Context(bool visible);

        void init_window(glm::uvec2 window_size, bool resizable);

        void cleanup();

        void load_shaders(std::string path);

        void resized();

        bool is_visible();

        GLFWwindow* get_window();

        size_t register_drawing(Drawing* drawing);

        void remove_drawing(Drawing *drawing);

        glm::uvec2 get_window_size();

        virtual bool draw();

    private:
        bool resize = false;
        int last_width = 0;
        int last_height = 0;
        GLFWwindow *window = nullptr;

        bool visible;
        Buffer<Drawing*> drawings;
    };

}

#endif //HIVEENGINE_RENDERER_CONTEXT_H
