#ifndef VULKAN_COMMON_INCLUDED
#  define VULKAN_COMMON_INCLUDED

#  include <Hydrogen.hpp>
#  include <Helium.hpp>

#  pragma warning(push)
#  pragma warning(disable:4820)
#  include <volk.h>
#  pragma warning(pop)

jmn::Result Translate(VkResult);
jmn::B8     SelectMemoryTypeIndex(VkPhysicalDeviceMemoryProperties const &pdmp, jmn::U32 memory_type_bits, VkMemoryPropertyFlags flags, jmn::U32 &index, jmn::Result &result);

jmn::B8 Create(VkAllocationCallbacks const *ac, VkDevice dev, VkSemaphore &semaphore, jmn::Result &result);
jmn::B8 Create(VkAllocationCallbacks const *ac, VkDevice dev, VkFenceCreateFlags flags, VkFence &fence, jmn::Result &result);
jmn::B8 Create(VkAllocationCallbacks const *ac, VkDevice dev, VkDeviceSize size, VkBufferUsageFlags2 usage, VkBuffer &buffer, jmn::Result &result);
jmn::B8 Create(VkAllocationCallbacks const *ac, VkDevice dev, VkCommandPoolCreateFlags flags, jmn::U32 qfi, VkCommandPool &command_pool, jmn::Result &result);
jmn::B8 Create(VkAllocationCallbacks const *ac, VkDevice dev, uint32_t set_layout_count, VkDescriptorSetLayout const *set_layout_array, uint32_t push_constant_range_count, VkPushConstantRange const *push_constant_range_array, VkPipelineLayout &pipeline_layout, jmn::Result &result);

jmn::B8 Allocate(VkDevice dev, VkCommandPool command_pool, VkCommandBufferLevel level, jmn::U32 command_buffer_count, VkCommandBuffer *command_buffer_array, jmn::Result &result);
jmn::B8 AllocateDedicated(VkAllocationCallbacks const *ac, VkPhysicalDeviceMemoryProperties const &pdmp, VkDevice dev, VkBuffer buffer, VkMemoryPropertyFlags flags, VkDeviceMemory &memory, jmn::Result &result);

void CmdBeginDebugUtilsLabel(VkCommandBuffer cb, jmn::C8 const *name, jmn::V4F32 color);
void CmdInsertDebugUtilsLabel(VkCommandBuffer cb, jmn::C8 const *name, jmn::V4F32 color);
void QueueBeginDebugUtilsLabel(VkQueue q, jmn::C8 const *name, jmn::V4F32 color);
void QueueInsertDebugUtilsLabel(VkQueue q, jmn::C8 const *name, jmn::V4F32 color);
jmn::B8 SetDebugUtilsObjectName(VkDevice dev, VkObjectType type, jmn::Handle handle, jmn::C8 const *name, jmn::Result &result);
jmn::B8 SetDebugUtilsObjectTag(VkDevice dev, VkObjectType type, jmn::Handle handle, jmn::U64 tag_id, jmn::Size tag_size, jmn::Addr tag_data, jmn::Result &result);

