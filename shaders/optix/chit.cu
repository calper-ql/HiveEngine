#include <optix.h>
#include <optixu/optixu_math_namespace.h>

#include "rt_function.h"
#include "payloads.h"

// Context global variables provided by the renderer system.
rtDeclareVariable(rtObject, sysTopObject, , );

// Semantic variables.
rtDeclareVariable(optix::Ray, theRay, rtCurrentRay, );
rtDeclareVariable(RayData, thePrd, rtPayload, );

// Attributes.
rtDeclareVariable(optix::float3, varGeoNormal, attribute GEO_NORMAL, );
rtDeclareVariable(optix::float3, varNormal,    attribute NORMAL, );

RT_PROGRAM void chit()
{
  // Transform the (unnormalized) object space normals into world space.
  float3 geoNormal = optix::normalize(rtTransformNormal(RT_OBJECT_TO_WORLD, varGeoNormal));
  float3 normal    = optix::normalize(rtTransformNormal(RT_OBJECT_TO_WORLD, varNormal));

  // Check if the ray hit the geometry on the frontface or the backface.
  // The geometric normal is always defined on the front face of the geometry.
  // In this implementation the coordinate systems are right-handed and the frontface triangle winding is counter-clockwise (matching OpenGL).

  // If theRay.direction and geometric normal are in the same hemisphere we're looking at a backface.
  if (0.0f < optix::dot(theRay.direction, geoNormal))
  {
    // Flip the shading normal to the backface, because only that is used below.
    // (See later examples for more intricate handling of the frontface condition.)
    normal = -normal;
  }

  // Visualize the resulting world space normal on the surface we're looking on.
  // Transform the normal components from [-1.0f, 1.0f] to the range [0.0f, 1.0f] to get colors for negative values.
  thePrd.radiance = normal * 0.5f + 0.5f;
}