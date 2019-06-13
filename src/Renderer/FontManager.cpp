//
// Created by calpe on 6/7/2019.
//

#include <HiveEngine/Renderer/FontManager.h>
#include <stdexcept>
#include <freetype/ftglyph.h>
#include <HiveEngine/HiveEngine.h>

namespace HiveEngine::Renderer {
    FontManager::FontManager() {
        auto error = FT_Init_FreeType(&library);
        if (error) {
            spdlog::error("Freetype init failed!");
            process_error();
        }
    }

    FontManager::~FontManager() {
        for (auto face: faces) {
            FT_Done_Face(face.second);
        }

        FT_Done_FreeType(library);
    }

    void FontManager::load_font(std::string font_name, std::string path) {
        FT_Face face;
        auto error = FT_New_Face(library, path.c_str(), 0, &face);
        if (error) {
            spdlog::error("Font at [ " + path + " ] could not be loaded!");
            process_error();
        }
        faces[font_name] = face;
        FT_Set_Pixel_Sizes(face, 0, 60*64);
    }

    Glyph FontManager::get_glyph(std::string font_name, FT_ULong c) {
        Glyph glyph;

        if(faces.find(font_name) == faces.end()){
            spdlog::error("Font (" + font_name + ") not found!");
            process_error();
        }

        auto face = faces[font_name];

        auto error = FT_Load_Char(face, c, FT_LOAD_RENDER);
        if(error){
            spdlog::critical("Char " + std::to_string(c) + " is missing from " + font_name + " font face!");
        }

        FT_GlyphSlot slot = face->glyph;

        glyph.texture.width = slot->bitmap.width;
        glyph.texture.height = slot->bitmap.rows;
        glyph.texture.channel = 1;

        glyph.metrics = slot->metrics;

        glyph.texture.data.resize(glyph.texture.width * glyph.texture.height * glyph.texture.channel);
        std::memcpy(glyph.texture.data.data(), slot->bitmap.buffer, glyph.texture.data.size());

        return glyph;
    }

}