template<typename T> jmn::B8 SetDebugUtilsObjectName(VkDevice dev, T const &object, jmn::C8 const *name, jmn::Result &result) { return SetDebugUtilsObjectName(dev, VK_OBJECT_TYPE_UNKNOWN, (jmn::Handle)object, name, result); }
template<> jmn::B8 SetDebugUtilsObjectName(VkDevice dev, VkInstance            const &object, jmn::C8 const *name, jmn::Result &result) { return SetDebugUtilsObjectName(dev, VK_OBJECT_TYPE_INSTANCE, (jmn::Handle)object, name, result); }
template<> jmn::B8 SetDebugUtilsObjectName(VkDevice dev, VkPhysicalDevice      const &object, jmn::C8 const *name, jmn::Result &result) { return SetDebugUtilsObjectName(dev, VK_OBJECT_TYPE_PHYSICAL_DEVICE, (jmn::Handle)object, name, result); }
template<> jmn::B8 SetDebugUtilsObjectName(VkDevice dev, VkDevice              const &object, jmn::C8 const *name, jmn::Result &result) { return SetDebugUtilsObjectName(dev, VK_OBJECT_TYPE_DEVICE, (jmn::Handle)object, name, result); }
template<> jmn::B8 SetDebugUtilsObjectName(VkDevice dev, VkQueue               const &object, jmn::C8 const *name, jmn::Result &result) { return SetDebugUtilsObjectName(dev, VK_OBJECT_TYPE_QUEUE, (jmn::Handle)object, name, result); }
template<> jmn::B8 SetDebugUtilsObjectName(VkDevice dev, VkSemaphore           const &object, jmn::C8 const *name, jmn::Result &result) { return SetDebugUtilsObjectName(dev, VK_OBJECT_TYPE_SEMAPHORE, (jmn::Handle)object, name, result); }
template<> jmn::B8 SetDebugUtilsObjectName(VkDevice dev, VkCommandBuffer       const &object, jmn::C8 const *name, jmn::Result &result) { return SetDebugUtilsObjectName(dev, VK_OBJECT_TYPE_COMMAND_BUFFER, (jmn::Handle)object, name, result); }
template<> jmn::B8 SetDebugUtilsObjectName(VkDevice dev, VkFence               const &object, jmn::C8 const *name, jmn::Result &result) { return SetDebugUtilsObjectName(dev, VK_OBJECT_TYPE_FENCE, (jmn::Handle)object, name, result); }
template<> jmn::B8 SetDebugUtilsObjectName(VkDevice dev, VkDeviceMemory        const &object, jmn::C8 const *name, jmn::Result &result) { return SetDebugUtilsObjectName(dev, VK_OBJECT_TYPE_DEVICE_MEMORY, (jmn::Handle)object, name, result); }
template<> jmn::B8 SetDebugUtilsObjectName(VkDevice dev, VkBuffer              const &object, jmn::C8 const *name, jmn::Result &result) { return SetDebugUtilsObjectName(dev, VK_OBJECT_TYPE_BUFFER, (jmn::Handle)object, name, result); }
template<> jmn::B8 SetDebugUtilsObjectName(VkDevice dev, VkImage               const &object, jmn::C8 const *name, jmn::Result &result) { return SetDebugUtilsObjectName(dev, VK_OBJECT_TYPE_IMAGE, (jmn::Handle)object, name, result); }
template<> jmn::B8 SetDebugUtilsObjectName(VkDevice dev, VkEvent               const &object, jmn::C8 const *name, jmn::Result &result) { return SetDebugUtilsObjectName(dev, VK_OBJECT_TYPE_EVENT, (jmn::Handle)object, name, result); }
template<> jmn::B8 SetDebugUtilsObjectName(VkDevice dev, VkQueryPool           const &object, jmn::C8 const *name, jmn::Result &result) { return SetDebugUtilsObjectName(dev, VK_OBJECT_TYPE_QUERY_POOL, (jmn::Handle)object, name, result); }
template<> jmn::B8 SetDebugUtilsObjectName(VkDevice dev, VkBufferView          const &object, jmn::C8 const *name, jmn::Result &result) { return SetDebugUtilsObjectName(dev, VK_OBJECT_TYPE_BUFFER_VIEW, (jmn::Handle)object, name, result); }
template<> jmn::B8 SetDebugUtilsObjectName(VkDevice dev, VkImageView           const &object, jmn::C8 const *name, jmn::Result &result) { return SetDebugUtilsObjectName(dev, VK_OBJECT_TYPE_IMAGE_VIEW, (jmn::Handle)object, name, result); }
template<> jmn::B8 SetDebugUtilsObjectName(VkDevice dev, VkShaderModule        const &object, jmn::C8 const *name, jmn::Result &result) { return SetDebugUtilsObjectName(dev, VK_OBJECT_TYPE_SHADER_MODULE, (jmn::Handle)object, name, result); }
template<> jmn::B8 SetDebugUtilsObjectName(VkDevice dev, VkPipelineCache       const &object, jmn::C8 const *name, jmn::Result &result) { return SetDebugUtilsObjectName(dev, VK_OBJECT_TYPE_PIPELINE_CACHE, (jmn::Handle)object, name, result); }
template<> jmn::B8 SetDebugUtilsObjectName(VkDevice dev, VkPipelineLayout      const &object, jmn::C8 const *name, jmn::Result &result) { return SetDebugUtilsObjectName(dev, VK_OBJECT_TYPE_PIPELINE_LAYOUT, (jmn::Handle)object, name, result); }
template<> jmn::B8 SetDebugUtilsObjectName(VkDevice dev, VkRenderPass          const &object, jmn::C8 const *name, jmn::Result &result) { return SetDebugUtilsObjectName(dev, VK_OBJECT_TYPE_RENDER_PASS, (jmn::Handle)object, name, result); }
template<> jmn::B8 SetDebugUtilsObjectName(VkDevice dev, VkPipeline            const &object, jmn::C8 const *name, jmn::Result &result) { return SetDebugUtilsObjectName(dev, VK_OBJECT_TYPE_PIPELINE, (jmn::Handle)object, name, result); }
template<> jmn::B8 SetDebugUtilsObjectName(VkDevice dev, VkDescriptorSetLayout const &object, jmn::C8 const *name, jmn::Result &result) { return SetDebugUtilsObjectName(dev, VK_OBJECT_TYPE_DESCRIPTOR_SET_LAYOUT, (jmn::Handle)object, name, result); }
template<> jmn::B8 SetDebugUtilsObjectName(VkDevice dev, VkSampler             const &object, jmn::C8 const *name, jmn::Result &result) { return SetDebugUtilsObjectName(dev, VK_OBJECT_TYPE_SAMPLER, (jmn::Handle)object, name, result); }
template<> jmn::B8 SetDebugUtilsObjectName(VkDevice dev, VkDescriptorPool      const &object, jmn::C8 const *name, jmn::Result &result) { return SetDebugUtilsObjectName(dev, VK_OBJECT_TYPE_DESCRIPTOR_POOL, (jmn::Handle)object, name, result); }
template<> jmn::B8 SetDebugUtilsObjectName(VkDevice dev, VkDescriptorSet       const &object, jmn::C8 const *name, jmn::Result &result) { return SetDebugUtilsObjectName(dev, VK_OBJECT_TYPE_DESCRIPTOR_SET, (jmn::Handle)object, name, result); }
template<> jmn::B8 SetDebugUtilsObjectName(VkDevice dev, VkFramebuffer         const &object, jmn::C8 const *name, jmn::Result &result) { return SetDebugUtilsObjectName(dev, VK_OBJECT_TYPE_FRAMEBUFFER, (jmn::Handle)object, name, result); }
template<> jmn::B8 SetDebugUtilsObjectName(VkDevice dev, VkCommandPool         const &object, jmn::C8 const *name, jmn::Result &result) { return SetDebugUtilsObjectName(dev, VK_OBJECT_TYPE_COMMAND_POOL, (jmn::Handle)object, name, result); }

