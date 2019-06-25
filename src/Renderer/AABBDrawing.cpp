//
// Created by calpe on 6/24/2019.
//

#include <HiveEngine/Renderer/AABBDrawing.h>
namespace HiveEngine::Renderer {
    AABBDrawing::AABBDrawing(HiveEngine::Renderer::Context *context, unsigned line_width) : Drawing(context) {
        this->line_width = line_width;

        vert_shader = context->shaders["aabb_shader.vert"];
        frag_shader = context->shaders["aabb_shader.frag"];

        program = glCreateProgram();
        glAttachShader(program, vert_shader);
        glAttachShader(program, frag_shader);
        glLinkProgram(program);

        GLint program_linked = GL_FALSE;
        glGetProgramiv(program, GL_LINK_STATUS, &program_linked);
        if(program_linked != GL_TRUE){
            spdlog::critical(" ======= PROGRAM INFOLOG =======");
            GLint log_size;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &log_size);
            std::vector<char> info_log(log_size);
            glGetProgramInfoLog(program, log_size, nullptr, info_log.data());
            spdlog::critical("  " + std::string(info_log.data(), info_log.size()));
            spdlog::error("Shader compilation failed, aborting!");
            process_error();
        }

        glm::vec3 lines[24];

        lines[0] = {0, 0, 0}; lines[1] = {1, 0, 0};
        lines[2] = {0, 1, 0}; lines[3] = {1, 1, 0};
        lines[4] = {0, 0, 1}; lines[5] = {1, 0, 1};
        lines[6] = {0, 1, 1}; lines[7] = {1, 1, 1};

        lines[8]  = {0, 0, 0}; lines[9]  = {0, 1, 0};
        lines[10] = {0, 0, 1}; lines[11] = {0, 1, 1};
        lines[12] = {1, 0, 0}; lines[13] = {1, 1, 0};
        lines[14] = {1, 0, 1}; lines[15] = {1, 1, 1};

        lines[16] = {0, 0, 0}; lines[17] = {0, 0, 1};
        lines[18] = {1, 0, 0}; lines[19] = {1, 0, 1};
        lines[20] = {0, 1, 0}; lines[21] = {0, 1, 1};
        lines[22] = {1, 1, 0}; lines[23] = {1, 1, 1};

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &VBO_DAABB);
        glGenBuffers(1, &VBO_significance);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * 24, lines, GL_DYNAMIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
        glEnableVertexAttribArray(0);

        glBindBuffer(GL_ARRAY_BUFFER, VBO_DAABB);
        glVertexAttribLPointer(1, 3,  GL_DOUBLE, sizeof(DAABB), (void*)offsetof(DAABB, min));
        glEnableVertexAttribArray(1);
        glVertexAttribLPointer(2, 3, GL_DOUBLE, sizeof(DAABB), (void*)offsetof(DAABB, max));
        glEnableVertexAttribArray(2);
        glVertexAttribDivisor(1, 1);
        glVertexAttribDivisor(2, 1);

        glBindBuffer(GL_ARRAY_BUFFER, VBO_significance);
        glVertexAttribIPointer(3, 1, GL_INT, 0, nullptr);
        glEnableVertexAttribArray(3);
        glVertexAttribDivisor(3, 1);

    }

    AABBDrawing::~AABBDrawing() {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &VBO_DAABB);
        glDeleteBuffers(1, &VBO_significance);
    }

    void AABBDrawing::draw() {
        CameraPackage cp;
        if(camera) {
            camera->set_ratio(get_window_size().x / (float)get_window_size().y);
            cp = camera->get_package();
        }

        Drawing::draw();

        glUseProgram(program);
        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO_DAABB);
        glBufferData(GL_ARRAY_BUFFER, sizeof(DAABB) * bbox.size(), bbox.data(), GL_DYNAMIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, VBO_significance);
        glBufferData(GL_ARRAY_BUFFER, sizeof(int) * significance.size(), significance.data(), GL_DYNAMIC_DRAW);

        unsigned int view_apply_loc = glGetUniformLocation(program, "view_apply");
        glUniform1i(view_apply_loc, cp.apply);

        unsigned int view_loc = glGetUniformLocation(program, "view");
        glUniformMatrix4fv(view_loc, 1, GL_FALSE, glm::value_ptr(cp.view));

        unsigned int offset_loc = glGetUniformLocation(program, "offset");
        glUniform3d(offset_loc, offset.x, offset.y, offset.z);

        glDrawArraysInstanced(GL_LINES, 0, 24, bbox.size());
    }
}
