#ifndef VULKAN_COMMON_INCLUDED
#  define VULKAN_COMMON_INCLUDED

#  include <Hydrogen.hpp>

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

#  endif // VULKAN_COMMON_IMPLEMENTATED
#endif // VULKAN_COMMON_IMPLEMENTATION
