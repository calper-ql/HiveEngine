//
// Created by calper on 5/11/19.
//

#include <HiveEngine/Texture.h>

#define STB_IMAGE_IMPLEMENTATION
#include <HiveEngine/stb/stb_image.h>
#include <stdexcept>
#include <iostream>

namespace HiveEngine {
    Texture load_texture(std::string path) {
        Texture texture = {};

        stbi_uc* pixels = stbi_load(path.c_str(), &texture.width, &texture.height, &texture.channel, 0);
        if(pixels == nullptr) {
            throw std::runtime_error("stbi_load could not find path: " + path);
        }

        texture.data.resize(texture.width * texture.height * texture.channel);
        memcpy(texture.data.data(), pixels, texture.data.size());
        stbi_image_free(pixels);

        return texture;
    }

}