template<typename T> jmn::B8 SetDebugUtilsObjectTag(VkDevice dev, T const &object, jmn::U64 tag_id, jmn::Size tag_size, jmn::Addr tag_data, jmn::Result &result) { return SetDebugUtilsObjectTag(dev, VK_OBJECT_TYPE_UNKNOWN, (jmn::Handle)object, tag_id, tag_size, tag_data, result); }
template<> jmn::B8 SetDebugUtilsObjectTag(VkDevice dev, VkInstance            const &object, jmn::U64 tag_id, jmn::Size tag_size, jmn::Addr tag_data, jmn::Result &result) { return SetDebugUtilsObjectTag(dev, VK_OBJECT_TYPE_INSTANCE, (jmn::Handle)object, tag_id, tag_size, tag_data, result); }
template<> jmn::B8 SetDebugUtilsObjectTag(VkDevice dev, VkPhysicalDevice      const &object, jmn::U64 tag_id, jmn::Size tag_size, jmn::Addr tag_data, jmn::Result &result) { return SetDebugUtilsObjectTag(dev, VK_OBJECT_TYPE_PHYSICAL_DEVICE, (jmn::Handle)object, tag_id, tag_size, tag_data, result); }
template<> jmn::B8 SetDebugUtilsObjectTag(VkDevice dev, VkDevice              const &object, jmn::U64 tag_id, jmn::Size tag_size, jmn::Addr tag_data, jmn::Result &result) { return SetDebugUtilsObjectTag(dev, VK_OBJECT_TYPE_DEVICE, (jmn::Handle)object, tag_id, tag_size, tag_data, result); }
template<> jmn::B8 SetDebugUtilsObjectTag(VkDevice dev, VkQueue               const &object, jmn::U64 tag_id, jmn::Size tag_size, jmn::Addr tag_data, jmn::Result &result) { return SetDebugUtilsObjectTag(dev, VK_OBJECT_TYPE_QUEUE, (jmn::Handle)object, tag_id, tag_size, tag_data, result); }
template<> jmn::B8 SetDebugUtilsObjectTag(VkDevice dev, VkSemaphore           const &object, jmn::U64 tag_id, jmn::Size tag_size, jmn::Addr tag_data, jmn::Result &result) { return SetDebugUtilsObjectTag(dev, VK_OBJECT_TYPE_SEMAPHORE, (jmn::Handle)object, tag_id, tag_size, tag_data, result); }
template<> jmn::B8 SetDebugUtilsObjectTag(VkDevice dev, VkCommandBuffer       const &object, jmn::U64 tag_id, jmn::Size tag_size, jmn::Addr tag_data, jmn::Result &result) { return SetDebugUtilsObjectTag(dev, VK_OBJECT_TYPE_COMMAND_BUFFER, (jmn::Handle)object, tag_id, tag_size, tag_data, result); }
template<> jmn::B8 SetDebugUtilsObjectTag(VkDevice dev, VkFence               const &object, jmn::U64 tag_id, jmn::Size tag_size, jmn::Addr tag_data, jmn::Result &result) { return SetDebugUtilsObjectTag(dev, VK_OBJECT_TYPE_FENCE, (jmn::Handle)object, tag_id, tag_size, tag_data, result); }
template<> jmn::B8 SetDebugUtilsObjectTag(VkDevice dev, VkDeviceMemory        const &object, jmn::U64 tag_id, jmn::Size tag_size, jmn::Addr tag_data, jmn::Result &result) { return SetDebugUtilsObjectTag(dev, VK_OBJECT_TYPE_DEVICE_MEMORY, (jmn::Handle)object, tag_id, tag_size, tag_data, result); }
template<> jmn::B8 SetDebugUtilsObjectTag(VkDevice dev, VkBuffer              const &object, jmn::U64 tag_id, jmn::Size tag_size, jmn::Addr tag_data, jmn::Result &result) { return SetDebugUtilsObjectTag(dev, VK_OBJECT_TYPE_BUFFER, (jmn::Handle)object, tag_id, tag_size, tag_data, result); }
template<> jmn::B8 SetDebugUtilsObjectTag(VkDevice dev, VkImage               const &object, jmn::U64 tag_id, jmn::Size tag_size, jmn::Addr tag_data, jmn::Result &result) { return SetDebugUtilsObjectTag(dev, VK_OBJECT_TYPE_IMAGE, (jmn::Handle)object, tag_id, tag_size, tag_data, result); }
template<> jmn::B8 SetDebugUtilsObjectTag(VkDevice dev, VkEvent               const &object, jmn::U64 tag_id, jmn::Size tag_size, jmn::Addr tag_data, jmn::Result &result) { return SetDebugUtilsObjectTag(dev, VK_OBJECT_TYPE_EVENT, (jmn::Handle)object, tag_id, tag_size, tag_data, result); }
template<> jmn::B8 SetDebugUtilsObjectTag(VkDevice dev, VkQueryPool           const &object, jmn::U64 tag_id, jmn::Size tag_size, jmn::Addr tag_data, jmn::Result &result) { return SetDebugUtilsObjectTag(dev, VK_OBJECT_TYPE_QUERY_POOL, (jmn::Handle)object, tag_id, tag_size, tag_data, result); }
template<> jmn::B8 SetDebugUtilsObjectTag(VkDevice dev, VkBufferView          const &object, jmn::U64 tag_id, jmn::Size tag_size, jmn::Addr tag_data, jmn::Result &result) { return SetDebugUtilsObjectTag(dev, VK_OBJECT_TYPE_BUFFER_VIEW, (jmn::Handle)object, tag_id, tag_size, tag_data, result); }
template<> jmn::B8 SetDebugUtilsObjectTag(VkDevice dev, VkImageView           const &object, jmn::U64 tag_id, jmn::Size tag_size, jmn::Addr tag_data, jmn::Result &result) { return SetDebugUtilsObjectTag(dev, VK_OBJECT_TYPE_IMAGE_VIEW, (jmn::Handle)object, tag_id, tag_size, tag_data, result); }
template<> jmn::B8 SetDebugUtilsObjectTag(VkDevice dev, VkShaderModule        const &object, jmn::U64 tag_id, jmn::Size tag_size, jmn::Addr tag_data, jmn::Result &result) { return SetDebugUtilsObjectTag(dev, VK_OBJECT_TYPE_SHADER_MODULE, (jmn::Handle)object, tag_id, tag_size, tag_data, result); }
template<> jmn::B8 SetDebugUtilsObjectTag(VkDevice dev, VkPipelineCache       const &object, jmn::U64 tag_id, jmn::Size tag_size, jmn::Addr tag_data, jmn::Result &result) { return SetDebugUtilsObjectTag(dev, VK_OBJECT_TYPE_PIPELINE_CACHE, (jmn::Handle)object, tag_id, tag_size, tag_data, result); }
template<> jmn::B8 SetDebugUtilsObjectTag(VkDevice dev, VkPipelineLayout      const &object, jmn::U64 tag_id, jmn::Size tag_size, jmn::Addr tag_data, jmn::Result &result) { return SetDebugUtilsObjectTag(dev, VK_OBJECT_TYPE_PIPELINE_LAYOUT, (jmn::Handle)object, tag_id, tag_size, tag_data, result); }
template<> jmn::B8 SetDebugUtilsObjectTag(VkDevice dev, VkRenderPass          const &object, jmn::U64 tag_id, jmn::Size tag_size, jmn::Addr tag_data, jmn::Result &result) { return SetDebugUtilsObjectTag(dev, VK_OBJECT_TYPE_RENDER_PASS, (jmn::Handle)object, tag_id, tag_size, tag_data, result); }
template<> jmn::B8 SetDebugUtilsObjectTag(VkDevice dev, VkPipeline            const &object, jmn::U64 tag_id, jmn::Size tag_size, jmn::Addr tag_data, jmn::Result &result) { return SetDebugUtilsObjectTag(dev, VK_OBJECT_TYPE_PIPELINE, (jmn::Handle)object, tag_id, tag_size, tag_data, result); }
template<> jmn::B8 SetDebugUtilsObjectTag(VkDevice dev, VkDescriptorSetLayout const &object, jmn::U64 tag_id, jmn::Size tag_size, jmn::Addr tag_data, jmn::Result &result) { return SetDebugUtilsObjectTag(dev, VK_OBJECT_TYPE_DESCRIPTOR_SET_LAYOUT, (jmn::Handle)object, tag_id, tag_size, tag_data, result); }
template<> jmn::B8 SetDebugUtilsObjectTag(VkDevice dev, VkSampler             const &object, jmn::U64 tag_id, jmn::Size tag_size, jmn::Addr tag_data, jmn::Result &result) { return SetDebugUtilsObjectTag(dev, VK_OBJECT_TYPE_SAMPLER, (jmn::Handle)object, tag_id, tag_size, tag_data, result); }
template<> jmn::B8 SetDebugUtilsObjectTag(VkDevice dev, VkDescriptorPool      const &object, jmn::U64 tag_id, jmn::Size tag_size, jmn::Addr tag_data, jmn::Result &result) { return SetDebugUtilsObjectTag(dev, VK_OBJECT_TYPE_DESCRIPTOR_POOL, (jmn::Handle)object, tag_id, tag_size, tag_data, result); }
template<> jmn::B8 SetDebugUtilsObjectTag(VkDevice dev, VkDescriptorSet       const &object, jmn::U64 tag_id, jmn::Size tag_size, jmn::Addr tag_data, jmn::Result &result) { return SetDebugUtilsObjectTag(dev, VK_OBJECT_TYPE_DESCRIPTOR_SET, (jmn::Handle)object, tag_id, tag_size, tag_data, result); }
template<> jmn::B8 SetDebugUtilsObjectTag(VkDevice dev, VkFramebuffer         const &object, jmn::U64 tag_id, jmn::Size tag_size, jmn::Addr tag_data, jmn::Result &result) { return SetDebugUtilsObjectTag(dev, VK_OBJECT_TYPE_FRAMEBUFFER, (jmn::Handle)object, tag_id, tag_size, tag_data, result); }
template<> jmn::B8 SetDebugUtilsObjectTag(VkDevice dev, VkCommandPool         const &object, jmn::U64 tag_id, jmn::Size tag_size, jmn::Addr tag_data, jmn::Result &result) { return SetDebugUtilsObjectTag(dev, VK_OBJECT_TYPE_COMMAND_POOL, (jmn::Handle)object, tag_id, tag_size, tag_data, result); }

