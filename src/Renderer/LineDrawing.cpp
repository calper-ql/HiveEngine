//
// Created by calpe on 6/12/2019.
//

#include <HiveEngine/Renderer/LineDrawing.h>

namespace HiveEngine::Renderer {

    LineDrawing::LineDrawing(Context *context, unsigned line_width) : Drawing(context) {
        this->line_width = line_width;

        vert_shader = context->shaders["line_shader.vert"];
        frag_shader = context->shaders["line_shader.frag"];

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

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);

        GLint part_size = sizeof(glm::vec3) + sizeof(glm::vec4);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, part_size, (void *)offsetof(LineData, a_position));
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, part_size, (void *)offsetof(LineData, a_color));
        glEnableVertexAttribArray(1);

        vertices.mark_changed();
    }

    void LineDrawing::draw() {
        Drawing::draw();
        glEnable(GL_DEPTH_TEST);

        glUseProgram(program);
        glBindVertexArray(VAO);

        if(vertices.is_changed()){
            vertices.mark_unchanged();
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(LineData) * vertices.size(), vertices.get_data().data(), GL_DYNAMIC_DRAW);
        }

        glLineWidth(line_width);

        glDrawArrays(GL_LINES, 0, vertices.size()*2);

    }

    LineDrawing::~LineDrawing() {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
    }

    LineDescription LineDrawing::add_line(glm::vec3 a, glm::vec4 ac, glm::vec3 b, glm::vec4 bc) {
        LineData data = {};
        LineDescription ld = {};

        data.a_position = a;
        data.b_position = b;
        data.a_color = ac;
        data.b_color = bc;

        ld.id = vertices.add(data);
        return ld;
    }

    void LineDrawing::remove_line(LineDescription ld) {
        LineData data = {};
        vertices.set(ld.id, data);
        vertices.remove(ld.id);
    }
}