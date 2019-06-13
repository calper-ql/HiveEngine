//
// Created by calpe on 6/10/2019.
//

#ifndef HIVEENGINE_TEXTDRAWING_H
#define HIVEENGINE_TEXTDRAWING_H

#include "FontManager.h"
#include <HiveEngine/Renderer/GlyphDrawing.h>
#include <HiveEngine/Common.h>

namespace HiveEngine::Renderer {
    enum TextDescriptionState {
        CENTER = 0, LEFT = 1, RIGHT = 2
    };

    struct TextDescription {
        std::string str;
        glm::vec3 pos;
        glm::vec4 color;
        float max_height;
        std::vector<ImageDescription> descriptors;
        HiveEngine::AABB bbox;
        size_t id;
        TextDescriptionState tds;
    };

    class TextDrawing : public Drawing {
        public:
        FontManager* font_manager;
        Context* context;

        Buffer<TextDescription> text_descriptors;

        std::vector<Glyph> glyphs;
        std::vector<GlyphDrawing*> glyph_drawings;

        float total_height;
        float hanging_dist;

        TextDrawing(Context* context, FontManager *font_manager, std::string font_name);
        virtual ~TextDrawing();

        TextDescription add_text(std::string str, glm::vec3 pos, float max_height, TextDescriptionState tds,
                                   glm::vec4 color = {
                                           1.0, 1.0, 1.0, 1.0});

        std::vector<ImageDescription> __add_text(std::string str, glm::vec3 pos, float max_height, TextDescriptionState tds,
                                   glm::vec4 color);

        void update_text(TextDescription td);

        void remove_text(TextDescription td);

        void update_window_size(glm::uvec2 new_size) override;

    };
}


#endif //HIVEENGINE_TEXTDRAWING_H