#define VK_CHECK(expr, resvar, jmplbl) do { VkResult _vkresult_ = expr; JMN_CHECK(_vkresult_ == VK_SUCCESS, resvar, Translate(_vkresult_), jmplbl); } while (0)

#endif // VULKAN_COMMON_INCLUDED

#ifdef __INTELLISENSE__
#  define VULKAN_COMMON_IMPLEMENTATION
#endif // __INTELLISENSE__

#ifdef VULKAN_COMMON_IMPLEMENTATION
#  ifndef VULKAN_COMMON_IMPLEMENTATED
#    define VULKAN_COMMON_IMPLEMENTATED

jmn::Result Translate(VkResult vkresult)
{
    using namespace jmn;

    switch (vkresult)
    {
        case VK_SUCCESS:
            return Result::Success;
        case VK_ERROR_OUT_OF_HOST_MEMORY:
        case VK_ERROR_OUT_OF_DEVICE_MEMORY:
        case VK_ERROR_TOO_MANY_OBJECTS:
        case VK_ERROR_OUT_OF_POOL_MEMORY:
            return Result::ErrorOutOfMemory;
        case VK_ERROR_LAYER_NOT_PRESENT:
        case VK_ERROR_EXTENSION_NOT_PRESENT:
        case VK_ERROR_FEATURE_NOT_PRESENT:
            return Result::ErrorNotFound;
        case VK_ERROR_INCOMPATIBLE_DRIVER:
        case VK_ERROR_FORMAT_NOT_SUPPORTED:
            return Result::ErrorNotSupported;
        default:
            return Result::ErrorGeneric;
    }
}

