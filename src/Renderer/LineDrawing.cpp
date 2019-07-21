//
// Created by calpe on 6/12/2019.
//

#include <HiveEngine/Renderer/LineDrawing.h>
#include <HiveEngine/Utilities.h>

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
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, part_size, (void *)offsetof(Line, a.position));
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, part_size, (void *)offsetof(Line, a.color));
        glEnableVertexAttribArray(1);

        lines.mark_changed();
    }

    void LineDrawing::draw() {
        CameraPackage cp;
        if(camera) {
            camera->set_ratio(get_window_size().x / (float)get_window_size().y);
            cp = camera->get_package();
        }

        Drawing::draw();
        glEnable(GL_DEPTH_TEST);

        glUseProgram(program);
        glBindVertexArray(VAO);

        if(lines.is_changed()){
            lines.mark_unchanged();
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(Line) * lines.size(), lines.get_data().data(), GL_DYNAMIC_DRAW);
        }

        glLineWidth(line_width);

        int view_apply_loc = glGetUniformLocation(program, "view_apply");
        glUniform1i(view_apply_loc, cp.apply);

        int view_loc = glGetUniformLocation(program, "view");
        glUniformMatrix4fv(view_loc, 1, GL_FALSE, glm::value_ptr(cp.view));

        int offset_loc = glGetUniformLocation(program, "offset");
        glUniform3f(offset_loc, offset.x, offset.y, offset.z);

        glDrawArrays(GL_LINES, 0, lines.size()*2);

    }

    LineDrawing::~LineDrawing() {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
    }

    LineDescription LineDrawing::add_line(glm::vec3 a, glm::vec4 ac, glm::vec3 b, glm::vec4 bc) {
        Line data = {};
        LineDescription ld = {};

        data.a.position = a;
        data.b.position = b;
        data.a.color = ac;
        data.b.color = bc;

        ld.id = lines.add(data);
        return ld;
    }

    void LineDrawing::remove_line(LineDescription ld) {
        Line data = {};
        lines.set(ld.id, data);
        lines.remove(ld.id);
    }

    LineDescription LineDrawing::add_line(Line line) {
        Line data = line;
        LineDescription ld = {};
        ld.id = lines.add(data);
        return ld;
    }

    void LineDrawing::refresh_line(LineDescription ld, Line new_line) {
        lines.set(ld.id, new_line);
    }
}