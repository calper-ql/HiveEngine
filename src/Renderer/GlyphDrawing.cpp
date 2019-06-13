#include <utility>

//
// Created by calper on 5/11/19.
//

#include <HiveEngine/Renderer/GlyphDrawing.h>

namespace HiveEngine::Renderer {

    GlyphDrawing::GlyphDrawing(Context* context, HiveEngine::Texture texture) : Drawing(context) {
        this->texture = texture;

        vert_shader = context->shaders["glyph_shader.vert"];
        frag_shader = context->shaders["glyph_shader.frag"];

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

        GLenum format = GL_RGBA;
        if(texture.channel == 3) format = GL_RGB;
        if(texture.channel == 1) format = GL_RED;

        glPixelStorei(GL_UNPACK_ALIGNMENT, texture.channel);

        glGenTextures(1, &texture_id);
        glBindTexture(GL_TEXTURE_2D, texture_id);

        glTexImage2D(GL_TEXTURE_2D,
                0,
                format,
                texture.width,
                texture.height,
                0,
                format,
                GL_UNSIGNED_BYTE,
                texture.data.data());

        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        imos.mark_changed();


        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);

        GLint part_size = sizeof(ImageOrientation)/3;

        // position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, part_size, (void *) offsetof(ImageOrientation, f0));
        glEnableVertexAttribArray(0);
        // texture coord attribute
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, part_size, (void *) offsetof(ImageOrientation, f0uv));
        glEnableVertexAttribArray(1);

        glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, part_size, (void *) offsetof(ImageOrientation, c0));
        glEnableVertexAttribArray(2);
    }

    GlyphDrawing::~GlyphDrawing() {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteTextures(1, &texture_id);
    }

    void GlyphDrawing::draw() {
        Drawing::draw();
        glEnable(GL_DEPTH_TEST);

        glUseProgram(program);
        glBindVertexArray(VAO);

        if(imos.is_changed()){
            imos.mark_unchanged();
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(ImageOrientation) * imos.size(), imos.get_data().data(), GL_DYNAMIC_DRAW);
        }

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture_id);
        glUniform1i(glGetUniformLocation(frag_shader, "texture1"), 0);
        glDrawArrays(GL_TRIANGLES, 0, imos.size()*3);
    }

    ImageDescription
    GlyphDrawing::add_image_center(glm::vec3 position, float width, float height, glm::vec4 color) {
        ImageDescription id;

        float left = position.x - width / 2.0f;
        float right = position.x + width / 2.0f;
        float up = position.y + height / 2.0f;
        float down = position.y - height / 2.0f;

        ImageOrientation io1 = {};
        ImageOrientation io2 = {};

        io1.f0 = {left, up, position.z};
        io1.f0uv = {0.0f, 0.0f};
        io1.f1 = {right, up, position.z};
        io1.f1uv = {1.0f, 0.0f};
        io1.f2 = {right, down, position.z};
        io1.f2uv = {1.0f, 1.0f};

        io2.f0 = {right, down, position.z};
        io2.f0uv = {1.0f, 1.0f};
        io2.f1 = {left, down, position.z};
        io2.f1uv = {0.0f, 1.0f};
        io2.f2 = {left, up, position.z};
        io2.f2uv = {0.0f, 0.0f};

        id.orientation1 = imos.add(io1);
        id.orientation2 = imos.add(io2);
        id.glyph_drawing = this;

        return id;
    }

    ImageDescription
    GlyphDrawing::add_image_lower_left(glm::vec3 position, float width, float height, glm::vec4 color) {
        ImageDescription id;

        float left = position.x;
        float right = position.x + width;
        float up = position.y + height;
        float down = position.y;

        ImageOrientation io1 = {};
        ImageOrientation io2 = {};

        io1.f0 = {left, up, position.z};
        io1.f0uv = {0.0f, 0.0f};
        io1.c0 = color;
        io1.f1 = {right, up, position.z};
        io1.f1uv = {1.0f, 0.0f};
        io1.c1 = color;
        io1.f2 = {right, down, position.z};
        io1.f2uv = {1.0f, 1.0f};
        io1.c2 = color;

        io2.f0 = {right, down, position.z};
        io2.f0uv = {1.0f, 1.0f};
        io2.c0 = color;
        io2.f1 = {left, down, position.z};
        io2.f1uv = {0.0f, 1.0f};
        io2.c1 = color;
        io2.f2 = {left, up, position.z};
        io2.f2uv = {0.0f, 0.0f};
        io2.c2 = color;

        id.orientation1 = imos.add(io1);
        id.orientation2 = imos.add(io2);
        id.glyph_drawing = this;

        return id;
    }


    void GlyphDrawing::remove_image(ImageDescription id) {
        ImageOrientation io1 = {};
        ImageOrientation io2 = {};

        imos.set(id.orientation1, io1);
        imos.set(id.orientation2, io2);

        imos.remove(id.orientation1);
        imos.remove(id.orientation2);
    }





}
