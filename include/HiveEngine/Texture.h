//
// Created by calper on 4/19/19.
//

#ifndef HIVEENGINE_TEXTURE_H
#define HIVEENGINE_TEXTURE_H

#include <cstddef>
#include <string>
#include <vector>

namespace HiveEngine {
    struct Texture {
        std::vector<uint8_t> data;
        int width, height, channel;
    };

    Texture load_texture(std::string path);
}

#endif //HIVEENGINE_TEXTURE_H
