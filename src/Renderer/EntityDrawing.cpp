//
// Created by calpe on 6/26/2019.
//

#include <HiveEngine/Renderer/EntityDrawing.h>
#include <HiveEngine/HiveEngine.h>

namespace HiveEngine::Renderer {
    EntityDrawing::EntityDrawing(HiveEngine::Renderer::Context *context, unsigned line_width) : Drawing(context) {
        this->line_width = line_width;

        vert_shader = context->shaders["entity_shader.vert"];
        frag_shader = context->shaders["entity_shader.frag"];

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

        glm::vec3 lines[360*2*3];

        glm::vec3 start = {0.0, 1.0, 0.0};
        glm::mat3 rm = generate_rotation_matrix('z', PI_DOUBLE/((360)));
        for (int i = 0; i < 360*2; i+=2) {
            lines[i] = start;
            lines[i+1] = rm * start;
            start = lines[i+1];
        }

        start = {0.0, 0.0, 1.0};
        rm = generate_rotation_matrix('y', PI_DOUBLE/((360)));
        for (int i = 0; i < 360*2; i+=2) {
            lines[(360*2) + i] = start;
            lines[(360*2) + i+1] = rm * start;
            start = lines[(360*2) + i+1];
        }

        start = {0.0, 1.0, 0.0};
        rm = generate_rotation_matrix('x', PI_DOUBLE/((360)));
        for (int i = 0; i < 360*2; i+=2) {
            lines[(360*2*2) + i] = start;
            lines[(360*2*2) + i+1] = rm * start;
            start = lines[(360*2*2) + i+1];
        }


        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &VBO_POS);
        glGenBuffers(1, &VBO_RAD);
        glGenBuffers(1, &VBO_significance);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(lines), lines, GL_DYNAMIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
        glEnableVertexAttribArray(0);

        glBindBuffer(GL_ARRAY_BUFFER, VBO_POS);
        glVertexAttribLPointer(1, 3,  GL_DOUBLE, 0, nullptr);
        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, VBO_RAD);
        glVertexAttribLPointer(2, 1, GL_DOUBLE, 0, nullptr);
        glEnableVertexAttribArray(2);
        glBindBuffer(GL_ARRAY_BUFFER, VBO_significance);
        glVertexAttribIPointer(3, 1, GL_INT, 0, nullptr);
        glEnableVertexAttribArray(3);

        glVertexAttribDivisor(1, 1);
        glVertexAttribDivisor(2, 1);
        glVertexAttribDivisor(3, 1);

    }

    EntityDrawing::~EntityDrawing() {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &VBO_POS);
        glDeleteBuffers(1, &VBO_RAD);
        glDeleteBuffers(1, &VBO_significance);
    }

    void EntityDrawing::draw() {
        CameraPackage cp;
        if(camera) {
            camera->set_ratio(get_window_size().x / (float)get_window_size().y);
            cp = camera->get_package();
        }

        Drawing::draw();

        glUseProgram(program);
        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO_POS);
        glBufferData(GL_ARRAY_BUFFER, sizeof(glm::dvec3) * pos.size(), pos.data(), GL_DYNAMIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, VBO_RAD);
        glBufferData(GL_ARRAY_BUFFER, sizeof(double) * radius.size(), radius.data(), GL_DYNAMIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, VBO_significance);
        glBufferData(GL_ARRAY_BUFFER, sizeof(int) * significance.size(), significance.data(), GL_DYNAMIC_DRAW);

        unsigned int view_apply_loc = glGetUniformLocation(program, "view_apply");
        glUniform1i(view_apply_loc, cp.apply);

        unsigned int view_loc = glGetUniformLocation(program, "view");
        glUniformMatrix4fv(view_loc, 1, GL_FALSE, glm::value_ptr(cp.view));

        unsigned int offset_loc = glGetUniformLocation(program, "offset");
        glUniform3d(offset_loc, offset.x, offset.y, offset.z);

        glDrawArraysInstanced(GL_LINES, 0, 360*2*3, pos.size());
    }
}