jmn::B8 SelectMemoryTypeIndex(VkPhysicalDeviceMemoryProperties const &pdmp, jmn::U32 memory_type_bits, VkMemoryPropertyFlags flags, jmn::U32 &index, jmn::Result &result)
{
    using namespace jmn;

    B8 found = false;
    for (U32 i = 0; i < pdmp.memoryTypeCount; ++i)
    {
        if (BitTest((LONG const *)&memory_type_bits, (LONG)i) && ((pdmp.memoryTypes[i].propertyFlags & flags) == flags))
        {
            index = i;
            found = true;
            break;
        }
    }
    JMN_CHECK(found, result, Result::ErrorNotFound, ex0);
    return true;
ex0:return false;
}

jmn::B8 Create(VkAllocationCallbacks const *ac, VkDevice dev, VkSemaphore &semaphore, jmn::Result &result)
{
    VkSemaphoreCreateInfo ci;
    ci.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    ci.pNext = NULL;
    ci.flags = 0;
    VK_CHECK(vkCreateSemaphore(dev, &ci, ac, &semaphore), result, ex0);
    return true;
ex0:return false;
}

jmn::B8 Create(VkAllocationCallbacks const *ac, VkDevice dev, VkFenceCreateFlags flags, VkFence &fence, jmn::Result &result)
{
    VkFenceCreateInfo ci;
    ci.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    ci.pNext = NULL;
    ci.flags = flags;
    VK_CHECK(vkCreateFence(dev, &ci, ac, &fence), result, ex0);
    return true;
ex0:return false;
}

