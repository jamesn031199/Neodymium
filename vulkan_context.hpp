#ifndef VULKAN_CONTEXT_INCLUDED
#  define VULKAN_CONTEXT_INCLUDED

#  include <Windows.h>

#  include "vulkan_common.hpp"

struct VulkanContext
{
    static inline constexpr jmn::Size HeapSize = jmn::MiB(128);

    jmn::Allocator                                        allocator;
    jmn::MemoryHeap                                       heap;
    CRITICAL_SECTION                                      heap_cs;
    VkAllocationCallbacks                                *ac;
    VkInstance                                            ins;
    VkDebugUtilsMessengerEXT                              dbg_msgr;
    VkPhysicalDevice                                      pd;
    jmn::Size                                             pd_qfp_c;
    VkQueueFamilyProperties2                             *pd_qfp_a;
    VkPhysicalDeviceMemoryProperties2                     pd_mp2;
    VkPhysicalDeviceProperties2                           pd_p2;
    VkPhysicalDeviceVulkan11Properties                    pd_vk11_p;
    VkPhysicalDeviceVulkan12Properties                    pd_vk12_p;
    VkPhysicalDeviceVulkan13Properties                    pd_vk13_p;
    VkPhysicalDeviceVulkan14Properties                    pd_vk14_p;
    VkPhysicalDeviceFeatures2                             pd_f2;
    VkPhysicalDeviceVulkan11Features                      pd_vk11_f;
    VkPhysicalDeviceVulkan12Features                      pd_vk12_f;
    VkPhysicalDeviceVulkan13Features                      pd_vk13_f;
    VkPhysicalDeviceVulkan14Features                      pd_vk14_f;
    VkPhysicalDeviceSwapchainMaintenance1FeaturesKHR      pd_sm1_f;
    VkPhysicalDevicePresentModeFifoLatestReadyFeaturesKHR pd_pmflr_f;
    jmn::U32                                              g_qfi;
    jmn::U32                                              c_qfi;
    jmn::U32                                              t_qfi;
    jmn::U32                                              g_qi;
    jmn::U32                                              c_qi;
    jmn::U32                                              tr_qi;
    jmn::U32                                              tw_qi; jmn::U8 _pad1[4];
    VkDevice                                              dev;
    VkQueue                                               g_q;
    VkQueue                                               c_q;
    VkQueue                                               tr_q;
    VkQueue                                               tw_q;
};

jmn::B8 Create(VulkanContext &vkc, jmn::Allocator allocator, jmn::U32 api_version, PFN_vkDebugUtilsMessengerCallbackEXT dbg_callback, void *dbg_user_data, jmn::Result &result);
void    Destroy(VulkanContext &vkc);

#endif // VULKAN_CONTEXT_INCLUDED

#ifdef __INTELLISENSE__
#  define VULKAN_CONTEXT_IMPLEMENTATION
#endif // __INTELLISENSE__

#ifdef VULKAN_CONTEXT_IMPLEMENTATION
#  ifndef VULKAN_CONTEXT_IMPLEMENTATED
#    define VULKAN_CONTEXT_IMPLEMENTATED

namespace VulkanContextInternal
{

