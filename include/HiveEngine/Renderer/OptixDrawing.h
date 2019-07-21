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

#include <HiveEngine/Context.h>

namespace HiveEngine::Renderer {
    class OptixDrawing;

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

    class OptixDrawingContextRepresentation : public ContextRepresentation {
    public:
        OptixDrawing* drawing;
        optix::Transform transform;
        optix::Group group;

        OptixDrawingContextRepresentation(OptixDrawing* drawing);

        virtual ~OptixDrawingContextRepresentation();

        NodeRepresentation *create_node_representation(int scene_id, int mesh_id) override;

        void update_position(glm::dvec3 new_position) override;
    };

    class OptixDrawingNodeRepresentation : public NodeRepresentation {
    public:
        OptixDrawingContextRepresentation* context_representation;
        optix::Transform transform;

        OptixDrawingNodeRepresentation(OptixDrawingContextRepresentation* context_representation, int scene_id, int mesh_id);

        ~OptixDrawingNodeRepresentation() override;

        void update_global_orientation(glm::dvec3 position, glm::mat3 rotation) override;

        void update_orientation(glm::dvec3 position, glm::mat3 rotation) override;
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

    public:

        optix::Context rtx_context;
        optix::Group root_node;
        std::vector<std::vector<optix::GeometryGroup>> scenes;

        std::map<std::string, optix::Program> program_space;

        OptixDrawing(Context* context, std::string program_space_path);
        ~OptixDrawing() override;

        void draw() override;


        size_t add_perspective(Camera* camera, glm::uvec2 size, bool window_bound, std::string raygen_program, std::string miss_program, std::string exception_program);
        void resize_perspective(size_t idx, glm:: uvec2 new_size);
        void remove_perspective(size_t idx);

        std::vector<optix::GeometryGroup> extract_geometry_groups(aiScene* scene);
        size_t add_scene(aiScene* scene);

        ContextRepresentation* create_context_representation();
    };
}

#endif //HIVEENGINE_OPTIXDRAWING_H
