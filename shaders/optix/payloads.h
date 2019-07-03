//
// Created by calpe on 6/27/2019.
//

#ifndef HIVEENGINE_PAYLOADS_H
#define HIVEENGINE_PAYLOADS_H

#include <optix.h>
#include <optixu/optixu_math_namespace.h>

struct RayData {
    optix::float3 radiance; // Radiance along the current path segment.
    optix::float3 origin;
    //optix::float3 main_light_position;
    //optix::float3 main_light_intensity;
};

#endif //HIVEENGINE_PAYLOADS_H
