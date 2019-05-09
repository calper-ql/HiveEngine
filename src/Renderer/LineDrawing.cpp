//
// Created by calper on 5/7/19.
//

#include <HiveEngine/Renderer/LineDrawing.h>

HiveEngineRenderer::LineDrawing::LineDrawing(HiveEngineRenderer::Directive *directive) : Drawing(directive) {
    std::uniform_real_distribution<float> d(-1.0f, 1.0f);
    std::uniform_real_distribution<float> c(0.0f, 1.0f);

    glm::vec3 last_point = {d(g), d(g), d(g)};
    glm::vec4 last_color = {d(g), d(g), d(g), 1.0f};
    for (int i = 0; i < 1000; ++i) {
        glm::vec3 next_point = {d(g), d(g), d(g)};
        glm::vec4 next_color = {d(g), d(g), d(g), 1.0f};

        HiveEngine::Point p1;
        HiveEngine::Point p2;

        p1.position = last_point;
        p1.color = last_color;

        p2.position = next_point;
        p2.color = next_color;

        points.push_back(p1);
        points.push_back(p2);
        last_point = next_point;
        last_color = next_color;
    }
}

void HiveEngineRenderer::LineDrawing::init(VkRenderPass render_pass) {
    if (!is_inited()) {
        Drawing::init(render_pass);
        VkVertexInputBindingDescription bindingDescription = {};
        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(HiveEngine::Point);
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions = {};
        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
        attributeDescriptions[0].offset = offsetof(HiveEngine::Point, position);
        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = VK_FORMAT_R32G32B32A32_SFLOAT;
        attributeDescriptions[1].offset = offsetof(HiveEngine::Point, color);

        VkBufferCreateInfo bufferInfo = {VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO};
        bufferInfo.size = sizeof(HiveEngine::Point) * points.size();
        bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;

        VmaAllocationCreateInfo allocInfo = {};
        allocInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;
        vmaCreateBuffer(get_context()->get_allocator(), &bufferInfo, &allocInfo, &pointBuffer, &allocation, nullptr);

        void *data;
        vmaMapMemory(get_context()->get_allocator(), allocation, &data);
        memcpy(data, points.data(), (size_t) bufferInfo.size);
        vmaUnmapMemory(get_context()->get_allocator(), allocation);

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
        vertexInputInfo.vertexBindingDescriptionCount = 1;
        vertexInputInfo.vertexAttributeDescriptionCount = attributeDescriptions.size();
        vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
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
        pipelineLayoutInfo.setLayoutCount = 0;
        pipelineLayoutInfo.pushConstantRangeCount = 0;

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
}

void HiveEngineRenderer::LineDrawing::update() {
    std::uniform_real_distribution<float> d(-0.001f, 0.001f);
    float clamp = 0.9;

    for (auto &p : points) {
        p.position += glm::vec3({d(g), d(g), d(g)});
        if(p.position.x < -clamp) p.position.x = -clamp;
        if(p.position.y < -clamp) p.position.y = -clamp;
        if(p.position.z < -clamp) p.position.z = -clamp;
        if(p.position.x > clamp) p.position.x = clamp;
        if(p.position.y > clamp) p.position.y = clamp;
        if(p.position.z > clamp) p.position.z = clamp;
    }

    void *data;
    vmaMapMemory(get_context()->get_allocator(), allocation, &data);
    memcpy(data, points.data(), (size_t) sizeof(HiveEngine::Point) * points.size());
    vmaUnmapMemory(get_context()->get_allocator(), allocation);
}

void HiveEngineRenderer::LineDrawing::draw(VkCommandBuffer cmd_buffer) {
    VkBuffer buffer[] = {pointBuffer};
    VkDeviceSize offsets[] = {0};
    vkCmdBindVertexBuffers(cmd_buffer, 0, 1, buffer, offsets);

    vkCmdBindPipeline(cmd_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);
    vkCmdDraw(cmd_buffer, points.size(), 1, 0, 0);
}

void HiveEngineRenderer::LineDrawing::cleanup() {
    if (is_inited()) {
        vkDestroyPipeline(get_context()->get_device(), graphicsPipeline, nullptr);
        vkDestroyPipelineLayout(get_context()->get_device(), pipelineLayout, nullptr);
        Drawing::cleanup();
    }
}
