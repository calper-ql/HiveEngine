//
// Created by calpe on 6/12/2019.
//

#ifndef HIVEENGINE_LINEDRAWING_H
#define HIVEENGINE_LINEDRAWING_H

#include <HiveEngine/Renderer/Drawing.h>
#include <HiveEngine/Buffer.hpp>
#include <HiveEngine/Texture.h>
#include <HiveEngine/Renderer/Camera.h>

namespace HiveEngine::Renderer {
    struct LineDescription {
        size_t id;
    };

    class LineDrawing : public Drawing {
    public:
        HiveEngine::Buffer<Line> lines; // Orientations
        HiveEngine::Renderer::Camera* camera = nullptr;

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

        LineDescription add_line(Line line);

        void refresh_line(LineDescription ld, Line new_line);

        void remove_line(LineDescription ld);
    };

}

#endif //HIVEENGINE_LINEDRAWING_H
