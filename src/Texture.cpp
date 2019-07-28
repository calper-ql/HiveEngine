//
// Created by calper on 5/11/19.
//

#include <HiveEngine/Texture.h>

#define STB_IMAGE_IMPLEMENTATION

#include <HiveEngine/stb/stb_image.h>
#include <stdexcept>
#include <iostream>
#include <filesystem>

namespace HiveEngine {
    Texture load_texture(std::string path) {
        Texture texture = {};


        if (!std::filesystem::exists(path)) {
            throw std::runtime_error("load_texture could not find path: " + path);
        }

        stbi_uc *pixels = stbi_load(path.c_str(), &texture.width, &texture.height, &texture.channel, STBI_rgb_alpha);
        if (pixels == nullptr) {
            throw std::runtime_error("stbi_load could not load: " + path);
        }

        texture.data.resize(texture.width * texture.height * 4);
        texture.channel = 4;
        memcpy(texture.data.data(), pixels, texture.data.size());
        stbi_image_free(pixels);

        return texture;
    }

}


