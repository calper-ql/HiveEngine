//
// Created by calpe on 7/31/2019.
//

#ifndef HIVEENGINE_MESHDRAWING_H
#define HIVEENGINE_MESHDRAWING_H

#include <HiveEngine/Renderer/Drawing.h>
#include <HiveEngine/Common.h>
#include <HiveEngine/Renderer/GPUBuffer.hpp>
#include <HiveEngine/Renderer/Camera.h>
#include <HiveEngine/Mesh.h>
#include <HiveEngine/Context.h>

namespace HiveEngine::Renderer {

    struct MeshDrawingTransform {
        glm::mat3 rotation = {};
        glm::vec3 position = {};
        glm::vec3 scale = {};
    };

    class MeshDrawing : public Drawing {
    public:
        Mesh mesh;

        Buffer<glm::vec3> vertex_buffer;
        Buffer<glm::uvec3> index_buffer;
        Buffer<MeshDrawingTransform> transform_buffer;
        Buffer<CameraPackage> camera_buffer;

        GPUBuffer<glm::vec3> vertex_gpu_buffer;
        GPUBuffer<glm::uvec3> index_gpu_buffer;
        GPUBuffer<MeshDrawingTransform> transform_gpu_buffer;
        GPUBuffer<CameraPackage> camera_gpu_buffer;

        VkPipeline graphicsPipeline;
        VkPipelineLayout pipelineLayout;

        std::array<VkVertexInputBindingDescription, 2> bindingDescriptions = {};
        std::array<VkVertexInputAttributeDescription, 6> attributeDescriptions = {};

        VkDescriptorPool descriptorPool;
        VkDescriptorSetLayout descriptorSetLayout;
        VkDescriptorSet descriptorSet;

        float line_width = 1;

        Camera *camera;

        MeshDrawing(Directive *directive, Mesh mesh, Camera *camera=nullptr);

        void init(VkRenderPass render_pass) override;

        void update();

        void draw(VkCommandBuffer cmd_buffer) override;

        void cleanup() override;
    };

    class MeshDrawingHandler : public ContextRepresentation {
    public:
        Buffer<Buffer<MeshDrawing*>> scenes;

        NodeRepresentation *create_node_representation(int scene_id, int mesh_id) override;

        void update_position(glm::dvec3 new_position) override;

        MeshDrawingHandler();

        ~MeshDrawingHandler() override;

        void add_scene(Directive* directive, std::vector<Mesh> meshes, Camera* camera=nullptr);

        void remove_scene(size_t idx);
    };

    class MeshDrawingNodeRepresentation : public NodeRepresentation {
    public:
        MeshDrawingHandler* mdh;
        size_t instance_id = 0;
        bool success = false;

        virtual ~MeshDrawingNodeRepresentation();

        MeshDrawingNodeRepresentation(int scene_id, int mesh_id, MeshDrawingHandler* mdh);

        void update_global_orientation(glm::dvec3 position, glm::mat3 rotation) override;

        void update_orientation(glm::dvec3 position, glm::mat3 rotation) override;
    };
}

#endif //HIVEENGINE_MESHDRAWING_H
