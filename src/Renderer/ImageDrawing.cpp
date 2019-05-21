#include <utility>

//
// Created by calper on 5/11/19.
//

#include <HiveEngine/Renderer/ImageDrawing.h>

namespace HiveEngineRenderer {

    ImageOrientation create_aligned_image_orientation(glm::vec3 position, float width, float height) {
        ImageOrientation io = {};

        float left = position.x - width/2.0f;
        float right = position.x + width/2.0f;
        float up = position.y + height/2.0f;
        float down = position.y - height/2.0f;

        io.f0 = {left, up, position.z};
        io.f0uv = {0.0f, 0.0f};

        io.f1 = {right, up, position.z};
        io.f1uv = {1.0f, 0.0f};

        io.f2 = {right, down, position.z};
        io.f2uv = {1.0f, 1.0f};

        io.f3 = {right, down, position.z};
        io.f3uv = {1.0f, 1.0f};

        io.f4 = {left, down, position.z};
        io.f4uv = {0.0f, 1.0f};

        io.f5 = {left, up, position.z};
        io.f5uv = {0.0f, 0.0f};

        return io;
    }

    ImageDrawing::ImageDrawing(Directive *directive, HiveEngine::Texture texture) : Drawing(directive) {
        this->texture = texture;


    }

    void ImageDrawing::init(VkRenderPass render_pass) {
        Drawing::init(render_pass);

        VkImageCreateInfo imageInfo = {};
        imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageInfo.imageType = VK_IMAGE_TYPE_2D;
        imageInfo.extent.width = texture.width;
        imageInfo.extent.height = texture.height;
        imageInfo.extent.depth = 1;
        imageInfo.mipLevels = 1;
        imageInfo.arrayLayers = 1;
        imageInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
        imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
        imageInfo.initialLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        imageInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
        imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
        imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        VmaAllocationCreateInfo textureCreateInfo = {};
        textureCreateInfo.usage = VMA_MEMORY_USAGE_UNKNOWN;

        auto res = vmaCreateImage(get_context()->get_allocator(), &imageInfo, &textureCreateInfo,
                &textureImage, &textureAllocation, nullptr);


        std::array<VkVertexInputBindingDescription, 1> bindingDescriptions = {};
        std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions = {};

        bindingDescriptions[0].binding = 0;
        bindingDescriptions[0].stride = sizeof(glm::vec3) + sizeof(glm::vec2);
        bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[0].offset = offsetof(ImageOrientation, f0);

        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = VK_FORMAT_R32G32_SFLOAT;
        attributeDescriptions[1].offset = offsetof(ImageOrientation, f0uv);

        VkDescriptorPoolSize poolSize = {};
        poolSize.type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        poolSize.descriptorCount = 1;

        VkDescriptorPoolCreateInfo poolInfo = {};
        poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolInfo.poolSizeCount = 1;
        poolInfo.pPoolSizes = &poolSize;

        poolInfo.maxSets = 1;

        if (vkCreateDescriptorPool(get_context()->get_device(), &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS) {
            throw std::runtime_error("failed to create descriptor pool!");
        }

        std::array<VkDescriptorSetLayoutBinding, 1> bindings = {};

        bindings[0].binding = 0;
        bindings[0].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        bindings[0].descriptorCount = 1;
        bindings[0].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
        bindings[0].pImmutableSamplers = nullptr;

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
        vertShaderStageInfo.module = get_context()->get_shaders()["image_shader.vert"];
        vertShaderStageInfo.pName = "main";

        VkPipelineShaderStageCreateInfo fragShaderStageInfo = {};
        fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        fragShaderStageInfo.module = get_context()->get_shaders()["image_shader.frag"];
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
        rasterizer.lineWidth = 1;
        rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
        rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
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

    void ImageDrawing::update() {
        if (imos.is_changed() || orientation_buffer == nullptr) {
            vmaDestroyBuffer(get_context()->get_allocator(), orientation_buffer, orientation_allocation);
            orientation_buffer = nullptr;
            orientation_allocation = nullptr;

            auto orientations = imos.get_data();

            VkBufferCreateInfo bufferInfo = {VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO};
            bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
            bufferInfo.size = sizeof(ImageOrientation) * orientations.size();

            VmaAllocationCreateInfo allocInfo = {};
            allocInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;
            vmaCreateBuffer(get_context()->get_allocator(), &bufferInfo, &allocInfo, &orientation_buffer, &orientation_allocation, nullptr);

            if(!orientations.empty()){
                void *data;
                vmaMapMemory(get_context()->get_allocator(), orientation_allocation, &data);
                memcpy(data, orientations.data(), sizeof(ImageOrientation) * orientations.size());
                vmaUnmapMemory(get_context()->get_allocator(), orientation_allocation);
            }
        }
        imos.mark_unchanged();

        if (imtds.is_changed() || description_buffer == nullptr) {
            vmaDestroyBuffer(get_context()->get_allocator(), description_buffer, description_allocation);
            description_buffer = nullptr;
            description_allocation = nullptr;

            auto decriptors = imtds.get_data();

            VkBufferCreateInfo bufferInfo = {VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO};
            bufferInfo.usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
            bufferInfo.size = sizeof(ImageTriangleDescription) * decriptors.size();

            VmaAllocationCreateInfo allocInfo = {};
            allocInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;
            vmaCreateBuffer(get_context()->get_allocator(), &bufferInfo, &allocInfo, &description_buffer, &description_allocation, nullptr);

            if(!decriptors.empty()){
                void *data;
                vmaMapMemory(get_context()->get_allocator(), description_allocation, &data);
                memcpy(data, decriptors.data(), sizeof(ImageTriangleDescription) * decriptors.size());
                vmaUnmapMemory(get_context()->get_allocator(), description_allocation);
            }

            VkDescriptorBufferInfo stateStorageBufferInfo = {};
            stateStorageBufferInfo.buffer = description_buffer;
            stateStorageBufferInfo.offset = 0;
            stateStorageBufferInfo.range = VK_WHOLE_SIZE;

            VkWriteDescriptorSet stateStorageWrite = {};
            stateStorageWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            stateStorageWrite.dstSet = descriptorSet;
            stateStorageWrite.dstBinding = 0;
            stateStorageWrite.dstArrayElement = 0;
            stateStorageWrite.descriptorCount = 1;
            stateStorageWrite.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
            stateStorageWrite.pBufferInfo = &stateStorageBufferInfo;
            stateStorageWrite.pImageInfo = nullptr;
            stateStorageWrite.pTexelBufferView = nullptr;

            std::array<VkWriteDescriptorSet, 1> write_descriptors = {stateStorageWrite};

            vkUpdateDescriptorSets(get_context()->get_device(), write_descriptors.size(), write_descriptors.data(), 0, nullptr);
        }
        imtds.mark_unchanged();

    }

    void ImageDrawing::draw(VkCommandBuffer cmd_buffer) {
        Drawing::draw(cmd_buffer);
        if(imos.size() > 0){
            this->update();

            if(!image_pushed){
                image_pushed = true;
                get_context()->copy_texture_to_image(texture, textureImage);
            }

            vkCmdBindPipeline(cmd_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);

            VkDeviceSize offsets[] = {0};
            vkCmdBindVertexBuffers(cmd_buffer, 0, 1, &orientation_buffer, offsets);

            vkCmdBindDescriptorSets(cmd_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &descriptorSet, 0, nullptr);

            vkCmdDraw(cmd_buffer, imos.size() * 6, 1, 0, 0);
        }
    }

    void ImageDrawing::cleanup() {
        Drawing::cleanup();

        vmaDestroyImage(get_context()->get_allocator(), textureImage, textureAllocation);

        vmaDestroyBuffer(get_context()->get_allocator(), orientation_buffer, orientation_allocation);
        orientation_buffer = nullptr;
        orientation_allocation = nullptr;

        vmaDestroyBuffer(get_context()->get_allocator(), description_buffer, description_allocation);
        description_buffer = nullptr;
        description_allocation = nullptr;

        vkDestroyDescriptorSetLayout(get_context()->get_device(), descriptorSetLayout, nullptr);
        vkDestroyDescriptorPool(get_context()->get_device(), descriptorPool, nullptr);

        vkDestroyPipeline(get_context()->get_device(), graphicsPipeline, nullptr);
        vkDestroyPipelineLayout(get_context()->get_device(), pipelineLayout, nullptr);
    }

    ImageDescription ImageDrawing::add_image(int texture_index, glm::vec3 position, float width, float height) {
        ImageDescription id;
        ImageTriangleDescription itd;
        itd.texture_index = texture_index;

        id.orientation = imos.add(create_aligned_image_orientation(position, width, height));
        id.itdesc1 = imtds.add(itd);
        id.itdesc2 = imtds.add(itd);

        return id;
    }

    void ImageDrawing::remove_image(ImageDescription id) {
        imos.remove(id.orientation);
        ImageTriangleDescription itd;
        itd.texture_index = -2;
        //itd.fallback_color = {1.0, 1.0, 1.0, 1.0};
        imtds.set(id.itdesc1, itd);
        imtds.set(id.itdesc2, itd);
        imtds.remove(id.itdesc1);
        imtds.remove(id.itdesc2);
    }

    ImageDrawing::~ImageDrawing() {

    }
}
