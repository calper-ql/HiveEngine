//
// Created by calpe on 6/10/2019.
//

#include "HiveEngine/Renderer/TextDrawing.h"

namespace HiveEngineRenderer {

    TextDrawing::TextDrawing(Directive *directive, FontManager *font_manager, std::string font_name) : Drawing(directive) {
        for (int i = 0; i < 128; ++i) {
            auto glyph = font_manager->get_glyph(font_name, i);
            glyphs.push_back(glyph);
            glyph_drawings.push_back(new GlyphDrawing(directive, glyph.texture));
        }
    }

    void TextDrawing::init(VkRenderPass render_pass) {
        Drawing::init(render_pass);
        for (int i = 0; i < 128; ++i) {
            if(glyphs[i].texture.channel > 0) glyph_drawings[i]->init(render_pass);
        }
    }

    void TextDrawing::draw(VkCommandBuffer cmd_buffer) {
        Drawing::draw(cmd_buffer);
        for (int i = 0; i < 128; ++i) {
            if(glyphs[i].texture.channel > 0) glyph_drawings[i]->draw(cmd_buffer);
        }
    }

    void TextDrawing::cleanup() {
        Drawing::cleanup();
        for (int i = 0; i < 128; ++i) {
            if(glyphs[i].texture.channel > 0) glyph_drawings[i]->cleanup();
        }
    }

    TextDescription TextDrawing::add_text_center(std::string str, glm::vec3 pos, float max_height) {
        TextDescription td;

        float width = 0;
        float height = 0;

        float ymax = -999999.0;
        float ymin = 999999.0;

        for (int i = 0; i < str.size(); ++i) {
            if(str[i] <= 127){
                width += glyphs[str[i]].metrics.horiAdvance;
                if(glyphs[str[i]].metrics.height > height) height = glyphs[str[i]].metrics.height;
            }
        }

        float scale = max_height / height;
        std::cout << "text scale: " << scale << std::endl;
        width *= scale;
        height *= scale;

        glm::vec3 start = pos;
        start.x -= width/2.0;

        for (int i = 0; i < str.size(); ++i) {
            if(str[i] <= 127){
                if(str[i] != ' '){
                    glm::vec3 glyph_center = start;
                    glyph_center.x += glyphs[str[i]].metrics.horiBearingX*scale;
                    glyph_center.y -= (glyphs[str[i]].metrics.horiBearingY*scale) + height*scale/2.0f;

                    td.glyphs.push_back(glyph_drawings[str[i]]->add_image_lower_left(
                            glyph_center,
                            glyphs[str[i]].metrics.width*scale,
                            glyphs[str[i]].metrics.height*scale,
                            {1.0, 1.0, 1.0, 1.0}));
                }

                start.x += glyphs[str[i]].metrics.horiAdvance * scale;
            }
        }

        return td;
    }
}