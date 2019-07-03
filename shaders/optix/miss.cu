#include <optix.h>
#include <optixu/optixu_math_namespace.h>

#include "payloads.h"

rtDeclareVariable(RayData, thePrd, rtPayload, );

RT_PROGRAM void miss() {
  thePrd.radiance = make_float3(0.2f, 0.2f, 0.2f);

}