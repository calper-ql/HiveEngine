//
// Created by calper on 4/19/19.
//

#ifndef HIVEENGINE_TEXTURE_H
#define HIVEENGINE_TEXTURE_H

#include <cstddef>

namespace HiveEngine {
    struct Texture {
        std::vector <uint8_t> data;
        unsigned width, height, channel;
    };
}

#endif //HIVEENGINE_TEXTURE_H
