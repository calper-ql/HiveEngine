#include <optix.h>
#include <optixu/optixu_math_namespace.h>
#include <optixu/optixu_matrix_namespace.h>

using namespace optix;

#include "payloads.h"

rtBuffer<float4, 2> result_buffer; // RGBA32F

rtDeclareVariable(uint2, launch_index, rtLaunchIndex, );
rtDeclareVariable(uint2, theLaunchDim, rtLaunchDim, );
rtDeclareVariable(uint2, theLaunchIndex, rtLaunchIndex, );

rtDeclareVariable(Matrix3x3, view, , );
rtDeclareVariable(float3, origin, , );
rtDeclareVariable(float, fov, , );


rtDeclareVariable(rtObject, sysTopObject, , );

RT_PROGRAM void color_raygen()
{

    RayData pyld;
    pyld.origin = origin;
    pyld.radiance = make_float3(0.0);

    const float2 pixel = make_float2(theLaunchIndex);
    const float2 fragment = pixel + make_float2(0.5f);
    const float2 screen = make_float2(theLaunchDim);
    const float2 ndc = (fragment / screen) * 2.0f - 1.0f;

    float aspectRatio = screen.x / screen.y;
    float focus = tan(fov/2.0);

    float3 direction = view * normalize(make_float3(-ndc.x * aspectRatio, -ndc.y, focus));

    optix::Ray ray = optix::make_Ray(origin, direction, 0, 0.0f, RT_DEFAULT_MAX);
    rtTrace(sysTopObject, ray, pyld);

    result_buffer[launch_index] = make_float4(pyld.radiance, 1.0f);

}