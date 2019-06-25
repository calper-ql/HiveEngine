//
// Created by calpe on 6/18/2019.
//

#ifndef HIVEENGINE_OPTIXDRAWING_H
#define HIVEENGINE_OPTIXDRAWING_H

#include <map>

#include <HiveEngine/Renderer/Drawing.h>
#include <optixu/optixpp_namespace.h>
#include <optix.h>
#include <optixu/optixu_math_namespace.h>
#include <optixu/optixu_matrix_namespace.h>

namespace HiveEngine::Renderer {
    struct OptixDrawingPerspective {
        GLint texture_id;
        glm::vec3 position;
        glm::mat4 orientation;
        float fov; // radians

    };

    class OptixDrawing : public Drawing {
    public:

        optix::Context rtx_context;
        std::map<std::string, optix::Program> program_space;

        OptixDrawing(Context* context, std::string program_space_path);
        ~OptixDrawing();

        void draw() override;
    };
}

#endif //HIVEENGINE_OPTIXDRAWING_H
