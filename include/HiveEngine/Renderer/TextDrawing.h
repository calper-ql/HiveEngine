//
// Created by calpe on 6/10/2019.
//

#ifndef HIVEENGINE_TEXTDRAWING_H
#define HIVEENGINE_TEXTDRAWING_H

#include "FontManager.h"
#include <HiveEngine/Renderer/GlyphDrawing.h>
#include <HiveEngine/Common.h>

namespace HiveEngineRenderer{
    struct TextDescription {
        std::vector<ImageDescription> glyphs;
        HiveEngine::AABB bbox;
    };

    class TextDrawing : public Drawing {
        public:
        FontManager* font_manager;

        std::vector<Glyph> glyphs;
        std::vector<GlyphDrawing*> glyph_drawings;

        TextDrawing(Directive *directive, FontManager *font_manager, std::string font_name);

        void init(VkRenderPass render_pass) override;

        void draw(VkCommandBuffer cmd_buffer) override;

        void cleanup() override;

        TextDescription add_text_center(std::string str, glm::vec3 pos, float max_height);
    };
}


#endif //HIVEENGINE_TEXTDRAWING_H