    static jmn::C8 const *DeprecatedVulkanExtensions[] = {
        VK_KHR_16BIT_STORAGE_EXTENSION_NAME,
        VK_KHR_8BIT_STORAGE_EXTENSION_NAME,
        VK_KHR_BIND_MEMORY_2_EXTENSION_NAME,
        VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME,
        VK_KHR_COPY_COMMANDS_2_EXTENSION_NAME,
        VK_KHR_CREATE_RENDERPASS_2_EXTENSION_NAME,
        VK_KHR_DEDICATED_ALLOCATION_EXTENSION_NAME,
        VK_KHR_DEPTH_STENCIL_RESOLVE_EXTENSION_NAME,
        VK_KHR_DESCRIPTOR_UPDATE_TEMPLATE_EXTENSION_NAME,
        VK_KHR_DEVICE_GROUP_EXTENSION_NAME,
        VK_KHR_DEVICE_GROUP_CREATION_EXTENSION_NAME,
        VK_KHR_DRAW_INDIRECT_COUNT_EXTENSION_NAME,
        VK_KHR_DRIVER_PROPERTIES_EXTENSION_NAME,
        VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME,
        VK_KHR_DYNAMIC_RENDERING_LOCAL_READ_EXTENSION_NAME,
        VK_KHR_EXTERNAL_FENCE_EXTENSION_NAME,
        VK_KHR_EXTERNAL_FENCE_CAPABILITIES_EXTENSION_NAME,
        VK_KHR_EXTERNAL_MEMORY_EXTENSION_NAME,
        VK_KHR_EXTERNAL_MEMORY_CAPABILITIES_EXTENSION_NAME,
        VK_KHR_EXTERNAL_SEMAPHORE_EXTENSION_NAME,
        VK_KHR_EXTERNAL_SEMAPHORE_CAPABILITIES_EXTENSION_NAME,
        VK_KHR_FORMAT_FEATURE_FLAGS_2_EXTENSION_NAME,
        VK_KHR_GET_MEMORY_REQUIREMENTS_2_EXTENSION_NAME,
        VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME,
        VK_KHR_GLOBAL_PRIORITY_EXTENSION_NAME,
        VK_KHR_IMAGE_FORMAT_LIST_EXTENSION_NAME,
        VK_KHR_IMAGELESS_FRAMEBUFFER_EXTENSION_NAME,
        VK_KHR_INDEX_TYPE_UINT8_EXTENSION_NAME,
        VK_KHR_LINE_RASTERIZATION_EXTENSION_NAME,
        VK_KHR_LOAD_STORE_OP_NONE_EXTENSION_NAME,
        VK_KHR_MAINTENANCE_1_EXTENSION_NAME,
        VK_KHR_MAINTENANCE_2_EXTENSION_NAME,
        VK_KHR_MAINTENANCE_3_EXTENSION_NAME,
        VK_KHR_MAINTENANCE_4_EXTENSION_NAME,
        VK_KHR_MAINTENANCE_5_EXTENSION_NAME,
        VK_KHR_MAINTENANCE_6_EXTENSION_NAME,
        VK_KHR_MAP_MEMORY_2_EXTENSION_NAME,
        VK_KHR_MULTIVIEW_EXTENSION_NAME,
        VK_KHR_PUSH_DESCRIPTOR_EXTENSION_NAME,
        VK_KHR_RELAXED_BLOCK_LAYOUT_EXTENSION_NAME,
        VK_KHR_SAMPLER_MIRROR_CLAMP_TO_EDGE_EXTENSION_NAME,
        VK_KHR_SAMPLER_YCBCR_CONVERSION_EXTENSION_NAME,
        VK_KHR_SEPARATE_DEPTH_STENCIL_LAYOUTS_EXTENSION_NAME,
        VK_KHR_SHADER_ATOMIC_INT64_EXTENSION_NAME,
        VK_KHR_SHADER_DRAW_PARAMETERS_EXTENSION_NAME,
        VK_KHR_SHADER_EXPECT_ASSUME_EXTENSION_NAME,
        VK_KHR_SHADER_FLOAT16_INT8_EXTENSION_NAME,
        VK_KHR_SHADER_FLOAT_CONTROLS_EXTENSION_NAME,
        VK_KHR_SHADER_FLOAT_CONTROLS_2_EXTENSION_NAME,
        VK_KHR_SHADER_INTEGER_DOT_PRODUCT_EXTENSION_NAME,
        VK_KHR_SHADER_NON_SEMANTIC_INFO_EXTENSION_NAME,
        VK_KHR_SHADER_SUBGROUP_EXTENDED_TYPES_EXTENSION_NAME,
        VK_KHR_SHADER_SUBGROUP_ROTATE_EXTENSION_NAME,
        VK_KHR_SHADER_TERMINATE_INVOCATION_EXTENSION_NAME,
        VK_KHR_SPIRV_1_4_EXTENSION_NAME,
        VK_KHR_STORAGE_BUFFER_STORAGE_CLASS_EXTENSION_NAME,
        VK_KHR_SYNCHRONIZATION_2_EXTENSION_NAME,
        VK_KHR_TIMELINE_SEMAPHORE_EXTENSION_NAME,
        VK_KHR_UNIFORM_BUFFER_STANDARD_LAYOUT_EXTENSION_NAME,
        VK_KHR_VARIABLE_POINTERS_EXTENSION_NAME,
        VK_KHR_VERTEX_ATTRIBUTE_DIVISOR_EXTENSION_NAME,
        VK_KHR_VULKAN_MEMORY_MODEL_EXTENSION_NAME,
        VK_KHR_ZERO_INITIALIZE_WORKGROUP_MEMORY_EXTENSION_NAME,
        VK_EXT_4444_FORMATS_EXTENSION_NAME,
        VK_EXT_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME,
        VK_EXT_CALIBRATED_TIMESTAMPS_EXTENSION_NAME,
        VK_EXT_DEBUG_MARKER_EXTENSION_NAME,
        VK_EXT_DEBUG_REPORT_EXTENSION_NAME,
        VK_EXT_DEPTH_CLAMP_ZERO_ONE_EXTENSION_NAME,
        VK_EXT_DESCRIPTOR_BUFFER_EXTENSION_NAME,
        VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME,
        VK_EXT_DEVICE_FAULT_EXTENSION_NAME,
        VK_EXT_EXTENDED_DYNAMIC_STATE_EXTENSION_NAME,
        VK_EXT_EXTENDED_DYNAMIC_STATE_2_EXTENSION_NAME,
        VK_EXT_GLOBAL_PRIORITY_EXTENSION_NAME,
        VK_EXT_GLOBAL_PRIORITY_QUERY_EXTENSION_NAME,
        VK_EXT_HOST_IMAGE_COPY_EXTENSION_NAME,
        VK_EXT_HOST_QUERY_RESET_EXTENSION_NAME,
        VK_EXT_IMAGE_ROBUSTNESS_EXTENSION_NAME,
        VK_EXT_INDEX_TYPE_UINT8_EXTENSION_NAME,
        VK_EXT_INLINE_UNIFORM_BLOCK_EXTENSION_NAME,
        VK_EXT_LINE_RASTERIZATION_EXTENSION_NAME,
        VK_EXT_LOAD_STORE_OP_NONE_EXTENSION_NAME,
        VK_EXT_PIPELINE_CREATION_CACHE_CONTROL_EXTENSION_NAME,
        VK_EXT_PIPELINE_CREATION_FEEDBACK_EXTENSION_NAME,
        VK_EXT_PIPELINE_PROTECTED_ACCESS_EXTENSION_NAME,
        VK_EXT_PIPELINE_ROBUSTNESS_EXTENSION_NAME,
        VK_EXT_PRESENT_MODE_FIFO_LATEST_READY_EXTENSION_NAME,
        VK_EXT_PRIVATE_DATA_EXTENSION_NAME,
        VK_EXT_ROBUSTNESS_2_EXTENSION_NAME,
        VK_EXT_SAMPLER_FILTER_MINMAX_EXTENSION_NAME,
        VK_EXT_SCALAR_BLOCK_LAYOUT_EXTENSION_NAME,
        VK_EXT_SEPARATE_STENCIL_USAGE_EXTENSION_NAME,
        VK_EXT_SHADER_DEMOTE_TO_HELPER_INVOCATION_EXTENSION_NAME,
        VK_EXT_SHADER_SUBGROUP_BALLOT_EXTENSION_NAME,
        VK_EXT_SHADER_SUBGROUP_VOTE_EXTENSION_NAME,
        VK_EXT_SHADER_VIEWPORT_INDEX_LAYER_EXTENSION_NAME,
        VK_EXT_SUBGROUP_SIZE_CONTROL_EXTENSION_NAME,
        VK_EXT_SURFACE_MAINTENANCE_1_EXTENSION_NAME,
        VK_EXT_SWAPCHAIN_MAINTENANCE_1_EXTENSION_NAME,
        VK_EXT_TEXEL_BUFFER_ALIGNMENT_EXTENSION_NAME,
        VK_EXT_TEXTURE_COMPRESSION_ASTC_HDR_EXTENSION_NAME,
        VK_EXT_TOOLING_INFO_EXTENSION_NAME,
        VK_EXT_VALIDATION_FEATURES_EXTENSION_NAME,
        VK_EXT_VALIDATION_FLAGS_EXTENSION_NAME,
        VK_EXT_VERTEX_ATTRIBUTE_DIVISOR_EXTENSION_NAME,
        VK_EXT_VERTEX_ATTRIBUTE_ROBUSTNESS_EXTENSION_NAME,
        VK_EXT_YCBCR_2PLANE_444_FORMATS_EXTENSION_NAME,
        VK_AMD_DRAW_INDIRECT_COUNT_EXTENSION_NAME,
        VK_AMD_GPU_SHADER_HALF_FLOAT_EXTENSION_NAME,
        VK_AMD_GPU_SHADER_INT16_EXTENSION_NAME,
        VK_AMD_NEGATIVE_VIEWPORT_HEIGHT_EXTENSION_NAME,
        VK_ARM_RASTERIZATION_ORDER_ATTACHMENT_ACCESS_EXTENSION_NAME,
        VK_IMG_FORMAT_PVRTC_EXTENSION_NAME,
        "VK_MVK_ios_surface",
        "VK_MVK_macos_surface",
        VK_NV_COMPUTE_SHADER_DERIVATIVES_EXTENSION_NAME,
        VK_NV_COPY_MEMORY_INDIRECT_EXTENSION_NAME,
        VK_NV_DEDICATED_ALLOCATION_EXTENSION_NAME,
        "VK_NV_displacement_micromap",
        VK_NV_EXTERNAL_MEMORY_EXTENSION_NAME,
        VK_NV_EXTERNAL_MEMORY_CAPABILITIES_EXTENSION_NAME,
        VK_NV_EXTERNAL_MEMORY_WIN32_EXTENSION_NAME,
        VK_NV_FRAGMENT_SHADER_BARYCENTRIC_EXTENSION_NAME,
        VK_NV_GLSL_SHADER_EXTENSION_NAME,
        VK_NV_MEMORY_DECOMPRESSION_EXTENSION_NAME,
        VK_NV_RAY_TRACING_EXTENSION_NAME,
        VK_NV_RAY_TRACING_INVOCATION_REORDER_EXTENSION_NAME,
        VK_NV_SHADER_SUBGROUP_PARTITIONED_EXTENSION_NAME,
        VK_NV_WIN32_KEYED_MUTEX_EXTENSION_NAME,
        VK_QCOM_FRAGMENT_DENSITY_MAP_OFFSET_EXTENSION_NAME,
        VK_QCOM_RENDER_PASS_SHADER_RESOLVE_EXTENSION_NAME,
        VK_VALVE_MUTABLE_DESCRIPTOR_TYPE_EXTENSION_NAME,
    };

