//
// Created by calper on 4/18/19.
//

#ifndef HIVEENGINE_HIVEENGINE_H
#define HIVEENGINE_HIVEENGINE_H

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#include <glm/glm.hpp>
#include <spdlog/spdlog.h>

namespace HiveEngine {

    extern double PI;
    extern double PI_HALF;
    extern double PI_DOUBLE;
    extern double epsilon;

    int get_major_version();

    int get_minor_version();

    void process_error();
}

#endif //HIVEENGINE_HIVEENGINE_H