jmn::B8 Create(VkAllocationCallbacks const *ac, VkDevice dev, VkDeviceSize size, VkBufferUsageFlags2 usage, VkBuffer &buffer, jmn::Result &result)
{
    VkBufferUsageFlags2CreateInfo buf2_ci;
    VkBufferCreateInfo            b_ci;

    b_ci   .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    buf2_ci.sType = VK_STRUCTURE_TYPE_BUFFER_USAGE_FLAGS_2_CREATE_INFO;

    b_ci   .pNext = &buf2_ci;
    buf2_ci.pNext = NULL;

    b_ci.flags                 = 0;
    b_ci.size                  = size;
    b_ci.usage                 = 0;
    b_ci.sharingMode           = VK_SHARING_MODE_EXCLUSIVE;
    b_ci.queueFamilyIndexCount = 0;
    b_ci.pQueueFamilyIndices   = NULL;

    buf2_ci.usage = usage;

    VK_CHECK(vkCreateBuffer(dev, &b_ci, ac, &buffer), result, ex0);
    return true;
ex0:return false;
}

jmn::B8 Create(VkAllocationCallbacks const *ac, VkDevice dev, VkCommandPoolCreateFlags flags, jmn::U32 qfi, VkCommandPool &command_pool, jmn::Result &result)
{
    VkCommandPoolCreateInfo ci;
    ci.sType            = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    ci.pNext            = NULL;
    ci.flags            = flags;
    ci.queueFamilyIndex = qfi;
    VK_CHECK(vkCreateCommandPool(dev, &ci, ac, &command_pool), result, ex0);
    return true;
ex0:return false;
}