    static void* VKAPI_CALL VulkanAllocationCallback(void *user_data, size_t size, size_t alignment, VkSystemAllocationScope)
    {
        auto const vkc = (VulkanContext *)user_data;
        EnterCriticalSection(&vkc->heap_cs);
        auto const ret_addr = vkc->heap.Alloc((jmn::Size)size, (jmn::Size)alignment);
        LeaveCriticalSection(&vkc->heap_cs);
        return (void *)ret_addr;
    }

    static void* VKAPI_CALL VulkanRellocationCallback(void *user_data, void *ptr, size_t new_size, size_t alignment, VkSystemAllocationScope)
    {
        auto const vkc = (VulkanContext *)user_data;
        EnterCriticalSection(&vkc->heap_cs);
        auto const ret_addr = vkc->heap.Realloc((jmn::Addr)ptr, 0, (jmn::Size)new_size, (jmn::Size)alignment);
        LeaveCriticalSection(&vkc->heap_cs);
        return (void *)ret_addr;
    }

    static void VKAPI_CALL VulkanFreeCallback(void *user_data, void *ptr)
    {
        auto const vkc = (VulkanContext *)user_data;
        EnterCriticalSection(&vkc->heap_cs);
        vkc->heap.Free((jmn::Addr)ptr);
        LeaveCriticalSection(&vkc->heap_cs);
    }

    static jmn::B8 AppendIfSupported(VkLayerProperties const *property_array, jmn::U32 property_count, jmn::C8 const *const *input_array, jmn::U32 input_count, jmn::C8 const **output_array, jmn::U32 &output_count, jmn::Result &result)
    {
        for (jmn::U32 i = 0; i < input_count; ++i)
        {
            jmn::B8 supported = false;
            for (jmn::U32 j = 0; j < property_count; ++j)
            {
                jmn::B8 matching = true;
                for (jmn::U32 k = 0; k < VK_MAX_EXTENSION_NAME_SIZE; ++k)
                {
                    auto const A = property_array[j].layerName[k], B = input_array[i][k];
                    matching = A == B;
                    if (!A || !B || !matching) break;
                }
                if (matching)
                {
                    supported = true;
                    break;
                }
            }
            if (supported) output_array[output_count++] = input_array[i];
            JMN_CHECK(supported, result, jmn::Result::ErrorNotSupported, ex0);
        }
        return true;
    ex0:return false;
    }

    static jmn::B8 AppendIfSupported(VkExtensionProperties const *property_array, jmn::U32 property_count, jmn::C8 const *const *input_array, jmn::U32 input_count, jmn::C8 const **output_array, jmn::U32 &output_count, jmn::Result &result)
    {
        for (jmn::U32 i = 0; i < input_count; ++i)
        {
            jmn::B8 supported = false;
            for (jmn::U32 j = 0; j < property_count; ++j)
            {
                jmn::B8 matching = true;
                for (jmn::U32 k = 0; k < VK_MAX_EXTENSION_NAME_SIZE; ++k)
                {
                    auto const A = property_array[j].extensionName[k], B = input_array[i][k];
                    matching = A == B;
                    if (!A || !B || !matching) break;
                }
                if (matching)
                {
                    supported = true;
                    break;
                }
            }
            if (supported) output_array[output_count++] = input_array[i];
            JMN_CHECK(supported, result, jmn::Result::ErrorNotSupported, ex0);
        }
        return true;
    ex0:return false;
    }

