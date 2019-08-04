//
// Created by calper on 5/3/19.
//

#include <HiveEngine/Renderer/StandardDirective.h>
#include <HiveEngine/Renderer/Drawing.h>

namespace HiveEngine::Renderer {

    StandardDirective::StandardDirective(Context *context) : Directive(context) {

    }

    void StandardDirective::init_frame_buffers() {
        Directive::init_frame_buffers();

        VkAttachmentDescription colorAttachment = {};
        colorAttachment.format = get_context()->get_image_format();
        colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
        colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        VkAttachmentReference colorAttachmentRef = {};
        colorAttachmentRef.attachment = 0;
        colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkSubpassDescription subpass = {};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &colorAttachmentRef;

        VkRenderPassCreateInfo renderPassInfo = {};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassInfo.attachmentCount = 1;
        renderPassInfo.pAttachments = &colorAttachment;
        renderPassInfo.subpassCount = 1;
        renderPassInfo.pSubpasses = &subpass;

        if (vkCreateRenderPass(get_context()->get_device(), &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS) {
            throw std::runtime_error("failed to create render pass!");
        }

        swapChainFramebuffers.resize(get_context()->get_swap_chain_image_views().size());

        for (size_t i = 0; i < get_context()->get_swap_chain_image_views().size(); i++) {
            VkImageView attachments[] = {
                    get_context()->get_swap_chain_image_views()[i]
            };

            VkFramebufferCreateInfo framebufferInfo = {};
            framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            framebufferInfo.renderPass = renderPass;
            framebufferInfo.attachmentCount = 1;
            framebufferInfo.pAttachments = attachments;
            framebufferInfo.width = get_context()->get_swap_chain_extent().width;
            framebufferInfo.height = get_context()->get_swap_chain_extent().height;
            framebufferInfo.layers = 1;

            if (vkCreateFramebuffer(get_context()->get_device(), &framebufferInfo, nullptr,
                                    &swapChainFramebuffers[i]) != VK_SUCCESS) {
                throw std::runtime_error("failed to create framebuffer!");
            }
        }


    }

    StandardDirective::~StandardDirective() {

    }

    void StandardDirective::init_command_buffer() {
        commandBuffers.resize(swapChainFramebuffers.size());

        VkCommandBufferAllocateInfo allocInfo = {};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool = get_context()->get_command_pool();
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount = (uint32_t) commandBuffers.size();

        if (vkAllocateCommandBuffers(get_context()->get_device(), &allocInfo, commandBuffers.data()) != VK_SUCCESS) {
            throw std::runtime_error("failed to allocate command buffers!");
        }

        for (int i = 0; i < drawings.size(); ++i) {
            if(drawings.get_state(i)) {
                auto drawing = drawings.get(i);
                if(!drawing->is_parent_managed()) drawing->init(renderPass);
            }
        }
    }

    VkCommandBuffer StandardDirective::get_command_buffer(uint8_t idx) {
        VkCommandBufferBeginInfo beginInfo = {};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;

        if (vkBeginCommandBuffer(commandBuffers[idx], &beginInfo) != VK_SUCCESS) {
            throw std::runtime_error("failed to begin recording command buffer!");
        }

        VkRenderPassBeginInfo renderPassInfo = {};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = renderPass;
        renderPassInfo.framebuffer = swapChainFramebuffers[idx];
        renderPassInfo.renderArea.offset = {0, 0};
        renderPassInfo.renderArea.extent = get_context()->get_swap_chain_extent();

        VkClearValue clearColor = {0.0f, 0.0f, 0.0f, 1.0f};
        renderPassInfo.clearValueCount = 1;
        renderPassInfo.pClearValues = &clearColor;

        vkCmdBeginRenderPass(commandBuffers[idx], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        for (int i = 0; i < drawings.size(); ++i) {
            if(drawings.get_state(i)) {
                auto drawing = drawings.get(i);
                if (drawing->is_enabled() && drawing->is_inited()) {
					if (!drawing->is_parent_managed()) drawing->draw(commandBuffers[idx]);
                }
            }
        }

        vkCmdEndRenderPass(commandBuffers[idx]);

        if (vkEndCommandBuffer(commandBuffers[idx]) != VK_SUCCESS) {
            throw std::runtime_error("failed to record command buffer!");
        }


        return commandBuffers[idx];
    }

    void StandardDirective::cleanup() {
		for (int i = 0; i < drawings.size(); ++i) {
			if (drawings.get_state(i)) {
				auto drawing = drawings.get(i);
				if (!drawing->is_parent_managed()) drawing->cleanup();
			}
		}

        for (auto framebuffer : swapChainFramebuffers) {
            vkDestroyFramebuffer(get_context()->get_device(), framebuffer, nullptr);
        }

        vkFreeCommandBuffers(get_context()->get_device(), get_context()->get_command_pool(),
                             static_cast<uint32_t>(commandBuffers.size()), commandBuffers.data());

        vkDestroyRenderPass(get_context()->get_device(), renderPass, nullptr);
    }

}