jmn::B8 Create(VkAllocationCallbacks const *ac, VkDevice dev, uint32_t set_layout_count, VkDescriptorSetLayout const *set_layout_array, uint32_t push_constant_range_count, VkPushConstantRange const *push_constant_range_array, VkPipelineLayout &pipeline_layout, jmn::Result &result)
{
    VkPipelineLayoutCreateInfo ci;
    ci.sType                  = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    ci.pNext                  = NULL;
    ci.flags                  = 0;
    ci.setLayoutCount         = set_layout_count;
    ci.pSetLayouts            = set_layout_array;
    ci.pushConstantRangeCount = push_constant_range_count;
    ci.pPushConstantRanges    = push_constant_range_array;
    VK_CHECK(vkCreatePipelineLayout(dev, &ci, ac, &pipeline_layout), result, ex0);
    return true;
ex0:return false;
}

jmn::B8 Allocate(VkDevice dev, VkCommandPool command_pool, VkCommandBufferLevel level, jmn::U32 command_buffer_count, VkCommandBuffer *command_buffer_array, jmn::Result &result)
{
    VkCommandBufferAllocateInfo cbai;
    cbai.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    cbai.pNext              = NULL;
    cbai.commandPool        = command_pool;
    cbai.level              = level;
    cbai.commandBufferCount = command_buffer_count;
    VK_CHECK(vkAllocateCommandBuffers(dev, &cbai, command_buffer_array), result, ex0);
    return true;
ex0:return false;
}

jmn::B8 AllocateDedicated(VkAllocationCallbacks const *ac, VkPhysicalDeviceMemoryProperties const &pdmp, VkDevice dev, VkBuffer buffer, VkMemoryPropertyFlags flags, VkDeviceMemory &memory, jmn::Result &result)
{
    VkBufferMemoryRequirementsInfo2 bmri2;
    bmri2.sType  = VK_STRUCTURE_TYPE_BUFFER_MEMORY_REQUIREMENTS_INFO_2;
    bmri2.pNext  = NULL;
    bmri2.buffer = buffer;

    VkMemoryDedicatedRequirements mdr;
    VkMemoryRequirements2         mr2;

    mr2.sType = VK_STRUCTURE_TYPE_MEMORY_REQUIREMENTS_2;
    mdr.sType = VK_STRUCTURE_TYPE_MEMORY_DEDICATED_REQUIREMENTS;

    mr2.pNext = &mdr;
    mdr.pNext = NULL;

    vkGetBufferMemoryRequirements2(dev, &bmri2, &mr2);

    VkMemoryDedicatedAllocateInfo mdai;
    VkMemoryAllocateFlagsInfo     mafi;
    VkMemoryAllocateInfo          mai;

    mai .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    mafi.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_FLAGS_INFO;
    mdai.sType = VK_STRUCTURE_TYPE_MEMORY_DEDICATED_ALLOCATE_INFO;

    mai .pNext = &mafi;
    mafi.pNext = mdr.prefersDedicatedAllocation ? &mdai : NULL;
    mdai.pNext = NULL;

    mai.allocationSize = mr2.memoryRequirements.size;
    if (!SelectMemoryTypeIndex(pdmp, mr2.memoryRequirements.memoryTypeBits, flags, mai.memoryTypeIndex, result)) goto ex0;

    mafi.flags      = 0;
    mafi.deviceMask = 0;

    mdai.image  = VK_NULL_HANDLE;
    mdai.buffer = buffer;

    VK_CHECK(vkAllocateMemory(dev, &mai, ac, &memory), result, ex0);
    return true;
ex0:return false;
}