    static void AppendIfSupported(VkLayerProperties const *property_array, jmn::U32 property_count, jmn::C8 const *const *input_array, jmn::U32 input_count, jmn::C8 const **output_array, jmn::U32 &output_count)
    {
        for (jmn::U32 i = 0; i < input_count; ++i)
        {
            jmn::B8 supported = false;
            for (jmn::U32 j = 0; j < property_count; ++j)
            {
                jmn::B8 matching = true;
                for (jmn::U32 k = 0; k < VK_MAX_EXTENSION_NAME_SIZE; ++k)
                {
                    auto const A = property_array[j].layerName[k], B = input_array[i][k];
                    matching = A == B;
                    if (!A || !B || !matching) break;
                }
                if (matching)
                {
                    supported = true;
                    break;
                }
            }
            if (supported) output_array[output_count++] = input_array[i];
        }
    }

    static void AppendIfSupported(VkExtensionProperties const *property_array, jmn::U32 property_count, jmn::C8 const *const *input_array, jmn::U32 input_count, jmn::C8 const **output_array, jmn::U32 &output_count)
    {
        for (jmn::U32 i = 0; i < input_count; ++i)
        {
            jmn::B8 supported = false;
            for (jmn::U32 j = 0; j < property_count; ++j)
            {
                jmn::B8 matching = true;
                for (jmn::U32 k = 0; k < VK_MAX_EXTENSION_NAME_SIZE; ++k)
                {
                    auto const A = property_array[j].extensionName[k], B = input_array[i][k];
                    matching = A == B;
                    if (!A || !B || !matching) break;
                }
                if (matching)
                {
                    supported = true;
                    break;
                }
            }
            if (supported) output_array[output_count++] = input_array[i];
        }
    }

    static void RemoveDeprecated(VkExtensionProperties *property_array, jmn::U32 &property_count)
    {
        for (jmn::U32 i = 0; i < property_count;)
        {
            jmn::B8 deprecated = false;
            for (jmn::Size j = 0; j < jmn::Length(DeprecatedVulkanExtensions); ++j)
            {
                jmn::B8 matching = true;
                for (jmn::U32 k = 0; k < VK_MAX_EXTENSION_NAME_SIZE; ++k)
                {
                    auto const A = property_array[i].extensionName[k], B = DeprecatedVulkanExtensions[j][k];
                    matching = A == B;
                    if (!A || !B || !matching) break;
                }
                if (matching)
                {
                    deprecated = true;
                    break;
                }
            }
            if (deprecated)
            {
                jmn::Move(property_array + i, property_array + i + 1, property_count - 1 - i);
                property_count -= 1;
            }
            else
            {
                i += 1;
            }
        }
    }

    static void AppendOrExtend(VkQueueFamilyProperties2 const *qfps, jmn::F32 const *qp, VkDeviceQueueCreateInfo *dqcis, jmn::Size &dqcic, jmn::U32 qfi, jmn::U32 &qi)
    {
        jmn::B8 unique = true;
        for (jmn::U32 i = 0; i < dqcic; ++i)
        {
            if (dqcis[i].queueFamilyIndex == qfi)
            {
                auto const max_queue_count = qfps[qfi].queueFamilyProperties.queueCount;
                auto      &queue_count     = dqcis[i].queueCount;
                if (queue_count < max_queue_count)
                {
                    qi = queue_count++;
                }
                else
                {
                    qi = queue_count - 1;
                }
                unique = false;
                break;
            }
        }
        if (unique)
        {
            auto dqci = dqcis + dqcic++;
            dqci->sType            = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            dqci->pNext            = NULL;
            dqci->flags            = 0;
            dqci->queueFamilyIndex = qfi;
            dqci->queueCount       = 1;
            dqci->pQueuePriorities = qp;
            qi = 0;
        }
    }

    static jmn::B8 AppendInstanceLayers(jmn::Allocator allocator, jmn::U32 required_lyr_name_count, jmn::C8 const *const *required_lyr_name_array, jmn::U32 optional_lyr_name_count, jmn::C8 const *const *optional_lyr_name_array,
        jmn::U32 &enabled_lyr_name_count, jmn::C8 const **enabled_lyr_name_array, jmn::Result &result)
    {
        jmn::B8            success        = false;
        jmn::U32           property_count = 0;
        VkLayerProperties *property_array = NULL;
        VK_CHECK(vkEnumerateInstanceLayerProperties(&property_count, property_array), result, ex0);
        if (!allocator.Alloc(property_count, property_array, result)) goto ex0;
        VK_CHECK(vkEnumerateInstanceLayerProperties(&property_count, property_array), result, ex1);
        if (!AppendIfSupported(property_array, property_count, required_lyr_name_array, required_lyr_name_count, enabled_lyr_name_array, enabled_lyr_name_count, result)) goto ex1;
        AppendIfSupported(property_array, property_count, optional_lyr_name_array, optional_lyr_name_count, enabled_lyr_name_array, enabled_lyr_name_count);
        success = true;
    ex1:allocator.Free(property_array, property_count);
    ex0:return success;
    }

    static jmn::B8 AppendInstanceExtensions(jmn::Allocator allocator, jmn::U32 required_lyr_name_count, jmn::C8 const *const *required_lyr_name_array, jmn::U32 optional_lyr_name_count, jmn::C8 const *const *optional_lyr_name_array,
        jmn::U32 &enabled_lyr_name_count, jmn::C8 const **enabled_lyr_name_array, jmn::Result &result)
    {
        jmn::B8                success        = false;
        jmn::U32               property_count = 0;
        VkExtensionProperties *property_array = NULL;
        VK_CHECK(vkEnumerateInstanceExtensionProperties(NULL, &property_count, property_array), result, ex0);
        if (!allocator.Alloc(property_count, property_array, result)) goto ex0;
        VK_CHECK(vkEnumerateInstanceExtensionProperties(NULL, &property_count, property_array), result, ex1);
        RemoveDeprecated(property_array, property_count);
        if (!AppendIfSupported(property_array, property_count, required_lyr_name_array, required_lyr_name_count, enabled_lyr_name_array, enabled_lyr_name_count, result)) goto ex1;
        AppendIfSupported(property_array, property_count, optional_lyr_name_array, optional_lyr_name_count, enabled_lyr_name_array, enabled_lyr_name_count);
        success = true;
    ex1:allocator.Free(property_array, property_count);
    ex0:return success;
    }

