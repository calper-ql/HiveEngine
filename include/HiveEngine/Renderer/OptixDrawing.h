//
// Created by calpe on 6/18/2019.
//

#ifndef HIVEENGINE_OPTIXDRAWING_H
#define HIVEENGINE_OPTIXDRAWING_H

#include <map>

#include <HiveEngine/Renderer/Camera.h>
#include <HiveEngine/Renderer/Drawing.h>

#include <optixu/optixpp_namespace.h>
#include <optix.h>
#include <optixu/optixu_math_namespace.h>
#include <optixu/optixu_matrix_namespace.h>

#include <HiveEngine/Buffer.hpp>

#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags


namespace HiveEngine::Renderer {

    struct OptixDrawingPerspective {
        GLuint texture_id;

        unsigned width;
        unsigned height;
        bool window_bound = false;
        float focal_distance = 0.1;

        optix::Buffer image;

        size_t id;

        float position;

        std::string raygen_program;
        std::string miss_program;
        std::string exception_program;
    };

    class OptixDrawing : public Drawing {
    private:
        Buffer<OptixDrawingPerspective> perspectives;
        Buffer<Camera*> cameras;

        GLuint VBO, VAO, EBO;
        GLuint program;

        GLuint vert_shader;
        GLuint frag_shader;

        GLuint gl_tex_id;

        void configure_node(aiNode* node, std::vector<optix::GeometryGroup>& ggs, optix::Group group);

    public:

        optix::Context rtx_context;
        optix::Group root_node;

        std::map<std::string, optix::Program> program_space;

        OptixDrawing(Context* context, std::string program_space_path);
        ~OptixDrawing() override;

        void draw() override;


        size_t add_perspective(Camera* camera, glm::uvec2 size, bool window_bound, std::string raygen_program, std::string miss_program, std::string exception_program);
        void resize_perspective(size_t idx, glm:: uvec2 new_size);
        void remove_perspective(size_t idx);

        std::vector<optix::GeometryGroup> extract_geometry_groups(aiScene* scene);
        optix::Group configure_scene(aiScene* scene);
    };
}

#endif //HIVEENGINE_OPTIXDRAWING_H
