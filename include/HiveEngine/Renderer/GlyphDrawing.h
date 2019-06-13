//
// Created by calper on 5/11/19.
//

#ifndef HIVEENGINE_GLYPHDRAWING_H
#define HIVEENGINE_GLYPHDRAWING_H

#include <HiveEngine/Renderer/Drawing.h>
#include <HiveEngine/Buffer.hpp>
#include <HiveEngine/Texture.h>

namespace HiveEngine::Renderer {
    struct ImageOrientation {
        glm::vec3 f0;
        glm::vec2 f0uv;
        glm::vec4 c0;
        glm::vec3 f1;
        glm::vec2 f1uv;
        glm::vec4 c1;
        glm::vec3 f2;
        glm::vec2 f2uv;
        glm::vec4 c2;
    };

    struct ImageColorDescription {
        glm::vec4 color = {1.0, 0.0, 1.0, 1.0};
    };

    class GlyphDrawing;

    struct ImageDescription {
        GlyphDrawing* glyph_drawing;
        size_t orientation1;
        size_t orientation2;
    };

    class GlyphDrawing : public Drawing {
    public:
        HiveEngine::Buffer<ImageOrientation> imos; // Orientations

        HiveEngine::Texture texture;

        GLuint texture_id;
        GLuint VBO, VAO;
        GLuint program;

        GLuint vert_shader;
        GLuint frag_shader;

        GlyphDrawing(Context* context, HiveEngine::Texture texture);

        ~GlyphDrawing() override;

        void draw() override;

        ImageDescription add_image_center(glm::vec3 position, float width, float height, glm::vec4 color);

        ImageDescription add_image_lower_left(glm::vec3 position, float width, float height, glm::vec4 color);

        void remove_image(ImageDescription id);
    };
}

#endif //HIVEENGINE_GLYPHDRAWING_H