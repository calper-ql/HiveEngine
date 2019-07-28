//
// Created by calpe on 6/7/2019.
//

#include <HiveEngine/Renderer/FontManager.h>
#include <stdexcept>
#include <freetype/ftglyph.h>

HiveEngine::Renderer::FontManager::FontManager() {
    auto error = FT_Init_FreeType(&library);
    if (error) {
        std::runtime_error("Freetype init failed!");
    }
}

HiveEngine::Renderer::FontManager::~FontManager() {
    for (auto face: faces) {
        FT_Done_Face(face.second);
    }

    FT_Done_FreeType(library);
}

void HiveEngine::Renderer::FontManager::load_font(std::string font_name, std::string path) {
    FT_Face face;
    auto error = FT_New_Face(library, path.c_str(), 0, &face);
    if (error) {
        std::runtime_error("Freetype could not load: " + path + "!");
    }
    faces[font_name] = face;
    FT_Set_Pixel_Sizes(face, 0, 60*64);
}

HiveEngine::Renderer::Glyph HiveEngine::Renderer::FontManager::get_glyph(std::string font_name, FT_ULong c) {
    Glyph glyph;

    if(faces.find(font_name) == faces.end()){
        std::runtime_error("Font not found!");
    }

    auto face = faces[font_name];

    auto error = FT_Load_Char(face, c, FT_LOAD_RENDER);
    // ignore for now

    FT_GlyphSlot slot = face->glyph;

    glyph.texture.width = slot->bitmap.width;
    glyph.texture.height = slot->bitmap.rows;
    glyph.texture.channel = 1;

    glyph.metrics = slot->metrics;

    glyph.texture.data.resize(glyph.texture.width * glyph.texture.height * glyph.texture.channel);
    std::memcpy(glyph.texture.data.data(), slot->bitmap.buffer, glyph.texture.data.size());

    return glyph;
}
