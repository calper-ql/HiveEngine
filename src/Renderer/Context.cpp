//
// Created by calper on 5/1/19.
//

#define VMA_IMPLEMENTATION

#include <HiveEngine/Renderer/Context.h>
#include <HiveEngine/HiveEngine.h>
#include <HiveEngine/Renderer/Drawing.h>

namespace HiveEngine::Renderer {
    Context::Context(bool visible) {
        this->visible = visible;
    }

    void Context::init_window(glm::uvec2 window_size, bool resizable) {
        spdlog::info("HiveEngine Version " + std::to_string(get_major_version()) + ":" + std::to_string(get_minor_version()) + " -> init renderer context");

        if(!glfwInit()){
            spdlog::error("Error occurred on glfwInit, this usually is a driver issue...");
            process_error();
        }

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        if(visible){
            glfwWindowHint(GLFW_VISIBLE, GLFW_TRUE);
        } else {
            glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
        }

        window = glfwCreateWindow(window_size.x, window_size.y, "HIVE ENGINE", nullptr, nullptr);
        if(window == nullptr){
            spdlog::error("Error occurred on glfwCreateWindow... visible state == " + std::to_string(visible));
            glfwTerminate();
            process_error();
        }

        glfwMakeContextCurrent(window);

        if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress))
        {
            spdlog::error("Error occurred on gladLoadGLLoader... visible state == " + std::to_string(visible));
            glfwTerminate();
            process_error();
        }

        glfwSetWindowSizeCallback(window, context_resize_event);
        //glfwSetFramebufferSizeCallback(window, context_resize_event);
        //glfwSetWindowMaximizeCallback(window, context_maximize_event);
        //glfwSetWindowPosCallback(window, context_position_event);
        glfwSetWindowUserPointer(window, this);

        glEnable(GL_DEPTH_TEST);
        glViewport(0, 0, window_size.x, window_size.y);

        draw();
    }

    void context_resize_event(GLFWwindow *window, int width, int height) {
        auto context = (Context*)glfwGetWindowUserPointer(window);
        context->resized();
    }

    void context_maximize_event(GLFWwindow *window, int maximized) {
        spdlog::info("Window (" + std::to_string((size_t)window) + ") maximize called!");
        auto context = (Context*)glfwGetWindowUserPointer(window);
        context->resized();
    }

    void context_position_event(GLFWwindow *window, int xpos, int ypos) {
        auto context = (Context*)glfwGetWindowUserPointer(window);
        context->resized();
    }

    GLenum get_glsl_shader_type(std::string path) {
        if(path.size() < 6){
            return GL_INVALID_ENUM;
        }

        std::string last_five = path.substr(path.size()-5,5);

        /*

        Shader extensions

        .vert - a vertex shader
        .tesc - a tessellation control shader
        .tese - a tessellation evaluation shader
        .geom - a geometry shader
        .frag - a fragment shader
        .comp - a compute shader

        */

        if(last_five == ".vert"){
            return GL_VERTEX_SHADER;
        } else if (last_five == ".frag") {
            return GL_FRAGMENT_SHADER;
        } else if (last_five == ".tesc") {
            return GL_TESS_CONTROL_SHADER;
        } else if (last_five == ".tese") {
            return GL_TESS_EVALUATION_SHADER;
        } else if (last_five == ".comp") {
            return GL_COMPUTE_SHADER;
        } else if (last_five == ".geom") {
            return GL_GEOMETRY_SHADER;
        }

        return GL_INVALID_ENUM;
    }

    void Context::cleanup() {

    }

    void Context::resized() {
        resize = true;
    }

    void Context::load_shaders(std::string path) {
        for(auto &entry : std::filesystem::directory_iterator(path)){
            if(entry.is_regular_file()){

                GLenum shader_type = get_glsl_shader_type(entry.path().string());
                if(shader_type == GL_INVALID_ENUM){
                    spdlog::critical("File at [ " + entry.path().string() + " ] does not have a recognized shader extension. Skipping...");
                } else {
                    spdlog::info("Loading shader: " + entry.path().string());
                    GLuint shader_id = glCreateShader(shader_type);
                    if(shader_id == 0) {
                        spdlog::error("Could not create shader (0==glCreateShader), please check (https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glCreateShader.xhtml)");
                        process_error();
                    } else if(shader_id == GL_INVALID_ENUM) {
                        spdlog::error("If you are seeing this error shader_type is GL_INVALID_ENUM and something went horribly wrong... check if your computer is on fire please!");
                        process_error();
                    } else {
                        std::string shader_name = entry.path().string();
                        shader_name.erase(0, shader_name.find_last_of("\\/")+1);
                        spdlog::info("Compiling shader: " + shader_name);

                        std::fstream file(entry.path());
                        std::string source(std::istreambuf_iterator<char>{file}, {});

                        GLchar* source_ptr = source.data();
                        GLint source_length = source.size();

                        glShaderSource(shader_id, 1, &source_ptr, &source_length);
                        glCompileShader(shader_id);

                        GLint compilation_status;
                        glGetShaderiv(shader_id, GL_COMPILE_STATUS, &compilation_status);

                        if(compilation_status == GL_TRUE){
                            spdlog::info("Compiled shader: " + shader_name);
                            shaders[shader_name] = shader_id;
                        } else {
                            spdlog::critical(" ======= SHADER INFOLOG =======");
                            GLint log_size;
                            glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &log_size);
                            std::vector<char> info_log(log_size);
                            glGetShaderInfoLog(shader_id, log_size, NULL, info_log.data());
                            spdlog::critical("  " + std::string(info_log.data(), info_log.size()));
                            spdlog::error("Shader compilation failed, aborting!");
                            process_error();
                        }
                    }


                }

            }
        }
    }

    bool Context::is_visible() {
        return visible;
    }

    GLFWwindow* Context::get_window() {
        return window;
    }

    glm::uvec2 Context::get_window_size(){
        int width, height;
        glfwGetWindowSize(window, &width, &height);
        return {width, height};
    }

    size_t Context::register_drawing(Drawing *drawing) {
        if(window == nullptr) {
            spdlog::error("Drawing was trying to register but window == nullptr...");
            process_error();
        }
        return drawings.add(drawing);
    }

    void Context::remove_drawing(Drawing *drawing) {
        auto interior = drawings.get(drawing->get_id());
        if(interior.second && drawing == interior.first){
            drawings.remove(drawing->get_id());
        }
    }

    bool Context::draw() {
        glfwMakeContextCurrent(window);
        glfwPollEvents();

        if(resize) {
            resize = false;
            int width, height;
            glfwGetWindowSize(window, &width, &height);
            glViewport(0, 0, width, height);
            for (int i = 0; i < drawings.size(); ++i) {
                auto pair = drawings.get(i);
                if(pair.second) pair.first->update_window_size({width, height});
            }
            spdlog::info("resized window");
        }

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        auto size = drawings.size();
        for (size_t i = 0; i < size; ++i) {
            auto drawing = drawings.get(i);
            if(drawing.second) drawing.first->draw();
        }

        glfwSwapBuffers(window);
        return !glfwWindowShouldClose(window);
    }

}


