//
// Created by calper on 5/7/19.
//

#include <HiveEngine/Renderer/AABBDrawing.h>

namespace HiveEngine::Renderer
{

AABBDrawing::AABBDrawing(Directive *directive, Camera *camera) : Drawing(directive)
{
    bindingDescriptions[0].binding = 0;
    bindingDescriptions[0].stride = sizeof(glm::vec3);
    bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    bindingDescriptions[1].binding = 1;
    bindingDescriptions[1].stride = sizeof(DAABB);
    bindingDescriptions[1].inputRate = VK_VERTEX_INPUT_RATE_INSTANCE;

    bindingDescriptions[2].binding = 2;
    bindingDescriptions[2].stride = sizeof(int);
    bindingDescriptions[2].inputRate = VK_VERTEX_INPUT_RATE_INSTANCE;

    attributeDescriptions[0].binding = 0;
    attributeDescriptions[0].location = 0;
    attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[0].offset = 0;

    attributeDescriptions[1].binding = 1;
    attributeDescriptions[1].location = 1;
    attributeDescriptions[1].format = VK_FORMAT_R64G64B64_SFLOAT;
    attributeDescriptions[1].offset = offsetof(HiveEngine::DAABB, min);

    attributeDescriptions[2].binding = 1;
    attributeDescriptions[2].location = 3;
    attributeDescriptions[2].format = VK_FORMAT_R64G64B64_SFLOAT;
    attributeDescriptions[2].offset = offsetof(HiveEngine::DAABB, max);

    attributeDescriptions[3].binding = 2;
    attributeDescriptions[3].location = 5;
    attributeDescriptions[3].format = VK_FORMAT_R32_SINT;
    attributeDescriptions[3].offset = 0;

    for (size_t i = 0; i < 24; i++)
        proto_box_buffer.add({});
    glm::vec3* lines = proto_box_buffer.get_ptr();

    lines[0] = {0, 0, 0};
    lines[1] = {1, 0, 0};
    lines[2] = {0, 1, 0};
    lines[3] = {1, 1, 0};
    lines[4] = {0, 0, 1};
    lines[5] = {1, 0, 1};
    lines[6] = {0, 1, 1};
    lines[7] = {1, 1, 1};

    lines[8] = {0, 0, 0};
    lines[9] = {0, 1, 0};
    lines[10] = {0, 0, 1};
    lines[11] = {0, 1, 1};
    lines[12] = {1, 0, 0};
    lines[13] = {1, 1, 0};
    lines[14] = {1, 0, 1};
    lines[15] = {1, 1, 1};

    lines[16] = {0, 0, 0};
    lines[17] = {0, 0, 1};
    lines[18] = {1, 0, 0};
    lines[19] = {1, 0, 1};
    lines[20] = {0, 1, 0};
    lines[21] = {0, 1, 1};
    lines[22] = {1, 1, 0};
    lines[23] = {1, 1, 1};

    CameraPackage package;
    camera_buffer.add(package);

    significance_gpu_buffer.cpu_buffer = &significance_buffer;
    camera_gpu_buffer.cpu_buffer = &camera_buffer;
    daabb_gpu_buffer.cpu_buffer = &daabb_buffer;
    proto_box_gpu_buffer.cpu_buffer = &proto_box_buffer;

    significance_gpu_buffer.usage = (VkBufferUsageFlagBits)(VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT);
    camera_gpu_buffer.usage = (VkBufferUsageFlagBits)(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT);
    daabb_gpu_buffer.usage = (VkBufferUsageFlagBits)(VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT);
    proto_box_gpu_buffer.usage = (VkBufferUsageFlagBits)(VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT);

    this->camera = camera;
}

void AABBDrawing::init(VkRenderPass render_pass)
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
    vertShaderStageInfo.module = get_context()->get_shaders()["aabb_shader.vert"];
    vertShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo fragShaderStageInfo = {};
    fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragShaderStageInfo.module = get_context()->get_shaders()["aabb_shader.frag"];
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

    VkPipelineDepthStencilStateCreateInfo depthStencil = {};
    depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthStencil.depthTestEnable = VK_TRUE;
    depthStencil.depthWriteEnable = VK_TRUE;
    depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
    depthStencil.depthBoundsTestEnable = VK_FALSE;
    depthStencil.minDepthBounds = 0.0f; // Optional
    depthStencil.maxDepthBounds = 1.0f; // Optional
    depthStencil.stencilTestEnable = VK_FALSE;
    depthStencil.front = {}; // Optional
    depthStencil.back = {}; // Optional

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
    pipelineInfo.pDepthStencilState = &depthStencil;
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

void AABBDrawing::update()
{
	VmaAllocator allocator = get_context()->get_allocator();

	significance_gpu_buffer.allocator = allocator;
	camera_gpu_buffer.allocator = allocator;
	daabb_gpu_buffer.allocator = allocator;
	proto_box_gpu_buffer.allocator = allocator;

    significance_gpu_buffer.update();

    daabb_gpu_buffer.update();

    proto_box_gpu_buffer.update();
    proto_box_buffer.mark_unchanged();

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

void AABBDrawing::draw(VkCommandBuffer cmd_buffer) {
	if (daabb_buffer.size() > 0) {
        VkDeviceSize offsets[1] = {0};
        update();

		vkCmdBindPipeline(cmd_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);

        vkCmdBindVertexBuffers(cmd_buffer, 0, 1, &proto_box_gpu_buffer.buffer, offsets);
        vkCmdBindVertexBuffers(cmd_buffer, 1, 1, &daabb_gpu_buffer.buffer, offsets);
        vkCmdBindVertexBuffers(cmd_buffer, 2, 1, &significance_gpu_buffer.buffer, offsets);

		vkCmdBindDescriptorSets(cmd_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &descriptorSet, 0,
			nullptr);

        vkCmdDraw(cmd_buffer, 24, daabb_buffer.size(), 0, 0);
	}
}

void AABBDrawing::cleanup()
{
    camera_gpu_buffer.cleanup();

    daabb_gpu_buffer.cleanup();

    significance_gpu_buffer.cleanup();

    proto_box_gpu_buffer.cleanup();
    proto_box_buffer.mark_changed();


    vkDestroyDescriptorSetLayout(get_context()->get_device(), descriptorSetLayout, nullptr);
    vkDestroyDescriptorPool(get_context()->get_device(), descriptorPool, nullptr);

    vkDestroyPipeline(get_context()->get_device(), graphicsPipeline, nullptr);
    vkDestroyPipelineLayout(get_context()->get_device(), pipelineLayout, nullptr);

    Drawing::cleanup();
}

} // namespace HiveEngine::Renderer
