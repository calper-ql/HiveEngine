//
// Created by calper on 4/18/19.
//

#include <HiveEngine/HiveEngine.h>

#define GLM_FORECE_DEPTH_ZERO_TO_ONE

namespace HiveEngine {

    double PI = 3.14159265359;
    double PI_HALF = 3.14159265359 / 2.0;
    double PI_DOUBLE = 3.14159265359 * 2.0;
    double epsilon = 1e-7;

    int get_major_version() {
        return 0;
    }

    int get_minor_version() {
        return 2;
    }

    void process_error() {
        throw std::runtime_error("Check error logs...");
    }
}