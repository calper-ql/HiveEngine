#ifndef HIVEENGINE_RENDERER_GPUBUFFER_HPP
#define HIVEENGINE_RENDERER_GPUBUFFER_HPP

#include <HiveEngine/Buffer.hpp>
#include <HiveEngine/Renderer/vk_mem_alloc.h>

namespace HiveEngine::Renderer {
    template<class T>
    class GPUBuffer {

    public:
        Buffer<T>* cpu_buffer = nullptr;    
        VmaAllocation allocation = nullptr;
        VkBuffer buffer = nullptr;
        VmaAllocator allocator;
        VkBufferUsageFlagBits usage;


        GPUBuffer(VmaAllocator allocator=nullptr, VkBufferUsageFlagBits usage=VK_BUFFER_USAGE_TRANSFER_DST_BIT){
            this->cpu_buffer = nullptr;
            this->allocator = allocator;
            this->usage = usage;
        }

        ~GPUBuffer(){
            cleanup();
        }

        void update(){
            if(!allocator) return;
            if(cpu_buffer && cpu_buffer->is_changed()){
                cleanup();
                VkBufferCreateInfo bufferInfo = {VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO};
                VmaAllocationCreateInfo allocInfo = {};
                allocInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;
                bufferInfo.usage = usage; // VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
                bufferInfo.size = sizeof(T) * cpu_buffer->size();
                vmaCreateBuffer(allocator, &bufferInfo, &allocInfo, &buffer, &allocation,
                        nullptr);

                if (cpu_buffer->size() > 0) {
                    void *data;
                    vmaMapMemory(allocator, allocation, &data);
                    memcpy(data, cpu_buffer->get_ptr(), (size_t) sizeof(T) * cpu_buffer->size());
                    vmaUnmapMemory(allocator, allocation);
                }
            }
        }

        void cleanup(){
            if(!allocator) return;
            vmaDestroyBuffer(allocator, buffer, allocation);
            allocation = nullptr;
            buffer = nullptr;
        }

    };
}

#endif