void CmdBeginDebugUtilsLabel(VkCommandBuffer cb, jmn::C8 const *name, jmn::V4F32 color)
{
    VkDebugUtilsLabelEXT dul;
    dul.sType      = VK_STRUCTURE_TYPE_DEBUG_UTILS_LABEL_EXT;
    dul.pNext      = NULL;
    dul.pLabelName = name;
    dul.color[0]   = color.r;
    dul.color[1]   = color.g;
    dul.color[2]   = color.b;
    dul.color[3]   = color.a;
    vkCmdBeginDebugUtilsLabelEXT(cb, &dul);
}

void CmdInsertDebugUtilsLabel(VkCommandBuffer cb, jmn::C8 const *name, jmn::V4F32 color)
{
    VkDebugUtilsLabelEXT dul;
    dul.sType      = VK_STRUCTURE_TYPE_DEBUG_UTILS_LABEL_EXT;
    dul.pNext      = NULL;
    dul.pLabelName = name;
    dul.color[0]   = color.r;
    dul.color[1]   = color.g;
    dul.color[2]   = color.b;
    dul.color[3]   = color.a;
    vkCmdInsertDebugUtilsLabelEXT(cb, &dul);
}
void QueueBeginDebugUtilsLabel(VkQueue q, jmn::C8 const *name, jmn::V4F32 color)
{
    VkDebugUtilsLabelEXT dul;
    dul.sType      = VK_STRUCTURE_TYPE_DEBUG_UTILS_LABEL_EXT;
    dul.pNext      = NULL;
    dul.pLabelName = name;
    dul.color[0]   = color.r;
    dul.color[1]   = color.g;
    dul.color[2]   = color.b;
    dul.color[3]   = color.a;
    vkQueueBeginDebugUtilsLabelEXT(q, &dul);
}

void QueueInsertDebugUtilsLabel(VkQueue q, jmn::C8 const *name, jmn::V4F32 color)
{
    VkDebugUtilsLabelEXT dul;
    dul.sType      = VK_STRUCTURE_TYPE_DEBUG_UTILS_LABEL_EXT;
    dul.pNext      = NULL;
    dul.pLabelName = name;
    dul.color[0]   = color.r;
    dul.color[1]   = color.g;
    dul.color[2]   = color.b;
    dul.color[3]   = color.a;
    vkQueueInsertDebugUtilsLabelEXT(q, &dul);
}

jmn::B8 SetDebugUtilsObjectName(VkDevice dev, VkObjectType type, jmn::Handle handle, jmn::C8 const *name, jmn::Result &result)
{
    VkDebugUtilsObjectNameInfoEXT duoni;
    duoni.sType        = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
    duoni.pNext        = NULL;
    duoni.objectType   = type;
    duoni.objectHandle = handle;
    duoni.pObjectName  = name;
    VK_CHECK(vkSetDebugUtilsObjectNameEXT(dev, &duoni), result, ex0);
    return true;
ex0:return false;
}

jmn::B8 SetDebugUtilsObjectTag(VkDevice dev, VkObjectType type, jmn::Handle handle, jmn::U64 tag_id, jmn::Size tag_size, jmn::Addr tag_data, jmn::Result &result)
{
    VkDebugUtilsObjectTagInfoEXT duoti;
    duoti.sType        = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_TAG_INFO_EXT;
    duoti.pNext        = NULL;
    duoti.objectType   = type;
    duoti.objectHandle = handle;
    duoti.tagName      = tag_id;
    duoti.tagSize      = tag_size;
    duoti.pTag         = (void const *)tag_data;
    VK_CHECK(vkSetDebugUtilsObjectTagEXT(dev, &duoti), result, ex0);
    return true;
ex0:return false;
}

#  endif // VULKAN_COMMON_IMPLEMENTATED
#endif // VULKAN_COMMON_IMPLEMENTATION
