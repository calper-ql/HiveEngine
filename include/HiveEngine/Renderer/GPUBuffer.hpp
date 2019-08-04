#ifndef HIVEENGINE_RENDERER_GPUBUFFER_HPP
#define HIVEENGINE_RENDERER_GPUBUFFER_HPP

#include <HiveEngine/Buffer.hpp>
#include <HiveEngine/Renderer/vk_mem_alloc.h>

namespace HiveEngine::Renderer {
    template<class T>
    class GPUBuffer {

    public:
        Buffer<T>* cpu_buffer;
        VmaAllocation allocation = nullptr;
        VkBuffer buffer = nullptr;
        VmaAllocator allocator;
        VkBufferUsageFlagBits usage;
        size_t last_size = 0;
        bool recreated = false;

        GPUBuffer(VmaAllocator allocator=nullptr, VkBufferUsageFlagBits usage=VK_BUFFER_USAGE_TRANSFER_DST_BIT){
            this->cpu_buffer = nullptr;
            this->allocator = allocator;
            this->usage = usage;
        }

        ~GPUBuffer(){
    
        }

        void update(){
            if(!allocator) return;
            if(cpu_buffer && (cpu_buffer->is_changed() || buffer == nullptr)){
                if(last_size == cpu_buffer->size()){
                    if (cpu_buffer->size() > 0) {
                        void *data;
                        vmaMapMemory(allocator, allocation, &data);
                        memcpy(data, cpu_buffer->get_ptr(), (size_t) sizeof(T) * cpu_buffer->size());
                        vmaUnmapMemory(allocator, allocation);
                    }
                } else {
                    cleanup();
                    recreated = true;
                    VkBufferCreateInfo bufferInfo = {VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO};
                    VmaAllocationCreateInfo allocInfo = {};
                    allocInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;
                    bufferInfo.usage = usage;
                    bufferInfo.size = sizeof(T) * cpu_buffer->size();
                    auto res = vmaCreateBuffer(allocator, &bufferInfo, &allocInfo, &buffer, &allocation,
                                    nullptr);

					if (res != VK_SUCCESS) {
						spdlog::error("vmaCreateBuffer failed with " + std::to_string(res) + " (GPUBuffer.hpp:48) ");
						process_error();
					}

					if (buffer == nullptr) {
						spdlog::error("vmaCreateBuffer failed buffer = VK_NULL_HANDLE");
						process_error();
					}

                    if (cpu_buffer->size() > 0) {
                        void *data;
                        vmaMapMemory(allocator, allocation, &data);
                        memcpy(data, cpu_buffer->get_ptr(), (size_t) sizeof(T) * cpu_buffer->size());
                        vmaUnmapMemory(allocator, allocation);
                    }

                    last_size = cpu_buffer->size();

                }


            }
        }

        void cleanup(){
            if(!allocator) return;
            vmaDestroyBuffer(allocator, buffer, allocation);
            allocation = nullptr;
            buffer = nullptr;
			last_size = 0;
        }

    };
}

#endif