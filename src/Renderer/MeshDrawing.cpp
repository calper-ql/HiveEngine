//
// Created by calpe on 7/31/2019.
//

#include <HiveEngine/Renderer/MeshDrawing.h>

//
// Created by calper on 5/7/19.
//

#include <HiveEngine/Renderer/AABBDrawing.h>

namespace HiveEngine::Renderer
{

    MeshDrawing::MeshDrawing(Directive *directive, Mesh mesh, Camera *camera) : Drawing(directive)
    {
        bindingDescriptions[0].binding = 0;
        bindingDescriptions[0].stride = sizeof(glm::vec3);
        bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        bindingDescriptions[1].binding = 1;
        bindingDescriptions[1].stride = sizeof(MeshDrawingTransform);
        bindingDescriptions[1].inputRate = VK_VERTEX_INPUT_RATE_INSTANCE;

        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[0].offset = 0;

        attributeDescriptions[1].binding = 1;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[1].offset = offsetof(MeshDrawingTransform, rotation) + sizeof(glm::vec3) * 0;

        attributeDescriptions[2].binding = 1;
        attributeDescriptions[2].location = 2;
        attributeDescriptions[2].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[2].offset = offsetof(MeshDrawingTransform, rotation) + sizeof(glm::vec3) * 1;

        attributeDescriptions[3].binding = 1;
        attributeDescriptions[3].location = 3;
        attributeDescriptions[3].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[3].offset = offsetof(MeshDrawingTransform, rotation) + sizeof(glm::vec3) * 2;

        attributeDescriptions[4].binding = 1;
        attributeDescriptions[4].location = 4;
        attributeDescriptions[4].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[4].offset = offsetof(MeshDrawingTransform, position);

        attributeDescriptions[5].binding = 1;
        attributeDescriptions[5].location = 5;
        attributeDescriptions[5].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[5].offset = offsetof(MeshDrawingTransform, scale);

        CameraPackage package;
        camera_buffer.add(package);

        vertex_gpu_buffer.cpu_buffer = &vertex_buffer;
        index_gpu_buffer.cpu_buffer = &index_buffer;
        transform_gpu_buffer.cpu_buffer = &transform_buffer;
        camera_gpu_buffer.cpu_buffer = &camera_buffer;

        vertex_gpu_buffer.usage = (VkBufferUsageFlagBits)(VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT);
        index_gpu_buffer.usage = (VkBufferUsageFlagBits)(VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT);
        transform_gpu_buffer.usage = (VkBufferUsageFlagBits)(VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT);
        camera_gpu_buffer.usage = (VkBufferUsageFlagBits)(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT);

        this->camera = camera;

        this->mesh = mesh;
        vertex_buffer.flush(mesh.vertices);
        index_buffer.flush(mesh.indices);
    }

