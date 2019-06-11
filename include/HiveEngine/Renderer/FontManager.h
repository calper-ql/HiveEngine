//
// Created by calpe on 6/7/2019.
//

#ifndef HIVEENGINE_FONTMANAGER_H
#define HIVEENGINE_FONTMANAGER_H

#include <ft2build.h>
#include FT_FREETYPE_H

#include <vector>
#include <string>
#include <map>
#include <iostream>

#include <HiveEngine/Texture.h>

namespace HiveEngineRenderer {
    struct Glyph {
        HiveEngine::Texture texture;
        FT_Glyph_Metrics metrics;
    };

    class FontManager {
    private:
        std::map<std::string, FT_Face> faces;

        FT_Library library;

    public:
        FontManager();

        ~FontManager();

        void load_font(std::string font_name, std::string path);

        Glyph get_glyph(std::string font_name, FT_ULong c);

    };
}

#endif //HIVEENGINE_FONTMANAGER_H
