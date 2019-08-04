#include <utility>

//
// Created by calper on 5/11/19.
//

#include <HiveEngine/Renderer/GlyphDrawing.h>

namespace HiveEngine::Renderer {

    GlyphDrawing::GlyphDrawing(Directive *directive, HiveEngine::Texture texture) : Drawing(directive) {
        this->texture = texture;
    }

    void GlyphDrawing::init(VkRenderPass render_pass) {
        Drawing::init(render_pass);
		if (texture.width == 0 || texture.height == 0 || texture.channel == 0) return;

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
        imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        imageInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
        imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
        imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        if(texture.channel == 3){
            imageInfo.format = VK_FORMAT_R8G8B8_UNORM;
        } else if(texture.channel == 1){
            imageInfo.format = VK_FORMAT_R8_UNORM;
        }

        VmaAllocationCreateInfo textureCreateInfo = {};
        textureCreateInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;

        auto res = vmaCreateImage(get_context()->get_allocator(), &imageInfo, &textureCreateInfo,
                                  &textureImage, &textureAllocation, nullptr);

        if(res != VK_SUCCESS){
			std::cout << " == " << std::endl;
			std::cout << texture.width << std::endl;
			std::cout << texture.height << std::endl;
			std::cout << texture.channel << std::endl;
            throw std::runtime_error("Glyph could not create img...");
        }

        std::array<VkVertexInputBindingDescription, 1> bindingDescriptions = {};
        std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions = {};

        bindingDescriptions[0].binding = 0;
        bindingDescriptions[0].stride = sizeof(glm::vec3) + sizeof(glm::vec2) + sizeof(glm::vec4);
        bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[0].offset = offsetof(ImageOrientation, f0);

        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = VK_FORMAT_R32G32_SFLOAT;
        attributeDescriptions[1].offset = offsetof(ImageOrientation, f0uv);

        std::array<VkDescriptorPoolSize, 1> poolSizes = {};
        poolSizes[0].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        poolSizes[0].descriptorCount = 1;

        VkDescriptorPoolCreateInfo poolInfo = {};
        poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolInfo.poolSizeCount = poolSizes.size();
        poolInfo.pPoolSizes = poolSizes.data();

        poolInfo.maxSets = 1;

        if (vkCreateDescriptorPool(get_context()->get_device(), &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS) {
            throw std::runtime_error("failed to create descriptor pool!");
        }

        std::array<VkDescriptorSetLayoutBinding, 1> bindings = {};

        bindings[0].binding = 1;
        bindings[0].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        bindings[0].descriptorCount = 1;
        bindings[0].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
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
        vertShaderStageInfo.module = get_context()->get_shaders()["glyph_shader.vert"];
        vertShaderStageInfo.pName = "main";

        VkPipelineShaderStageCreateInfo fragShaderStageInfo = {};
        fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        fragShaderStageInfo.module = get_context()->get_shaders()["glyph_shader.frag"];
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

    void GlyphDrawing::update() {
		if (!image_pushed) {
			image_pushed = true;

			get_context()->transition_image_layout(textureImage, VK_FORMAT_R8G8B8A8_UNORM,
				VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
			get_context()->copy_texture_to_image(texture, textureImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
			get_context()->transition_image_layout(textureImage, VK_FORMAT_R8G8B8A8_UNORM,
				VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
				VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

			VkImageViewCreateInfo viewInfo = {};
			viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			viewInfo.image = textureImage;
			viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			viewInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
			viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			viewInfo.subresourceRange.baseMipLevel = 0;
			viewInfo.subresourceRange.levelCount = 1;
			viewInfo.subresourceRange.baseArrayLayer = 0;
			viewInfo.subresourceRange.layerCount = 1;

			if (texture.channel == 3) {
				viewInfo.format = VK_FORMAT_R8G8B8_UNORM;
			}
			else if (texture.channel == 1) {
				viewInfo.format = VK_FORMAT_R8_UNORM;
			}

			if (vkCreateImageView(get_context()->get_device(), &viewInfo, nullptr, &imageView) != VK_SUCCESS) {
				throw std::runtime_error("failed to create texture image view!");
			}

			VkSamplerCreateInfo samplerInfo = {};
			samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
			samplerInfo.magFilter = VK_FILTER_LINEAR;
			samplerInfo.minFilter = VK_FILTER_LINEAR;
			samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
			samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
			samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
			samplerInfo.anisotropyEnable = VK_FALSE;
			samplerInfo.maxAnisotropy = 1;
			samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
			samplerInfo.unnormalizedCoordinates = VK_FALSE;
			samplerInfo.compareEnable = VK_FALSE;
			samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
			samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;

			if (vkCreateSampler(get_context()->get_device(), &samplerInfo, nullptr, &textureSampler) !=
				VK_SUCCESS) {
				throw std::runtime_error("failed to create texture sampler!");
			}

			VkDescriptorImageInfo imageInfo = {};
			imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			imageInfo.imageView = imageView;
			imageInfo.sampler = textureSampler;

			VkWriteDescriptorSet imageWrite = {};
			imageWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			imageWrite.dstSet = descriptorSet;
			imageWrite.dstBinding = 1;
			imageWrite.dstArrayElement = 0;
			imageWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			imageWrite.descriptorCount = 1;
			imageWrite.pImageInfo = &imageInfo;

			std::array<VkWriteDescriptorSet, 1> write_descriptors = { imageWrite };

			vkUpdateDescriptorSets(get_context()->get_device(), write_descriptors.size(), write_descriptors.data(), 0, nullptr);
		}

        if (imos.is_changed() || orientation_buffer == nullptr) {
            vmaDestroyBuffer(get_context()->get_allocator(), orientation_buffer, orientation_allocation);
            orientation_buffer = nullptr;
            orientation_allocation = nullptr;

            VkBufferCreateInfo bufferInfo = {VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO};
            bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
            bufferInfo.size = sizeof(ImageOrientation) * imos.size();

            VmaAllocationCreateInfo allocInfo = {};
            allocInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;
            vmaCreateBuffer(get_context()->get_allocator(), &bufferInfo, &allocInfo, &orientation_buffer,
                            &orientation_allocation, nullptr);

            if (imos.size() > 0) {
                void *data;
                vmaMapMemory(get_context()->get_allocator(), orientation_allocation, &data);
                memcpy(data, imos.get_ptr(), bufferInfo.size);
                vmaUnmapMemory(get_context()->get_allocator(), orientation_allocation);
            }

           
        }
        imos.mark_unchanged();

    }

    void GlyphDrawing::draw(VkCommandBuffer cmd_buffer) {
        Drawing::draw(cmd_buffer);
		if (texture.width == 0 || texture.height == 0 || texture.channel == 0) return;

        if (imos.size() > 0) {
            
            this->update();

            vkCmdBindPipeline(cmd_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);

            VkDeviceSize offsets[] = {0};
            vkCmdBindVertexBuffers(cmd_buffer, 0, 1, &orientation_buffer, offsets);

            vkCmdBindDescriptorSets(cmd_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &descriptorSet,
                                    0, nullptr);

            vkCmdDraw(cmd_buffer, imos.size() * 3, 1, 0, 0);
        }
    }

    void GlyphDrawing::cleanup() {
        Drawing::cleanup();
		if (texture.width == 0 || texture.height == 0 || texture.channel == 0) return;

		vmaDestroyImage(get_context()->get_allocator(), textureImage, textureAllocation);

        if (image_pushed) {
            vkDestroyImageView(get_context()->get_device(), imageView, nullptr);
            vkDestroySampler(get_context()->get_device(), textureSampler, nullptr);
            image_pushed = false;
        }

        vmaDestroyBuffer(get_context()->get_allocator(), orientation_buffer, orientation_allocation);
        orientation_buffer = nullptr;
        orientation_allocation = nullptr;

        vkDestroyDescriptorSetLayout(get_context()->get_device(), descriptorSetLayout, nullptr);
        vkDestroyDescriptorPool(get_context()->get_device(), descriptorPool, nullptr);

        vkDestroyPipeline(get_context()->get_device(), graphicsPipeline, nullptr);
        vkDestroyPipelineLayout(get_context()->get_device(), pipelineLayout, nullptr);
    }

    ImageDescription
    GlyphDrawing::add_image_center(glm::vec3 position, float width, float height, glm::vec4 color) {
        ImageDescription id;

        float left = position.x - width / 2.0f;
        float right = position.x + width / 2.0f;
        float up = position.y + height / 2.0f;
        float down = position.y - height / 2.0f;

        ImageOrientation io1 = {};
        ImageOrientation io2 = {};

        io1.f0 = {left, up, position.z};
        io1.f0uv = {0.0f, 0.0f};
        io1.f1 = {right, up, position.z};
        io1.f1uv = {1.0f, 0.0f};
        io1.f2 = {right, down, position.z};
        io1.f2uv = {1.0f, 1.0f};

        io2.f0 = {right, down, position.z};
        io2.f0uv = {1.0f, 1.0f};
        io2.f1 = {left, down, position.z};
        io2.f1uv = {0.0f, 1.0f};
        io2.f2 = {left, up, position.z};
        io2.f2uv = {0.0f, 0.0f};

        id.orientation1 = imos.add(io1);
        id.orientation2 = imos.add(io2);
        id.glyph_drawing = this;

        return id;
    }

    ImageDescription
    GlyphDrawing::add_image_lower_left(glm::vec3 position, float width, float height, glm::vec4 color) {
        ImageDescription id;

        float left = position.x;
        float right = position.x + width;
        float up = position.y + height;
        float down = position.y;

        ImageOrientation io1 = {};
        ImageOrientation io2 = {};

        io1.f0 = {left, up, position.z};
        io1.f0uv = {0.0f, 1.0f};
        io1.c0 = color;
        io1.f1 = {right, up, position.z};
        io1.f1uv = {1.0f, 1.0f};
        io1.c1 = color;
        io1.f2 = {right, down, position.z};
        io1.f2uv = {1.0f, 0.0f};
        io1.c2 = color;

        io2.f0 = {right, down, position.z};
        io2.f0uv = {1.0f, 0.0f};
        io2.c0 = color;
        io2.f1 = {left, down, position.z};
        io2.f1uv = {0.0f, 0.0f};
        io2.c1 = color;
        io2.f2 = {left, up, position.z};
        io2.f2uv = {0.0f, 1.0f};
        io2.c2 = color;

        id.orientation1 = imos.add(io1);
        id.orientation2 = imos.add(io2);
        id.glyph_drawing = this;

        return id;
    }


    void GlyphDrawing::remove_image(ImageDescription id) {
        ImageOrientation io1 = {};
        ImageOrientation io2 = {};

        imos.set(id.orientation1, io1);
        imos.set(id.orientation2, io2);

        imos.remove(id.orientation1);
        imos.remove(id.orientation2);
    }

    GlyphDrawing::~GlyphDrawing() {
		
    }



}
