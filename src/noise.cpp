//
// Created by calpe on 6/13/2019.
//

#include <HiveEngine/noise.h>

namespace HiveEngine {



    inline float linear_interp(float n0, float n1, float a)
    {
        return ((1.0 - a) * n0) + (a * n1);
    }

    inline float s_curve_5(float a)
    {
        float a3 = a * a * a;
        float a4 = a3 * a;
        float a5 = a4 * a;
        return (6.0f * a5) - (15.0f * a4) + (10.0f * a3);
    }

    float HiveEngine::gradient_coherent_noise(glm::vec3 vertex, int seed) {
        // Create a unit-length cube aligned along an integer boundary.  This cube
        // surrounds the input point.
        int x0 = (vertex.x > 0.0? (int)vertex.x: (int)vertex.x - 1);
        int x1 = x0 + 1;
        int y0 = (vertex.y > 0.0? (int)vertex.y: (int)vertex.y - 1);
        int y1 = y0 + 1;
        int z0 = (vertex.z > 0.0? (int)vertex.z: (int)vertex.z - 1);
        int z1 = z0 + 1;

        // Map the difference between the coordinates of the input value and the
        // coordinates of the cube's outer-lower-left vertex onto an S-curve.
        float xs = 0, ys = 0, zs = 0;

        xs = s_curve_5(vertex.x - (float) x0);
        ys = s_curve_5(vertex.y - (float) y0);
        zs = s_curve_5(vertex.z - (float) z0);


        // Now calculate the noise values at each vertex of the cube.  To generate
        // the coherent-noise value at the input point, interpolate these eight
        // noise values using the S-curve value as the interpolant (trilinear
        // interpolation.)
        float n0, n1, ix0, ix1, iy0, iy1;
        n0   = gradient_noise ({vertex.x, vertex.y, vertex.z}, {x0, y0, z0}, seed);
        n1   = gradient_noise ({vertex.x, vertex.y, vertex.z}, {x1, y0, z0}, seed);
        ix0  = linear_interp(n0, n1, xs);
        n0   = gradient_noise ({vertex.x, vertex.y, vertex.z}, {x0, y1, z0}, seed);
        n1   = gradient_noise ({vertex.x, vertex.y, vertex.z}, {x1, y1, z0}, seed);
        ix1  = linear_interp(n0, n1, xs);
        iy0  = linear_interp(ix0, ix1, ys);
        n0   = gradient_noise ({vertex.x, vertex.y, vertex.z}, {x0, y0, z1}, seed);
        n1   = gradient_noise ({vertex.x, vertex.y, vertex.z}, {x1, y0, z1}, seed);
        ix0  = linear_interp(n0, n1, xs);
        n0   = gradient_noise ({vertex.x, vertex.y, vertex.z}, {x0, y1, z1}, seed);
        n1   = gradient_noise ({vertex.x, vertex.y, vertex.z}, {x1, y1, z1}, seed);
        ix1  = linear_interp(n0, n1, xs);
        iy1  = linear_interp(ix0, ix1, ys);

        return linear_interp(iy0, iy1, zs);
    }

    float HiveEngine::gradient_noise(glm::vec3 f, glm::ivec3 i, int seed) {
        int vectorIndex = X_NOISE_GEN * i.x
                          + Y_NOISE_GEN * i.y
                          + Z_NOISE_GEN * i.z
                          + SEED_NOISE_GEN * seed
                          & 0xffffffff;
        vectorIndex ^= (vectorIndex >> SHIFT_NOISE_GEN);
        vectorIndex &= 0xff;

        float xvGradient = g_randomVectors[(vectorIndex << 2)    ];
        float yvGradient = g_randomVectors[(vectorIndex << 2) + 1];
        float zvGradient = g_randomVectors[(vectorIndex << 2) + 2];

        // Set up us another vector equal to the distance between the two vectors
        // passed to this function.
        float xvPoint = (f.x - (float)i.x);
        float yvPoint = (f.y - (float)i.y);
        float zvPoint = (f.z - (float)i.z);

        // Now compute the dot product of the gradient vector with the distance
        // vector.  The resulting value is gradient noise.  Apply a scaling value
        // so that this noise value ranges from -1.0 to 1.0.
        return ((xvGradient * xvPoint)
                + (yvGradient * yvPoint)
                + (zvGradient * zvPoint)) * 2.12;
    }

    NoiseModule::NoiseModule() {

    }
}




