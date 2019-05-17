//
// Created by calper on 5/7/19.
//

#include <HiveEngine/Renderer/LineDrawing.h>

HiveEngineRenderer::LineDrawing::LineDrawing(HiveEngineRenderer::Directive *directive, HiveEngineRenderer::Camera* camera) : Drawing(directive) {
    bindingDescriptions[0].binding = 0;
    bindingDescriptions[0].stride = sizeof(HiveEngine::Point);
    bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    attributeDescriptions[0].binding = 0;
    attributeDescriptions[0].location = 0;
    attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[0].offset = offsetof(HiveEngine::Point, position);
    attributeDescriptions[1].binding = 0;
    attributeDescriptions[1].location = 1;
    attributeDescriptions[1].format = VK_FORMAT_R32G32B32A32_SFLOAT;
    attributeDescriptions[1].offset = offsetof(HiveEngine::Point, color);

    if(camera == nullptr){
        throw std::runtime_error("LineDrawing -> camera was nullptr!");
    }
    this->camera = camera;

}

void HiveEngineRenderer::LineDrawing::init(VkRenderPass render_pass) {
        Drawing::init(render_pass);

        std::array<VkDescriptorPoolSize, 2> poolSize = {};
        poolSize[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        poolSize[0].descriptorCount = 1;
        poolSize[1].type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        poolSize[1].descriptorCount = 1;

        VkDescriptorPoolCreateInfo poolInfo = {};
        poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolInfo.poolSizeCount = poolSize.size();
        poolInfo.pPoolSizes = poolSize.data();
        poolInfo.maxSets = 1;

        if (vkCreateDescriptorPool(get_context()->get_device(), &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS) {
            throw std::runtime_error("failed to create descriptor pool!");
        }

        std::array<VkDescriptorSetLayoutBinding, 2> bindings = {};

        bindings[0].binding = 0;
        bindings[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        bindings[0].descriptorCount = 1;
        bindings[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
        bindings[0].pImmutableSamplers = nullptr;

        bindings[1].binding = 1;
        bindings[1].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        bindings[1].descriptorCount = 1;
        bindings[1].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
        bindings[1].pImmutableSamplers = nullptr;

        VkDescriptorSetLayoutCreateInfo layoutInfo = {};
        layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layoutInfo.bindingCount = bindings.size();
        layoutInfo.pBindings = bindings.data();

        if (vkCreateDescriptorSetLayout(get_context()->get_device(), &layoutInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS) {
            throw std::runtime_error("failed to create descriptor set layout!");
        }

        VkDescriptorSetAllocateInfo allocInfo = {};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = descriptorPool;
        allocInfo.descriptorSetCount = 1;
        allocInfo.pSetLayouts = &descriptorSetLayout;

        if (vkAllocateDescriptorSets(get_context()->get_device(), &allocInfo, &descriptorSet) != VK_SUCCESS) {
            throw std::runtime_error("failed to allocate descriptor sets!");
        }

        VkPipelineShaderStageCreateInfo vertShaderStageInfo = {};
        vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
        vertShaderStageInfo.module = get_context()->get_shaders()["line_shader.vert"];
        vertShaderStageInfo.pName = "main";

        VkPipelineShaderStageCreateInfo fragShaderStageInfo = {};
        fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        fragShaderStageInfo.module = get_context()->get_shaders()["line_shader.frag"];
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
        inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
        inputAssembly.primitiveRestartEnable = VK_FALSE;

        VkViewport viewport = {};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = (float) get_context()->get_swap_chain_extent().width;
        viewport.height = (float) get_context()->get_swap_chain_extent().height;
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
            VK_SUCCESS) {
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
                                      &graphicsPipeline) != VK_SUCCESS) {
            throw std::runtime_error("failed to create graphics pipeline!");
        }


}

void HiveEngineRenderer::LineDrawing::update() {
    if(point_allocation == nullptr) line_buffer.mark_changed();

    if(vk_camera_buffer == nullptr){
        auto package = camera->get_package();

        VkBufferCreateInfo bufferInfo = {VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO};
        bufferInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
        bufferInfo.size = sizeof(package);

        VmaAllocationCreateInfo allocInfo = {};
        allocInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;

        vmaCreateBuffer(get_context()->get_allocator(), &bufferInfo, &allocInfo, &vk_camera_buffer, &camera_allocation, nullptr);
    }

    {
        auto package = camera->get_package();
        void *data;

        vmaMapMemory(get_context()->get_allocator(), camera_allocation, &data);
        memcpy(data, &package, sizeof(package));
        vmaUnmapMemory(get_context()->get_allocator(), camera_allocation);
    }

    if(line_buffer.is_changed()){
        vmaDestroyBuffer(get_context()->get_allocator(), vk_point_buffer, point_allocation);
        vmaDestroyBuffer(get_context()->get_allocator(), vk_state_buffer, state_allocation);
        point_allocation = nullptr;
        vk_point_buffer = nullptr;
        state_allocation = nullptr;
        vk_state_buffer = nullptr;

        auto points = line_buffer.get_data();
        auto states = line_buffer.get_state();

        VkBufferCreateInfo bufferInfo = {VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO};

        VmaAllocationCreateInfo allocInfo = {};
        allocInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;

        bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
        bufferInfo.size = sizeof(HiveEngine::Line) * line_buffer.size();
        vmaCreateBuffer(get_context()->get_allocator(), &bufferInfo, &allocInfo, &vk_point_buffer, &point_allocation, nullptr);

        bufferInfo.usage =  VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
        bufferInfo.size = sizeof(int) * line_buffer.size();
        vmaCreateBuffer(get_context()->get_allocator(), &bufferInfo, &allocInfo, &vk_state_buffer, &state_allocation, nullptr);

        if(line_buffer.size() > 0){
            void *data;

            vmaMapMemory(get_context()->get_allocator(), point_allocation, &data);
            memcpy(data, points.data(), (size_t) sizeof(HiveEngine::Line) * line_buffer.size());
            vmaUnmapMemory(get_context()->get_allocator(), point_allocation);

            vmaMapMemory(get_context()->get_allocator(), state_allocation, &data);
            memcpy(data, states.data(), (size_t) sizeof(int) * line_buffer.size());
            vmaUnmapMemory(get_context()->get_allocator(), state_allocation);
        }

        VkDescriptorBufferInfo cameraBufferInfo = {};
        cameraBufferInfo.buffer = vk_camera_buffer;
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

        VkDescriptorBufferInfo stateStorageBufferInfo = {};
        stateStorageBufferInfo.buffer = vk_state_buffer;
        stateStorageBufferInfo.offset = 0;
        stateStorageBufferInfo.range = (size_t) sizeof(int) * line_buffer.size();

        VkWriteDescriptorSet stateStorageWrite = {};
        stateStorageWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        stateStorageWrite.dstSet = descriptorSet;
        stateStorageWrite.dstBinding = 1;
        stateStorageWrite.dstArrayElement = 0;
        stateStorageWrite.descriptorCount = 1;
        stateStorageWrite.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        stateStorageWrite.pBufferInfo = &stateStorageBufferInfo;
        stateStorageWrite.pImageInfo = nullptr;
        stateStorageWrite.pTexelBufferView = nullptr;

        std::array<VkWriteDescriptorSet, 2> write_descriptors = {cameraWrite, stateStorageWrite};

        vkUpdateDescriptorSets(get_context()->get_device(), write_descriptors.size(), write_descriptors.data(), 0, nullptr);

        line_buffer.mark_unchanged();
    }

}

void HiveEngineRenderer::LineDrawing::draw(VkCommandBuffer cmd_buffer) {
    if(line_buffer.size() > 0){
        this->update();

        vkCmdBindPipeline(cmd_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);

        VkDeviceSize offsets[] = {0};
        vkCmdBindVertexBuffers(cmd_buffer, 0, 1, &vk_point_buffer, offsets);

        vkCmdBindDescriptorSets(cmd_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &descriptorSet, 0, nullptr);

        vkCmdDraw(cmd_buffer, line_buffer.size()*2, 1, 0, 0);
    }
}

void HiveEngineRenderer::LineDrawing::cleanup() {
        vmaDestroyBuffer(get_context()->get_allocator(), vk_point_buffer, point_allocation);
        vmaDestroyBuffer(get_context()->get_allocator(), vk_state_buffer, state_allocation);
        point_allocation = nullptr;
        vk_point_buffer = nullptr;
        state_allocation = nullptr;
        vk_state_buffer = nullptr;

        vmaDestroyBuffer(get_context()->get_allocator(), vk_camera_buffer, camera_allocation);
        vk_camera_buffer = nullptr;
        camera_allocation = nullptr;

        vkDestroyDescriptorSetLayout(get_context()->get_device(), descriptorSetLayout, nullptr);
        vkDestroyDescriptorPool(get_context()->get_device(), descriptorPool, nullptr);

        line_buffer.mark_changed();
        vkDestroyPipeline(get_context()->get_device(), graphicsPipeline, nullptr);
        vkDestroyPipelineLayout(get_context()->get_device(), pipelineLayout, nullptr);
        Drawing::cleanup();

}
