//
// Created by calper on 5/7/19.
//

#include <HiveEngine/Renderer/LineDrawing.h>

HiveEngine::Renderer::LineDrawing::LineDrawing(HiveEngine::Renderer::Directive *directive,
                                             HiveEngine::Renderer::Camera *camera) : Drawing(directive) {
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

    this->camera = camera;

    CameraPackage package;
    camera_buffer.add(package);
    camera_gpu_buffer.cpu_buffer = &camera_buffer;
    camera_gpu_buffer.usage = (VkBufferUsageFlagBits) (VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT |
                                                       VK_BUFFER_USAGE_TRANSFER_DST_BIT);
}

void HiveEngine::Renderer::LineDrawing::init(VkRenderPass render_pass) {
    Drawing::init(render_pass);

    std::array<VkDescriptorPoolSize, 1> poolSize = {};
    poolSize[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSize[0].descriptorCount = 1;

    VkDescriptorPoolCreateInfo poolInfo = {};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = poolSize.size();
    poolInfo.pPoolSizes = poolSize.data();
    poolInfo.maxSets = 1;

    if (vkCreateDescriptorPool(get_context()->get_device(), &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS) {
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
        VK_SUCCESS) {
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

void HiveEngine::Renderer::LineDrawing::update() {
    if (point_allocation == nullptr) line_buffer.mark_changed();

    VmaAllocator allocator = get_context()->get_allocator();
    camera_gpu_buffer.allocator = allocator;

    CameraPackage package;
    if (camera)
        package = camera->get_package();
    camera_buffer.set(0, package);
    camera_gpu_buffer.update();

    if (line_buffer.is_changed()) {
        point_allocation = nullptr;
        vk_point_buffer = nullptr;

        auto points = line_buffer.get_data();
        auto states = line_buffer.get_state();

        VkBufferCreateInfo bufferInfo = {VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO};

        VmaAllocationCreateInfo allocInfo = {};
        allocInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;

        bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
        bufferInfo.size = sizeof(HiveEngine::Line) * line_buffer.size();
        vmaCreateBuffer(get_context()->get_allocator(), &bufferInfo, &allocInfo, &vk_point_buffer, &point_allocation,
                        nullptr);


        if (line_buffer.size() > 0) {
            void *data;

            vmaMapMemory(get_context()->get_allocator(), point_allocation, &data);
            memcpy(data, points.data(), (size_t) sizeof(HiveEngine::Line) * line_buffer.size());
            vmaUnmapMemory(get_context()->get_allocator(), point_allocation);
        }

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

        line_buffer.mark_unchanged();
    }

}

void HiveEngine::Renderer::LineDrawing::draw(VkCommandBuffer cmd_buffer) {
    if (line_buffer.size() > 0) {
        this->update();

        vkCmdBindPipeline(cmd_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);

        VkDeviceSize offsets[] = {0};
        vkCmdBindVertexBuffers(cmd_buffer, 0, 1, &vk_point_buffer, offsets);

        vkCmdBindDescriptorSets(cmd_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &descriptorSet, 0,
                                nullptr);

        vkCmdDraw(cmd_buffer, line_buffer.size() * 2, 1, 0, 0);
    }
}

void HiveEngine::Renderer::LineDrawing::cleanup() {
    vmaDestroyBuffer(get_context()->get_allocator(), vk_point_buffer, point_allocation);
    point_allocation = nullptr;
    vk_point_buffer = nullptr;

    camera_gpu_buffer.cleanup();

    vkDestroyDescriptorSetLayout(get_context()->get_device(), descriptorSetLayout, nullptr);
    vkDestroyDescriptorPool(get_context()->get_device(), descriptorPool, nullptr);

    line_buffer.mark_changed();
    vkDestroyPipeline(get_context()->get_device(), graphicsPipeline, nullptr);
    vkDestroyPipelineLayout(get_context()->get_device(), pipelineLayout, nullptr);
    Drawing::cleanup();

}

namespace HiveEngine::Renderer {
    LineDescription LineDrawing::add_line(glm::vec3 a, glm::vec4 ac, glm::vec3 b, glm::vec4 bc) {
        Line data = {};
        LineDescription ld = {};

        data.a.position = a;
        data.b.position = b;
        data.a.color = ac;
        data.b.color = bc;

        ld.id = line_buffer.add(data);
        return ld;
    }

    void LineDrawing::remove_line(LineDescription ld) {
        Line data = {};
        line_buffer.set(ld.id, data);
        line_buffer.remove(ld.id);
    }

    LineDescription LineDrawing::add_line(Line line) {
        Line data = line;
        LineDescription ld = {};
        ld.id = line_buffer.add(data);
        return ld;
    }

    void LineDrawing::refresh_line(LineDescription ld, Line new_line) {
        line_buffer.set(ld.id, new_line);
    }
}