    void MeshDrawing::init(VkRenderPass render_pass)
    {
        Drawing::init(render_pass);

        std::array<VkDescriptorPoolSize, 1> poolSize = {};
        poolSize[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        poolSize[0].descriptorCount = 1;

        VkDescriptorPoolCreateInfo poolInfo = {};
        poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolInfo.poolSizeCount = poolSize.size();
        poolInfo.pPoolSizes = poolSize.data();
        poolInfo.maxSets = 1;

        if (vkCreateDescriptorPool(get_context()->get_device(), &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create descriptor pool!");
        }

        std::array<VkDescriptorSetLayoutBinding, 1> bindings = {};
        bindings[0].binding = 0;
        bindings[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        bindings[0].descriptorCount = 1;
        bindings[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
        bindings[0].pImmutableSamplers = nullptr;

        VkDescriptorSetLayoutCreateInfo layoutInfo = {};
        layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layoutInfo.bindingCount = bindings.size();
        layoutInfo.pBindings = bindings.data();

        if (vkCreateDescriptorSetLayout(get_context()->get_device(), &layoutInfo, nullptr, &descriptorSetLayout) !=
            VK_SUCCESS)
        {
            throw std::runtime_error("failed to create descriptor set layout!");
        }

        VkDescriptorSetAllocateInfo allocInfo = {};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = descriptorPool;
        allocInfo.descriptorSetCount = 1;
        allocInfo.pSetLayouts = &descriptorSetLayout;

        if (vkAllocateDescriptorSets(get_context()->get_device(), &allocInfo, &descriptorSet) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to allocate descriptor sets!");
        }

        VkPipelineShaderStageCreateInfo vertShaderStageInfo = {};
        vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
        vertShaderStageInfo.module = get_context()->get_shaders()["mesh_shader.vert"];
        vertShaderStageInfo.pName = "main";

        VkPipelineShaderStageCreateInfo fragShaderStageInfo = {};
        fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        fragShaderStageInfo.module = get_context()->get_shaders()["mesh_shader.frag"];
        fragShaderStageInfo.pName = "main";

        VkPipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo, fragShaderStageInfo};

        VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
        vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInputInfo.vertexBindingDescriptionCount = bindingDescriptions.size();
        vertexInputInfo.vertexAttributeDescriptionCount = attributeDescriptions.size();
        vertexInputInfo.pVertexBindingDescriptions = bindingDescriptions.data();
        vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

        VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
        inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        inputAssembly.primitiveRestartEnable = VK_FALSE;

        VkViewport viewport = {};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = (float)get_context()->get_swap_chain_extent().width;
        viewport.height = (float)get_context()->get_swap_chain_extent().height;
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;

        VkRect2D scissor = {};
        scissor.offset = {0, 0};
        scissor.extent = get_context()->get_swap_chain_extent();

        VkPipelineViewportStateCreateInfo viewportState = {};
        viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportState.viewportCount = 1;
        viewportState.pViewports = &viewport;
        viewportState.scissorCount = 1;
        viewportState.pScissors = &scissor;

        VkPipelineRasterizationStateCreateInfo rasterizer = {};
        rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterizer.depthClampEnable = VK_FALSE;
        rasterizer.rasterizerDiscardEnable = VK_FALSE;
        rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
        rasterizer.lineWidth = line_width;
        rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
        rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
        rasterizer.depthBiasEnable = VK_FALSE;

        VkPipelineMultisampleStateCreateInfo multisampling = {};
        multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampling.sampleShadingEnable = VK_FALSE;
        multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

        VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
        colorBlendAttachment.colorWriteMask =
                VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT |
                VK_COLOR_COMPONENT_A_BIT;
        colorBlendAttachment.blendEnable = VK_FALSE;

        VkPipelineColorBlendStateCreateInfo colorBlending = {};
        colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colorBlending.logicOpEnable = VK_FALSE;
        colorBlending.logicOp = VK_LOGIC_OP_COPY;
        colorBlending.attachmentCount = 1;
        colorBlending.pAttachments = &colorBlendAttachment;
        colorBlending.blendConstants[0] = 0.0f;
        colorBlending.blendConstants[1] = 0.0f;
        colorBlending.blendConstants[2] = 0.0f;
        colorBlending.blendConstants[3] = 0.0f;

        VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 1;
        pipelineLayoutInfo.pushConstantRangeCount = 0;
        pipelineLayoutInfo.pSetLayouts = &descriptorSetLayout;

        if (vkCreatePipelineLayout(get_context()->get_device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) !=
            VK_SUCCESS)
        {
            throw std::runtime_error("failed to create pipeline layout!");
        }

        VkGraphicsPipelineCreateInfo pipelineInfo = {};
        pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipelineInfo.stageCount = 2;
        pipelineInfo.pStages = shaderStages;
        pipelineInfo.pVertexInputState = &vertexInputInfo;
        pipelineInfo.pInputAssemblyState = &inputAssembly;
        pipelineInfo.pViewportState = &viewportState;
        pipelineInfo.pRasterizationState = &rasterizer;
        pipelineInfo.pMultisampleState = &multisampling;
        pipelineInfo.pColorBlendState = &colorBlending;
        pipelineInfo.layout = pipelineLayout;
        pipelineInfo.renderPass = render_pass;
        pipelineInfo.subpass = 0;
        pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

        if (vkCreateGraphicsPipelines(get_context()->get_device(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr,
                                      &graphicsPipeline) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create graphics pipeline!");
        }
    }

    void MeshDrawing::update()
    {
        VmaAllocator allocator = get_context()->get_allocator();

        vertex_gpu_buffer.allocator = allocator;
        index_gpu_buffer.allocator = allocator;
        transform_gpu_buffer.allocator = allocator;
        camera_gpu_buffer.allocator = allocator;

        vertex_gpu_buffer.update();
        index_gpu_buffer.update();
        transform_gpu_buffer.update();

        vertex_buffer.mark_unchanged();
        index_buffer.mark_unchanged();
        transform_buffer.mark_unchanged();

        CameraPackage package;
        if (camera)
            package = camera->get_package();
        camera_buffer.set(0, package);
        camera_gpu_buffer.update();

        if(camera_gpu_buffer.recreated){
            VkDescriptorBufferInfo cameraBufferInfo = {};
            cameraBufferInfo.buffer = camera_gpu_buffer.buffer;
            cameraBufferInfo.offset = 0;
            cameraBufferInfo.range = sizeof(CameraPackage);

            VkWriteDescriptorSet cameraWrite = {};
            cameraWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            cameraWrite.dstSet = descriptorSet;
            cameraWrite.dstBinding = 0;
            cameraWrite.dstArrayElement = 0;
            cameraWrite.descriptorCount = 1;
            cameraWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            cameraWrite.pBufferInfo = &cameraBufferInfo;
            cameraWrite.pImageInfo = nullptr;
            cameraWrite.pTexelBufferView = nullptr;

            std::array<VkWriteDescriptorSet, 1> write_descriptors = {cameraWrite};

            vkUpdateDescriptorSets(get_context()->get_device(), write_descriptors.size(), write_descriptors.data(), 0,
                                   nullptr);

            camera_gpu_buffer.recreated = false;
        }

    }

    void MeshDrawing::draw(VkCommandBuffer cmd_buffer) {
        if (transform_buffer.size() > 0) {
            VkDeviceSize offsets[1] = {0};
            update();

			vkCmdBindPipeline(cmd_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);

			vkCmdBindVertexBuffers(cmd_buffer, 0, 1, &vertex_gpu_buffer.buffer, offsets);
			vkCmdBindVertexBuffers(cmd_buffer, 1, 1, &transform_gpu_buffer.buffer, offsets);
			vkCmdBindIndexBuffer(cmd_buffer, index_gpu_buffer.buffer, 0, VK_INDEX_TYPE_UINT32);

			vkCmdBindDescriptorSets(cmd_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &descriptorSet, 0, nullptr);

			vkCmdDrawIndexed(cmd_buffer, static_cast<uint32_t>(index_buffer.size() * 3), transform_buffer.size(), 0, 0, 0);
        }
    }

    void MeshDrawing::cleanup()
    {
        vertex_gpu_buffer.cleanup();
        index_gpu_buffer.cleanup();
        transform_gpu_buffer.cleanup();
        camera_gpu_buffer.cleanup();

        vkDestroyDescriptorSetLayout(get_context()->get_device(), descriptorSetLayout, nullptr);
        vkDestroyDescriptorPool(get_context()->get_device(), descriptorPool, nullptr);

        vkDestroyPipeline(get_context()->get_device(), graphicsPipeline, nullptr);
        vkDestroyPipelineLayout(get_context()->get_device(), pipelineLayout, nullptr);

        Drawing::cleanup();
    }

    MeshDrawingHandler::MeshDrawingHandler() : ContextRepresentation() {

    }

    void MeshDrawingHandler::add_scene(Directive* directive, std::vector<Mesh> meshes, Camera* camera) {
        Buffer<MeshDrawing*> drawings;

        for (auto mesh: meshes) {
            drawings.add(new MeshDrawing(directive, mesh, camera));
        }

        scenes.add(drawings);
    }

    void MeshDrawingHandler::remove_scene(size_t idx) {
        if(scenes.size() >= idx) return;
        if(scenes.get_state(idx)){
            auto scene = scenes.get(idx);
            for (int i = 0; i < scene.size(); ++i) {
                if(scene.get_state(i)){
                    delete scene.get(i);
                }
            }
            scenes.remove(idx);
        }
    }

    MeshDrawingHandler::~MeshDrawingHandler() {
        for (int i = 0; i < scenes.size(); ++i) {
            remove_scene(i);
        }
    }

    NodeRepresentation *MeshDrawingHandler::create_node_representation(int scene_id, int mesh_id) {
        return new MeshDrawingNodeRepresentation(scene_id, mesh_id, this);
    }

    void MeshDrawingHandler::update_position(glm::dvec3 new_position) {

    }


    void MeshDrawingNodeRepresentation::update_global_orientation(glm::dvec3 position, glm::mat3 rotation) {
        if(success && mdh->scenes.get_state(get_scene_id())) {
            if (mdh->scenes.get(get_scene_id()).get_state(get_mesh_id())) {
                auto mesh = mdh->scenes.get(get_scene_id()).get(get_mesh_id());
                MeshDrawingTransform transform;
                transform.scale = {1.0, 1.0, 1.0};
                transform.position = position;
                transform.rotation = rotation;
                mesh->transform_buffer.set(instance_id, transform);
            }
        }
    }

    void MeshDrawingNodeRepresentation::update_orientation(glm::dvec3 position, glm::mat3 rotation) {

    }

    MeshDrawingNodeRepresentation::MeshDrawingNodeRepresentation(int scene_id, int mesh_id, MeshDrawingHandler* mdh) : NodeRepresentation(
            scene_id, mesh_id) {
            this->mdh = mdh;
			if (scene_id < 0 && mesh_id < 0) return;
            if(mdh->scenes.get_state(scene_id)){
                if(mdh->scenes.get(scene_id).get_state(mesh_id)){
                    auto mesh = mdh->scenes.get(scene_id).get(mesh_id);
                    this->instance_id = mesh->transform_buffer.add(MeshDrawingTransform());
                    success = true;
                }
            }


    }

    MeshDrawingNodeRepresentation::~MeshDrawingNodeRepresentation() {
        if(success && mdh->scenes.get_state(get_scene_id())) {
            if (mdh->scenes.get(get_scene_id()).get_state(get_mesh_id())) {
                auto mesh = mdh->scenes.get(get_scene_id()).get(get_mesh_id());
                MeshDrawingTransform transform;
                mesh->transform_buffer.set(instance_id, transform);
                mesh->transform_buffer.remove(instance_id);
            }
        }
    }
} // namespace HiveEngine::Renderer
