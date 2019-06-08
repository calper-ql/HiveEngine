//
// Created by calpe on 6/7/2019.
//

#include <HiveEngine/Renderer/FontManager.h>
#include <stdexcept>

HiveEngineRenderer::FontManager::FontManager() {
    auto error = FT_Init_FreeType(&library);
    if(error){
        std::runtime_error("Freetype init failed!");
    }
}

HiveEngineRenderer::FontManager::~FontManager() {
    for(auto face: faces){
        FT_Done_Face(face.second);
    }

    FT_Done_FreeType(library);
}

void HiveEngineRenderer::FontManager::load_font(std::string font_name, std::string path) {
    FT_Face face;
    auto error = FT_New_Face(library, path.c_str(), 0, &face);
    if(error){
        std::runtime_error("Freetype could not load: " + path + "!");
    }
    std::cout << "font count: " << face->num_charmaps << std::endl;



}