    static jmn::B8 AppendDeviceExtensions(jmn::Allocator allocator, VkPhysicalDevice pd, jmn::U32 required_lyr_name_count, jmn::C8 const *const *required_lyr_name_array, jmn::U32 optional_lyr_name_count, jmn::C8 const *const *optional_lyr_name_array,
        jmn::U32 &enabled_lyr_name_count, jmn::C8 const **enabled_lyr_name_array, jmn::Result &result)
    {
        jmn::B8                success        = false;
        jmn::U32               property_count = 0;
        VkExtensionProperties *property_array = NULL;
        VK_CHECK(vkEnumerateDeviceExtensionProperties(pd, NULL, &property_count, property_array), result, ex0);
        if (!allocator.Alloc(property_count, property_array, result)) goto ex0;
        VK_CHECK(vkEnumerateDeviceExtensionProperties(pd, NULL, &property_count, property_array), result, ex1);
        RemoveDeprecated(property_array, property_count);
        if (!AppendIfSupported(property_array, property_count, required_lyr_name_array, required_lyr_name_count, enabled_lyr_name_array, enabled_lyr_name_count, result)) goto ex1;
        AppendIfSupported(property_array, property_count, optional_lyr_name_array, optional_lyr_name_count, enabled_lyr_name_array, enabled_lyr_name_count);
        success = true;
    ex1:allocator.Free(property_array, property_count);
    ex0:return success;
    }

