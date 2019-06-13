//
// Created by calpe on 6/12/2019.
//

#ifndef HIVEENGINE_LINEDRAWING_H
#define HIVEENGINE_LINEDRAWING_H

#include <HiveEngine/Renderer/Drawing.h>
#include <HiveEngine/Buffer.hpp>
#include <HiveEngine/Texture.h>

namespace HiveEngine::Renderer {
    struct LineData {
        glm::vec3 a_position;
        glm::vec4 a_color;
        glm::vec3 b_position;
        glm::vec4 b_color;
    };

    struct LineDescription {
        size_t id;
    };

    class LineDrawing : public Drawing {
    public:
        HiveEngine::Buffer<LineData> vertices; // Orientations

        GLuint texture_id;
        GLuint VBO, VAO;
        GLuint program;

        GLuint vert_shader;
        GLuint frag_shader;

        unsigned line_width;

        explicit LineDrawing(Context *context, unsigned line_width=1);

        ~LineDrawing() override;

        void draw() override;

        LineDescription add_line(glm::vec3 a, glm::vec4 ac, glm::vec3 b, glm::vec4 bc);

        void remove_line(LineDescription ld);
    };

}

#endif //HIVEENGINE_LINEDRAWING_H
