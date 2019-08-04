//
// Created by calpe on 6/10/2019.
//

#include "HiveEngine/Renderer/TextDrawing.h"

namespace HiveEngine::Renderer {

    TextDrawing::TextDrawing(Directive *directive, FontManager *font_manager, std::string font_name) : Drawing(directive) {
        float highest_point = 0;
        float lowest_point = 0;

        for (int i = 0; i < 126; ++i) {
            auto glyph = font_manager->get_glyph(font_name, i);

			glyphs.push_back(glyph);
			auto gd = new GlyphDrawing(directive, glyph.texture);
			glyph_drawings.push_back(gd);
			gd->mark_parent_managed();
			
            if(glyph.metrics.horiBearingY > highest_point) highest_point = glyph.metrics.horiBearingY;
            auto diff = glyph.metrics.height - glyph.metrics.horiBearingY;
            if(diff > lowest_point) lowest_point = diff;
        }

        total_height = highest_point;
        hanging_dist = lowest_point;
    }

    TextDrawing::~TextDrawing() {
        for(auto gd: glyph_drawings) delete gd;
    }

    void TextDrawing::init(VkRenderPass render_pass) {
        Drawing::init(render_pass);

		int total = 0;
		for (int i = 0; i < glyph_drawings.size(); ++i) {
			if (glyph_drawings[i]->textureAllocation) total++;
		}

		for (int i = 0; i < glyph_drawings.size(); ++i) {
			glyph_drawings[i]->init(render_pass);
		}
    }

    void TextDrawing::draw(VkCommandBuffer cmd_buffer) {
        Drawing::draw(cmd_buffer);
		for (size_t i = 0; i < text_descriptors.size(); ++i) if (text_descriptors.get_state(i)) update_text(text_descriptors.get(i));
        for (int i = 0; i < glyph_drawings.size(); ++i) glyph_drawings[i]->draw(cmd_buffer);
    }

    void TextDrawing::cleanup() {
        Drawing::cleanup();
		for (int i = 0; i < glyph_drawings.size(); ++i) {
			glyph_drawings[i]->cleanup();
		}
    }

    std::vector<ImageDescription> TextDrawing::__add_text(std::string str, glm::vec3 pos, float max_height, TextDescriptionState tds,
                                            glm::vec4 color) {

        std::vector<ImageDescription> data;

        float width = 0;
        float height = total_height + hanging_dist;

        auto window_size = get_directive()->get_size();
        float ratio = (float)window_size.y / (float)window_size.x;

        for (int i = 0; i < str.size(); ++i) {
            if(str[i] <= 127) width += glyphs[str[i]].metrics.horiAdvance * ratio;
        }

        float scale = max_height / height;

        width *= scale;
        height *= scale;

        glm::vec3 start = pos;
        start.x -= width/2.0;
        start.y += hanging_dist * scale;

        for (int i = 0; i < str.size(); ++i) {
            if(str[i] <= 127){
                glm::vec3 glyph_center = start;
                glyph_center.x += glyphs[str[i]].metrics.horiBearingX * scale * ratio;
                glyph_center.y -= (glyphs[str[i]].metrics.height*scale) + (glyphs[str[i]].metrics.horiBearingY*scale - glyphs[str[i]].metrics.height*scale);

                data.push_back(glyph_drawings[str[i]]->add_image_lower_left(
                        glyph_center,
                        glyphs[str[i]].metrics.width*scale * ratio,
                        glyphs[str[i]].metrics.height*scale,
                        color));

                start.x += glyphs[str[i]].metrics.horiAdvance * scale * ratio;
            }
        }

        return data;
    }

    TextDescription
    TextDrawing::add_text(std::string str, glm::vec3 pos, float max_height, TextDescriptionState tds, glm::vec4 color) {
        TextDescription td;
        td.tds = tds;
        td.str = str;
        td.pos = pos;
        td.max_height = max_height;
        td.color = color;
        td.descriptors = __add_text(str, pos, max_height, tds, color);
        td.id = text_descriptors.add(td);
        text_descriptors.set(td.id, td);
        return td;
    }

    void TextDrawing::update_text(TextDescription td) {
        for (int i = 0; i < td.descriptors.size(); ++i) td.descriptors[i].glyph_drawing->remove_image(td.descriptors[i]);
		td.descriptors.clear();
        td.descriptors = __add_text(td.str, td.pos, td.max_height, td.tds, td.color);
        text_descriptors.set(td.id, td);
    }

    void TextDrawing::remove_text(TextDescription td) {
        for (int i = 0; i < td.descriptors.size(); ++i) td.descriptors[i].glyph_drawing->remove_image(td.descriptors[i]);
		td.descriptors.clear();
        text_descriptors.set(td.id, td);
        text_descriptors.remove(td.id);
    }

}