    static jmn::B8 SelectPhysicalDevice(jmn::Allocator allocator, VkInstance ins, VkPhysicalDevice &pd, jmn::Result &result)
    {
        jmn::B8                     success               = false;
        jmn::U32                    physical_device_count = 0;
        VkPhysicalDevice           *physical_device_array = NULL;
        VkPhysicalDeviceProperties *property_array        = NULL;
        VK_CHECK(vkEnumeratePhysicalDevices(ins, &physical_device_count, physical_device_array), result, ex0);
        if (!allocator.Alloc(physical_device_count, physical_device_array, result)) goto ex0;
        if (!allocator.Alloc(physical_device_count, property_array, result)) goto ex1;
        VK_CHECK(vkEnumeratePhysicalDevices(ins, &physical_device_count, physical_device_array), result, ex2);
        for (jmn::U32 i = 0; i < physical_device_count; ++i) vkGetPhysicalDeviceProperties(physical_device_array[i], property_array + i);

        pd = VK_NULL_HANDLE;
    #if 1
        for (jmn::U32 i = 0; i < physical_device_count; ++i)
        {
            if (property_array[i].deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
            {
                pd = physical_device_array[i];
                break;
            }
        }
    #else
        for (jmn::U32 i = 0; i < physical_device_count; ++i)
        {
            if (property_array[i].deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU)
            {
                pd = physical_device_array[i];
                break;
            }
        }
    #endif
        JMN_CHECK(pd != VK_NULL_HANDLE, result, jmn::Result::ErrorNotSupported, ex2);
        success = true;
    ex2:allocator.Free(property_array, physical_device_count);
    ex1:allocator.Free(physical_device_array, physical_device_count);
    ex0:return success;
    }

    jmn::B8 CreateEnvironment(VulkanContext &vkc, jmn::Result &result)
    {
        VK_CHECK(volkInitialize(), result, ex0);

        JMN_CHECK(::InitializeCriticalSectionEx(&vkc.heap_cs, 4096, 0), result, jmn::Result::ErrorGeneric, ex1);

        if (!vkc.allocator.Alloc(1, vkc.ac, result)) goto ex2;
        vkc.ac->pUserData             = &vkc;
        vkc.ac->pfnAllocation         = VulkanAllocationCallback;
        vkc.ac->pfnReallocation       = VulkanRellocationCallback;
        vkc.ac->pfnFree               = VulkanFreeCallback;
        vkc.ac->pfnInternalAllocation = NULL;
        vkc.ac->pfnInternalFree       = NULL;

        return true;
    ex2:; ::DeleteCriticalSection(&vkc.heap_cs);
    ex1:volkFinalize();
    ex0:return false;
    }

    static jmn::B8 CreateInstance(VulkanContext &vkc, jmn::U32 api_version, PFN_vkDebugUtilsMessengerCallbackEXT dbg_callback, void *dbg_user_data, jmn::Result &result)
    {
        jmn::U32       required_lyr_name_count = 0;
        jmn::C8 const *required_lyr_name_array[8] ={};
        jmn::U32       required_ext_name_count = 0;
        jmn::C8 const *required_ext_name_array[8] ={};

        jmn::U32       optional_lyr_name_count = 0;
        jmn::C8 const *optional_lyr_name_array[8] ={};
        jmn::U32       optional_ext_name_count = 0;
        jmn::C8 const *optional_ext_name_array[8] ={};

        jmn::U32       enabled_lyr_name_count = 0;
        jmn::C8 const *enabled_lyr_name_array[8] ={};
        jmn::U32       enabled_ext_name_count = 0;
        jmn::C8 const *enabled_ext_name_array[8] ={};

        required_ext_name_array[required_ext_name_count++] = VK_KHR_SURFACE_EXTENSION_NAME;
        required_ext_name_array[required_ext_name_count++] = VK_KHR_GET_SURFACE_CAPABILITIES_2_EXTENSION_NAME;
        required_ext_name_array[required_ext_name_count++] = VK_KHR_SURFACE_MAINTENANCE_1_EXTENSION_NAME;
        required_ext_name_array[required_ext_name_count++] = VK_KHR_WIN32_SURFACE_EXTENSION_NAME;
        required_ext_name_array[required_ext_name_count++] = VK_EXT_SWAPCHAIN_COLOR_SPACE_EXTENSION_NAME;

    #ifdef DEBUG_MODE
        optional_ext_name_array[optional_ext_name_count++] = VK_EXT_DEBUG_UTILS_EXTENSION_NAME;
        optional_lyr_name_array[optional_lyr_name_count++] = "VK_LAYER_KHRONOS_validation";
        optional_lyr_name_array[optional_lyr_name_count++] = "VK_LAYER_LUNARG_monitor";
    #endif

        if (!AppendInstanceLayers    (vkc.allocator, required_lyr_name_count, required_lyr_name_array, optional_lyr_name_count, optional_lyr_name_array, enabled_lyr_name_count, enabled_lyr_name_array, result)) goto ex0;
        if (!AppendInstanceExtensions(vkc.allocator, required_ext_name_count, required_ext_name_array, optional_ext_name_count, optional_ext_name_array, enabled_ext_name_count, enabled_ext_name_array, result)) goto ex0;

        {
            VkApplicationInfo ai;
            ai.sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO;
            ai.pNext              = NULL;
            ai.pApplicationName   = NULL;
            ai.applicationVersion = VK_MAKE_API_VERSION(0, 0, 0, 0);
            ai.pEngineName        = NULL;
            ai.engineVersion      = VK_MAKE_API_VERSION(0, 0, 0, 0);
            ai.apiVersion         = api_version;
            VkInstanceCreateInfo ci;
            ci.sType                   = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
            ci.pNext                   = NULL;
            ci.flags                   = 0;
            ci.pApplicationInfo        = &ai;
            ci.enabledLayerCount       = enabled_lyr_name_count;
            ci.ppEnabledLayerNames     = enabled_lyr_name_array;
            ci.enabledExtensionCount   = enabled_ext_name_count;
            ci.ppEnabledExtensionNames = enabled_ext_name_array;
            VK_CHECK(vkCreateInstance(&ci, vkc.ac, &vkc.ins), result, ex0);
        }

        volkLoadInstanceOnly(vkc.ins);

    #ifdef DEBUG_MODE
        if (vkCreateDebugUtilsMessengerEXT)
        {
            VkDebugUtilsMessengerCreateInfoEXT ci;
            ci.sType           = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
            ci.pNext           = NULL;
            ci.flags           = 0;
            ci.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT;
            ci.messageType     = VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT;
            ci.pfnUserCallback = dbg_callback;
            ci.pUserData       = dbg_user_data;
            VK_CHECK(vkCreateDebugUtilsMessengerEXT(vkc.ins, &ci, vkc.ac, &vkc.dbg_msgr), result, ex1);
        }
    #else
        (void)dbg_callback, (void)dbg_user_data;
    #endif

        return true;
    #ifdef DEBUG_MODE
        //ex2:if (vkc.dbg_msgr != VK_NULL_HANDLE) vkDestroyDebugUtilsMessengerEXT(vkc.ins, vkc.dbg_msgr, vkc.ac);
        ex1:vkDestroyInstance(vkc.ins, vkc.ac);
    #endif
    ex0:return false;
    }

    static jmn::B8 CreatePhysicalDevice(VulkanContext &vkc, jmn::Result &result)
    {
        if (!SelectPhysicalDevice(vkc.allocator, vkc.ins, vkc.pd, result)) goto ex0;

        vkc.pd_qfp_c = 0;
        vkc.pd_qfp_a = NULL;
        vkGetPhysicalDeviceQueueFamilyProperties2(vkc.pd, (jmn::U32 *)&vkc.pd_qfp_c, vkc.pd_qfp_a);

        if (!vkc.allocator.Alloc(vkc.pd_qfp_c, vkc.pd_qfp_a, result)) goto ex0;
        for (jmn::Size i = 0; i < vkc.pd_qfp_c; ++i)
        {
            vkc.pd_qfp_a[i].sType = VK_STRUCTURE_TYPE_QUEUE_FAMILY_PROPERTIES_2;
            vkc.pd_qfp_a[i].pNext = NULL;
        }

        vkGetPhysicalDeviceQueueFamilyProperties2(vkc.pd, (jmn::U32 *)&vkc.pd_qfp_c, vkc.pd_qfp_a);

        vkc.pd_mp2    .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MEMORY_PROPERTIES_2;
        vkc.pd_p2     .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
        vkc.pd_vk11_p .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_PROPERTIES;
        vkc.pd_vk12_p .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_PROPERTIES;
        vkc.pd_vk13_p .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_PROPERTIES;
        vkc.pd_vk14_p .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_4_PROPERTIES;
        vkc.pd_f2     .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
        vkc.pd_vk11_f .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES;
        vkc.pd_vk12_f .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;
        vkc.pd_vk13_f .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;
        vkc.pd_vk14_f .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_4_FEATURES;
        vkc.pd_sm1_f  .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SWAPCHAIN_MAINTENANCE_1_FEATURES_KHR;
        vkc.pd_pmflr_f.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PRESENT_MODE_FIFO_LATEST_READY_FEATURES_KHR;

        vkc.pd_mp2    .pNext = NULL;
        vkc.pd_p2     .pNext = &vkc.pd_vk11_p;
        vkc.pd_vk11_p .pNext = &vkc.pd_vk12_p;
        vkc.pd_vk12_p .pNext = &vkc.pd_vk13_p;
        vkc.pd_vk13_p .pNext = &vkc.pd_vk14_p;
        vkc.pd_vk14_p .pNext = NULL;
        vkc.pd_f2     .pNext = &vkc.pd_vk11_f;
        vkc.pd_vk11_f .pNext = &vkc.pd_vk12_f;
        vkc.pd_vk12_f .pNext = &vkc.pd_vk13_f;
        vkc.pd_vk13_f .pNext = &vkc.pd_vk14_f;
        vkc.pd_vk14_f .pNext = &vkc.pd_sm1_f;
        vkc.pd_sm1_f  .pNext = &vkc.pd_pmflr_f;
        vkc.pd_pmflr_f.pNext = NULL;

        vkGetPhysicalDeviceMemoryProperties2(vkc.pd, &vkc.pd_mp2);

        vkGetPhysicalDeviceProperties2(vkc.pd, &vkc.pd_p2);

        if (!vkc.allocator.Alloc(vkc.pd_vk14_p.copySrcLayoutCount, vkc.pd_vk14_p.pCopySrcLayouts, result)) goto ex1;
        if (!vkc.allocator.Alloc(vkc.pd_vk14_p.copyDstLayoutCount, vkc.pd_vk14_p.pCopyDstLayouts, result)) goto ex2;

        vkGetPhysicalDeviceProperties2(vkc.pd, &vkc.pd_p2);

        vkGetPhysicalDeviceFeatures2(vkc.pd, &vkc.pd_f2);

        return true;
        //ex3:vkc.allocator.Free(vkc.pd_vk14p.pCopyDstLayouts, vkc.pd_vk14p.copyDstLayoutCount);
    ex2:vkc.allocator.Free(vkc.pd_vk14_p.pCopySrcLayouts, vkc.pd_vk14_p.copySrcLayoutCount);
    ex1:vkc.allocator.Free(vkc.pd_qfp_a, vkc.pd_qfp_c);
    ex0:return false;
    }

    static jmn::B8 SelectQueueFamilies(VulkanContext &vkc, jmn::Result &result)
    {
        VkQueueFlags const SearchMask   = VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT | VK_QUEUE_TRANSFER_BIT;
        VkQueueFlags const GraphicsMask = VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT | VK_QUEUE_TRANSFER_BIT;
        VkQueueFlags const TransferMask =                                                VK_QUEUE_TRANSFER_BIT;

        vkc.g_qfi = UINT32_MAX;
        for (jmn::Size i = 0; i < vkc.pd_qfp_c; ++i)
        {
            auto const qf = vkc.pd_qfp_a[i].queueFamilyProperties.queueFlags & SearchMask;
            if (qf == GraphicsMask)
            {
                vkc.g_qfi = (jmn::U32)i;
                break;
            }
        }
        JMN_CHECK(vkc.g_qfi != UINT32_MAX, result, jmn::Result::ErrorNotSupported, ex0);

        JMN_CHECK(vkGetPhysicalDeviceWin32PresentationSupportKHR(vkc.pd, vkc.g_qfi), result, jmn::Result::ErrorNotSupported, ex0);

        vkc.c_qfi = UINT32_MAX;
        for (jmn::Size i = 0; i < vkc.pd_qfp_c; ++i)
        {
            auto const qf = vkc.pd_qfp_a[i].queueFamilyProperties.queueFlags & SearchMask;
            if (!(qf & VK_QUEUE_GRAPHICS_BIT) && (qf & VK_QUEUE_COMPUTE_BIT))
            {
                vkc.c_qfi = (jmn::U32)i;
                break;
            }
        }
        if (vkc.c_qfi == UINT32_MAX) vkc.c_qfi = vkc.g_qfi;

        vkc.t_qfi = UINT32_MAX;
        for (jmn::Size i = 0; i < vkc.pd_qfp_c; ++i)
        {
            auto const qf = vkc.pd_qfp_a[i].queueFamilyProperties.queueFlags & SearchMask;
            if (qf == TransferMask)
            {
                vkc.t_qfi = (jmn::U32)i;
                break;
            }
        }
        if (vkc.t_qfi == UINT32_MAX) vkc.t_qfi = vkc.c_qfi;

        return true;
    ex0:return false;
    }

    static jmn::B8 CreateDevice(VulkanContext &vkc, jmn::Result &result)
    {
        jmn::F32 const          dqp[2]   ={ 1.0f, 1.0f };
        jmn::Size               dqcic    = 0;
        VkDeviceQueueCreateInfo dqcis[4] ={};
        AppendOrExtend(vkc.pd_qfp_a, dqp, dqcis, dqcic, vkc.g_qfi, vkc.g_qi);
        AppendOrExtend(vkc.pd_qfp_a, dqp, dqcis, dqcic, vkc.c_qfi, vkc.c_qi);
        AppendOrExtend(vkc.pd_qfp_a, dqp, dqcis, dqcic, vkc.t_qfi, vkc.tr_qi);
        AppendOrExtend(vkc.pd_qfp_a, dqp, dqcis, dqcic, vkc.t_qfi, vkc.tw_qi);

        jmn::U32       required_ext_name_count = 0;
        jmn::C8 const *required_ext_name_array[8] ={};

        jmn::U32       optional_ext_name_count = 0;
        jmn::C8 const *optional_ext_name_array[8] ={};

        jmn::U32       enabled_ext_name_count = 0;
        jmn::C8 const *enabled_ext_name_array[8] ={};

        required_ext_name_array[required_ext_name_count++] = VK_KHR_SWAPCHAIN_EXTENSION_NAME;
        required_ext_name_array[required_ext_name_count++] = VK_KHR_SWAPCHAIN_MAINTENANCE_1_EXTENSION_NAME;

        optional_ext_name_array[optional_ext_name_count++] = VK_KHR_PRESENT_MODE_FIFO_LATEST_READY_EXTENSION_NAME;
        optional_ext_name_array[optional_ext_name_count++] = VK_EXT_FULL_SCREEN_EXCLUSIVE_EXTENSION_NAME;

        if (!AppendDeviceExtensions(vkc.allocator, vkc.pd, required_ext_name_count, required_ext_name_array, optional_ext_name_count, optional_ext_name_array, enabled_ext_name_count, enabled_ext_name_array, result)) goto ex0;

        {
            VkPhysicalDevicePresentModeFifoLatestReadyFeaturesKHR e_pmflr_f ={ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PRESENT_MODE_FIFO_LATEST_READY_FEATURES_KHR, NULL };
            VkPhysicalDeviceSwapchainMaintenance1FeaturesKHR      e_sm1_f   ={ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SWAPCHAIN_MAINTENANCE_1_FEATURES_KHR       , &e_pmflr_f };
            VkPhysicalDeviceVulkan14Features                      e_vk14_f  ={ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_4_FEATURES                        , &e_sm1_f };
            VkPhysicalDeviceVulkan13Features                      e_vk13_f  ={ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES                        , &e_vk14_f };
            VkPhysicalDeviceVulkan12Features                      e_vk12_f  ={ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES                        , &e_vk13_f };
            VkPhysicalDeviceVulkan11Features                      e_vk11_f  ={ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES                        , &e_vk12_f };
            VkPhysicalDeviceFeatures2                             e_f2      ={ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2                                 , &e_vk11_f };
            VkDeviceCreateInfo                                    ci        ={ VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO                                         , &e_f2 };

            e_vk12_f.timelineSemaphore              = VK_TRUE;
            e_vk13_f.synchronization2               = VK_TRUE;
            e_vk13_f.shaderDemoteToHelperInvocation = VK_TRUE;
            e_vk13_f.dynamicRendering               = VK_TRUE;
            e_vk14_f.maintenance5                   = VK_TRUE;
            e_sm1_f .swapchainMaintenance1          = VK_TRUE;
            e_pmflr_f.presentModeFifoLatestReady = vkc.pd_pmflr_f.presentModeFifoLatestReady;

            JMN_CHECK(JMN_IMPLIES(e_vk12_f.timelineSemaphore, vkc.pd_vk12_f.timelineSemaphore), result, jmn::Result::ErrorNotSupported, ex0);
            JMN_CHECK(JMN_IMPLIES(e_vk13_f.synchronization2, vkc.pd_vk13_f.synchronization2), result, jmn::Result::ErrorNotSupported, ex0);
            JMN_CHECK(JMN_IMPLIES(e_vk13_f.shaderDemoteToHelperInvocation, vkc.pd_vk13_f.shaderDemoteToHelperInvocation), result, jmn::Result::ErrorNotSupported, ex0);
            JMN_CHECK(JMN_IMPLIES(e_vk13_f.dynamicRendering, vkc.pd_vk13_f.dynamicRendering), result, jmn::Result::ErrorNotSupported, ex0);
            JMN_CHECK(JMN_IMPLIES(e_vk14_f.maintenance5, vkc.pd_vk14_f.maintenance5), result, jmn::Result::ErrorNotSupported, ex0);
            JMN_CHECK(JMN_IMPLIES(e_sm1_f.swapchainMaintenance1, vkc.pd_sm1_f.swapchainMaintenance1), result, jmn::Result::ErrorNotSupported, ex0);

            ci.flags                   = 0;
            ci.queueCreateInfoCount    = (jmn::U32)dqcic;
            ci.pQueueCreateInfos       = dqcis;
            ci.enabledLayerCount       = 0;
            ci.ppEnabledLayerNames     = NULL;
            ci.enabledExtensionCount   = enabled_ext_name_count;
            ci.ppEnabledExtensionNames = enabled_ext_name_array;
            ci.pEnabledFeatures        = NULL;
            VK_CHECK(vkCreateDevice(vkc.pd, &ci, vkc.ac, &vkc.dev), result, ex0);
            volkLoadDevice(vkc.dev);
        }

        vkGetDeviceQueue(vkc.dev, vkc.g_qfi, vkc.g_qi, &vkc.g_q);
        vkGetDeviceQueue(vkc.dev, vkc.c_qfi, vkc.c_qi, &vkc.c_q);
        vkGetDeviceQueue(vkc.dev, vkc.t_qfi, vkc.tr_qi, &vkc.tr_q);
        vkGetDeviceQueue(vkc.dev, vkc.t_qfi, vkc.tw_qi, &vkc.tw_q);

        return true;
        //ex1:vkDestroyDevice(vkc.dev, vkc.ac);
    ex0:return false;
    }

    static void DestroyDevice(VulkanContext &vkc)
    {
        vkDestroyDevice(vkc.dev, vkc.ac);
    }

    static void DestroyPhysicalDevice(VulkanContext &vkc)
    {
        vkc.allocator.Free(vkc.pd_vk14_p.pCopyDstLayouts, vkc.pd_vk14_p.copyDstLayoutCount);
        vkc.allocator.Free(vkc.pd_vk14_p.pCopySrcLayouts, vkc.pd_vk14_p.copySrcLayoutCount);
        vkc.allocator.Free(vkc.pd_qfp_a, vkc.pd_qfp_c);
    }

    static void DestroyInstance(VulkanContext &vkc)
    {
    #ifdef DEBUG_MODE
        if (vkc.dbg_msgr != VK_NULL_HANDLE) vkDestroyDebugUtilsMessengerEXT(vkc.ins, vkc.dbg_msgr, vkc.ac);
    #endif
        vkDestroyInstance(vkc.ins, vkc.ac);
    }

    static void DestroyEnvironment(VulkanContext &vkc)
    {
        vkc.allocator.Free(vkc.ac);
        ::DeleteCriticalSection(&vkc.heap_cs);
        volkFinalize();
        JMN_ASSERT(vkc.heap.block_count == 1);
    }

}

jmn::B8 Create(VulkanContext &vkc, jmn::Allocator allocator, jmn::U32 api_version, PFN_vkDebugUtilsMessengerCallbackEXT dbg_callback, void *dbg_user_data, jmn::Result &result)
{
    if (!vkc.heap.Create(vkc.allocator = allocator, VulkanContext::HeapSize, jmn::MemoryHeap::DefaultMinBlockSize, result)) goto ex0;
    if (!VulkanContextInternal::CreateEnvironment   (vkc, result)) goto ex1;
    if (!VulkanContextInternal::CreateInstance      (vkc, api_version, dbg_callback, dbg_user_data, result)) goto ex2;
    if (!VulkanContextInternal::CreatePhysicalDevice(vkc, result)) goto ex3;
    if (!VulkanContextInternal::SelectQueueFamilies (vkc, result)) goto ex4;
    if (!VulkanContextInternal::CreateDevice        (vkc, result)) goto ex4;
    return true;
//ex5:VulkanContextInternal::DestroyDevice        (vkc);
ex4:VulkanContextInternal::DestroyPhysicalDevice(vkc);
ex3:VulkanContextInternal::DestroyInstance      (vkc);
ex2:VulkanContextInternal::DestroyEnvironment   (vkc);
ex1:vkc.heap.Destroy(allocator);
ex0:return false;
}

void Destroy(VulkanContext &vkc)
{
    VulkanContextInternal::DestroyDevice        (vkc);
    VulkanContextInternal::DestroyPhysicalDevice(vkc);
    VulkanContextInternal::DestroyInstance      (vkc);
    VulkanContextInternal::DestroyEnvironment   (vkc);
    vkc.heap.Destroy(vkc.allocator);
}

#  endif // VULKAN_CONTEXT_IMPLEMENTATED
#endif // VULKAN_CONTEXT_IMPLEMENTATION
