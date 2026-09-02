#ifndef VULKAN_SWAP_CHAIN_INCLUDED
#  define VULKAN_SWAP_CHAIN_INCLUDED

#  include <Helium.hpp>

#  include "settings.hpp"
#  include "vulkan_context.hpp"

struct VulkanSwapChain
{
    static inline constexpr jmn::Size OutdatedBitIndex = 0;

    static inline constexpr VkFormat DepthFormat = VK_FORMAT_D24_UNORM_S8_UINT;

    using Flags = jmn::B64;

    enum FlagBits : Flags
    {
        OutdatedBit = (Flags)1 << OutdatedBitIndex,
    };

    Flags                         flags;
    HWND                          window;
    VkSurfaceKHR                  surf;
    VkSwapchainKHR                sc;
    jmn::U32                      mic;
    VkFormat                      fmt;
    VkColorSpaceKHR               cs;
    VkExtent2D                    ext;
    jmn::U32                      ial;
    jmn::U32                      pm_i;
    VkSurfaceTransformFlagBitsKHR pt;
    VkCompositeAlphaFlagBitsKHR   ca;
    jmn::U32                      spmc_c;
    VkPresentModeKHR             *spmc_a;
    jmn::U32                      img_i;
    jmn::U32                      img_c;
    VkImage                      *img_a;
    VkImageView                  *imgv_a;
    VkImageView                  *dps_imgv_a;
    VkImage                       dps_img;
    VkDeviceMemory                dps_mem;
};

jmn::B8 Create(jmn::Allocator allocator, HWND window, HINSTANCE instance, VulkanContext const &ctx, Settings const &settings, VulkanSwapChain &vksc, jmn::Result &result);
jmn::B8 Recreate(VulkanSwapChain &vksc, jmn::Allocator allocator, VulkanContext const &ctx, Settings const &settings, jmn::Result &result);
void    Destroy(VulkanSwapChain &vksc, jmn::Allocator allocator, VulkanContext const &ctx);
void    UpdatePresentMode(VulkanSwapChain &vksc, Settings const &settings);
jmn::B8 Acquire(VulkanSwapChain &vksc, VulkanContext const &ctx, VkSemaphore semaphore, jmn::Result &result);
jmn::B8 Present(VulkanSwapChain &vksc, VulkanContext const &ctx, jmn::U32 wait_semaphore_count, VkSemaphore const *wait_semaphore_array, VkFence fence, jmn::Result &result);

#endif // VULKAN_SWAP_CHAIN_INCLUDED

#ifdef __INTELLISENSE__
#  define VULKAN_SWAP_CHAIN_IMPLEMENTATION
#endif // __INTELLISENSE__

#ifdef VULKAN_SWAP_CHAIN_IMPLEMENTATION
#  ifndef VULKAN_SWAP_CHAIN_IMPLEMENTATED
#    define VULKAN_SWAP_CHAIN_IMPLEMENTATED

namespace VulkanSwapChainInternal
{

    static jmn::B8 CreateWin32Surface(VulkanContext const &ctx, HWND window, HINSTANCE instance, VkSurfaceKHR &surface, jmn::Result &result)
    {
        VkWin32SurfaceCreateInfoKHR ci;
        ci.sType     = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
        ci.pNext     = NULL;
        ci.flags     = 0;
        ci.hinstance = instance;
        ci.hwnd      = window;
        VK_CHECK(vkCreateWin32SurfaceKHR(ctx.ins, &ci, ctx.ac, &surface), result, ex0);
        return true;
    ex0:return false;
    }

    static jmn::B8 SelectSurfaceFormat(jmn::Allocator allocator, Settings const &settings, VkPhysicalDevice pd, VkPhysicalDeviceSurfaceInfo2KHR const *pdsi2, VkFormat &format, VkColorSpaceKHR &color_space, jmn::Result &result)
    {
        jmn::B8         success                        = false;
        jmn::U32        preferred_format_count         = 0;
        VkFormat        preferred_format_array[8]      = {};
        jmn::U32        preferred_color_space_count    = 0;
        VkColorSpaceKHR preferred_color_space_array[8] = {};

        if (settings.flags & Settings::HDRBit)
        {
            preferred_format_array[preferred_format_count++] = VK_FORMAT_R16G16B16A16_SFLOAT;
            preferred_format_array[preferred_format_count++] = VK_FORMAT_A2B10G10R10_UNORM_PACK32;
            preferred_format_array[preferred_format_count++] = VK_FORMAT_A2R10G10B10_UNORM_PACK32;

            preferred_color_space_array[preferred_color_space_count++] = VK_COLOR_SPACE_HDR10_ST2084_EXT;
            preferred_color_space_array[preferred_color_space_count++] = VK_COLOR_SPACE_EXTENDED_SRGB_LINEAR_EXT;
        }

        preferred_format_array[preferred_format_count++] = VK_FORMAT_B8G8R8A8_SRGB;
        preferred_format_array[preferred_format_count++] = VK_FORMAT_R8G8B8A8_SRGB;

        preferred_color_space_array[preferred_color_space_count++] = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;

        jmn::U32             supported_surface_format_count = 0;
        VkSurfaceFormat2KHR *supported_surface_format_array = NULL;
        VK_CHECK(vkGetPhysicalDeviceSurfaceFormats2KHR(pd, pdsi2, &supported_surface_format_count, supported_surface_format_array), result, ex0);

        if (!allocator.Alloc(supported_surface_format_count, supported_surface_format_array, result)) goto ex0;
        for (jmn::U32 i = 0; i < supported_surface_format_count; ++i)
        {
            supported_surface_format_array[i].sType = VK_STRUCTURE_TYPE_SURFACE_FORMAT_2_KHR;
            supported_surface_format_array[i].pNext = NULL;
        }

        VK_CHECK(vkGetPhysicalDeviceSurfaceFormats2KHR(pd, pdsi2, &supported_surface_format_count, supported_surface_format_array), result, ex1);

        jmn::B8 found = false;
        for (jmn::U32 i = 0; i < preferred_format_count; ++i)
        {
            auto &pf = preferred_format_array[i];
            for (jmn::U32 j = 0; j < preferred_color_space_count; ++j)
            {
                auto &pcs = preferred_color_space_array[j];
                for (jmn::U32 k = 0; k < supported_surface_format_count; ++k)
                {
                    auto &ssf = supported_surface_format_array[k].surfaceFormat;
                    if ((pf == ssf.format) && (pcs == ssf.colorSpace))
                    {
                        format      = pf;
                        color_space = pcs;
                        found = true;
                        break;
                    }
                }
                if (found) break;
            }
            if (found) break;
        }
        JMN_CHECK(found, result, jmn::Result::ErrorNotSupported, ex1);
        success = true;
    ex1:allocator.Free(supported_surface_format_array, supported_surface_format_count);
    ex0:return success;
    }

    static jmn::B8 SelectPresentMode(jmn::Allocator allocator, Settings const &settings, VkPhysicalDevice pd, VkPhysicalDeviceSurfaceInfo2KHR const *pdsi2, VkPresentModeKHR &present_mode, jmn::Result &result)
    {
        jmn::B8          success                         = false;
        jmn::U32         preferred_present_mode_count    = 0;
        VkPresentModeKHR preferred_present_mode_array[4] ={};

        if (!(settings.flags & Settings::VSyncBit))
        {
            preferred_present_mode_array[preferred_present_mode_count++] = VK_PRESENT_MODE_FIFO_LATEST_READY_KHR;
            preferred_present_mode_array[preferred_present_mode_count++] = VK_PRESENT_MODE_MAILBOX_KHR;
            preferred_present_mode_array[preferred_present_mode_count++] = VK_PRESENT_MODE_FIFO_RELAXED_KHR;
            preferred_present_mode_array[preferred_present_mode_count++] = VK_PRESENT_MODE_IMMEDIATE_KHR;
        }
        preferred_present_mode_array[preferred_present_mode_count++] = VK_PRESENT_MODE_FIFO_KHR;

        jmn::U32          supported_present_mode_count = 0;
        VkPresentModeKHR *supported_present_mode_array = NULL;
        VK_CHECK(vkGetPhysicalDeviceSurfacePresentModes2EXT(pd, pdsi2, &supported_present_mode_count, supported_present_mode_array), result, ex0);
        if (!allocator.Alloc(supported_present_mode_count, supported_present_mode_array, result)) goto ex0;
        VK_CHECK(vkGetPhysicalDeviceSurfacePresentModes2EXT(pd, pdsi2, &supported_present_mode_count, supported_present_mode_array), result, ex1);

        jmn::B8 found = false;
        for (jmn::U32 i = 0; i < preferred_present_mode_count; ++i)
        {
            for (jmn::U32 j = 0; j < supported_present_mode_count; ++j)
            {
                if (preferred_present_mode_array[i] == supported_present_mode_array[j])
                {
                    present_mode = preferred_present_mode_array[i];
                    found = true;
                    break;
                }
            }
            if (found) break;
        }
        JMN_CHECK(found, result, jmn::Result::ErrorNotSupported, ex1);
        success = true;
    ex1:allocator.Free(supported_present_mode_array, supported_present_mode_count);
    ex0:return success;
    }

    static jmn::U32 SelectPresentMode(Settings const &settings, VkPresentModeKHR const *present_mode_array, jmn::U32 present_mode_count)
    {
        if (settings.flags & Settings::VSyncBit)
        {
            for (jmn::U32 i = 0; i < present_mode_count; ++i) if (present_mode_array[i] == VK_PRESENT_MODE_FIFO_KHR) return i;
        }
        else
        {
            for (jmn::U32 i = 0; i < present_mode_count; ++i) if (present_mode_array[i] == VK_PRESENT_MODE_FIFO_LATEST_READY_KHR) return i;
            for (jmn::U32 i = 0; i < present_mode_count; ++i) if (present_mode_array[i] == VK_PRESENT_MODE_MAILBOX_KHR)           return i;
            for (jmn::U32 i = 0; i < present_mode_count; ++i) if (present_mode_array[i] == VK_PRESENT_MODE_FIFO_RELAXED_KHR)      return i;
            for (jmn::U32 i = 0; i < present_mode_count; ++i) if (present_mode_array[i] == VK_PRESENT_MODE_IMMEDIATE_KHR)         return i;
        }
        return 0;
    }

    static jmn::B8 SetAndClampExtent(VkSurfaceCapabilitiesKHR const &sc, HWND window, VkExtent2D &extent, jmn::Result &result)
    {
        if ((sc.currentExtent.width == UINT32_MAX) || (sc.currentExtent.height == UINT32_MAX))
        {
            RECT rect;
            JMN_CHECK(::GetClientRect(window, &rect), result, jmn::Result::ErrorGeneric, ex0);
            extent ={ (jmn::U32)(rect.right - rect.left), (jmn::U32)(rect.bottom - rect.top) };
        }
        else extent = sc.currentExtent;

        extent.width  = JMN_CLAMP(extent.width, sc.minImageExtent.width, sc.maxImageExtent.width);
        extent.height = JMN_CLAMP(extent.height, sc.minImageExtent.height, sc.maxImageExtent.height);

        return true;
    ex0:return false;
    }

    static jmn::B8 CreateImages(VulkanSwapChain &vksc, jmn::Allocator allocator, VulkanContext const &ctx, jmn::Result &result)
    {
        jmn::U32 const old_img_c = vksc.img_c;
        vksc.img_c = 0;
        VK_CHECK(vkGetSwapchainImagesKHR(ctx.dev, vksc.sc, &vksc.img_c, NULL), result, ex0);
        if (!allocator.Realloc(vksc.img_a, old_img_c, vksc.img_c, vksc.img_a, result)) goto ex0;
        if (!allocator.Realloc(vksc.imgv_a, old_img_c, vksc.img_c, vksc.imgv_a, result)) goto ex1;
        if (!allocator.Realloc(vksc.dps_imgv_a, old_img_c, vksc.img_c, vksc.dps_imgv_a, result)) goto ex2;
        VK_CHECK(vkGetSwapchainImagesKHR(ctx.dev, vksc.sc, &vksc.img_c, vksc.img_a), result, ex3);
        return true;
    ex3:allocator.Free(vksc.dps_imgv_a, vksc.img_c);
    ex2:allocator.Free(vksc.imgv_a, vksc.img_c);
    ex1:allocator.Free(vksc.img_a, vksc.img_c);
    ex0:return false;
    }

    static jmn::B8 CreateImageViews(VulkanSwapChain &vksc, VulkanContext const &ctx, jmn::Result &result)
    {
        VkImageViewCreateInfo imgv_ci;
        imgv_ci.sType            = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        imgv_ci.pNext            = NULL;
        imgv_ci.flags            = 0;
        imgv_ci.image            = VK_NULL_HANDLE;
        imgv_ci.viewType         = VK_IMAGE_VIEW_TYPE_2D;
        imgv_ci.format           = vksc.fmt;
        imgv_ci.components       ={ VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY };
        imgv_ci.subresourceRange ={ VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };

        jmn::U32 imgv_i;
        for (imgv_i = 0; imgv_i < vksc.img_c; ++imgv_i)
        {
            imgv_ci.image = vksc.img_a[imgv_i];
            VK_CHECK(vkCreateImageView(ctx.dev, &imgv_ci, ctx.ac, vksc.imgv_a + imgv_i), result, ex0);
        }

        return true;
    ex0:while (imgv_i --> 0) vkDestroyImageView(ctx.dev, vksc.imgv_a[imgv_i], ctx.ac);
        return false;
    }

    static jmn::B8 CreateDepthImage(VulkanSwapChain &vksc, VulkanContext const &ctx, jmn::Result &result)
    {
        {
            VkImageFormatListCreateInfo imfl_ci;
            VkImageCreateInfo           img_ci;

            img_ci .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
            imfl_ci.sType = VK_STRUCTURE_TYPE_IMAGE_FORMAT_LIST_CREATE_INFO;

            img_ci .pNext = &imfl_ci;
            imfl_ci.pNext = NULL;

            img_ci.flags                 = 0;
            img_ci.imageType             = VK_IMAGE_TYPE_2D;
            img_ci.format                = VulkanSwapChain::DepthFormat;
            img_ci.extent                = { vksc.ext.width, vksc.ext.height, 1 };
            img_ci.mipLevels             = 1;
            img_ci.arrayLayers           = vksc.img_c;
            img_ci.samples               = VK_SAMPLE_COUNT_1_BIT;
            img_ci.tiling                = VK_IMAGE_TILING_OPTIMAL;
            img_ci.usage                 = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
            img_ci.sharingMode           = VK_SHARING_MODE_EXCLUSIVE;
            img_ci.queueFamilyIndexCount = 0;
            img_ci.pQueueFamilyIndices   = NULL;
            img_ci.initialLayout         = VK_IMAGE_LAYOUT_UNDEFINED;

            imfl_ci.viewFormatCount = 1;
            imfl_ci.pViewFormats    = &VulkanSwapChain::DepthFormat;

            VK_CHECK(vkCreateImage(ctx.dev, &img_ci, ctx.ac, &vksc.dps_img), result, ex0);
        }
        {
            VkImageMemoryRequirementsInfo2 imri2;
            imri2.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_REQUIREMENTS_INFO_2;
            imri2.pNext = NULL;
            imri2.image = vksc.dps_img;

            VkMemoryDedicatedRequirements mdr;
            VkMemoryRequirements2         mr2;

            mr2.sType = VK_STRUCTURE_TYPE_MEMORY_REQUIREMENTS_2;
            mdr.sType = VK_STRUCTURE_TYPE_MEMORY_DEDICATED_REQUIREMENTS;

            mr2.pNext = &mdr;
            mdr.pNext = NULL;

            vkGetImageMemoryRequirements2(ctx.dev, &imri2, &mr2);

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
            if (!SelectMemoryTypeIndex(ctx.pd_mp2.memoryProperties, mr2.memoryRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, mai.memoryTypeIndex, result)) goto ex1;

            mafi.flags      = 0;
            mafi.deviceMask = 0;

            mdai.image  = vksc.dps_img;
            mdai.buffer = VK_NULL_HANDLE;

            VK_CHECK(vkAllocateMemory(ctx.dev, &mai, ctx.ac, &vksc.dps_mem), result, ex1);
        }
        VK_CHECK(vkBindImageMemory(ctx.dev, vksc.dps_img, vksc.dps_mem, 0), result, ex2);

        VkImageViewCreateInfo imgv_ci;
        imgv_ci.sType            = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        imgv_ci.pNext            = NULL;
        imgv_ci.flags            = 0;
        imgv_ci.image            = vksc.dps_img;
        imgv_ci.viewType         = VK_IMAGE_VIEW_TYPE_2D;
        imgv_ci.format           = VulkanSwapChain::DepthFormat;
        imgv_ci.components       ={ VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY };
        imgv_ci.subresourceRange ={ VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT, 0, 1, 0, 1 };

        jmn::U32 imgv_i;
        for (imgv_i = 0; imgv_i < vksc.img_c; ++imgv_i)
        {
            imgv_ci.subresourceRange.baseArrayLayer = imgv_i;
            VK_CHECK(vkCreateImageView(ctx.dev, &imgv_ci, ctx.ac, vksc.dps_imgv_a + imgv_i), result, ex3);
        }

        return true;
    ex3:while (imgv_i --> 0) vkDestroyImageView(ctx.dev, vksc.dps_imgv_a[imgv_i], ctx.ac);
    ex2:vkFreeMemory(ctx.dev, vksc.dps_mem, ctx.ac);
    ex1:vkDestroyImage(ctx.dev, vksc.dps_img, ctx.ac);
    ex0:return false;
    }

    static void DestroyDepthImage(VulkanSwapChain &vksc, VulkanContext const &ctx)
    {
        for (jmn::U32 i = vksc.img_c; i --> 0;) vkDestroyImageView(ctx.dev, vksc.dps_imgv_a[i], ctx.ac);
        vkFreeMemory(ctx.dev, vksc.dps_mem, ctx.ac);
        vkDestroyImage(ctx.dev, vksc.dps_img, ctx.ac);
    }

    static void DestroyImageViews(VulkanSwapChain &vksc, VulkanContext const &ctx)
    {
        for (jmn::U32 i = vksc.img_c; i --> 0;) vkDestroyImageView(ctx.dev, vksc.imgv_a[i], ctx.ac);
    }

    static void DestroyImages(VulkanSwapChain &vksc, jmn::Allocator allocator)
    {
        allocator.Free(vksc.dps_imgv_a, vksc.img_c);
        allocator.Free(vksc.imgv_a, vksc.img_c);
        allocator.Free(vksc.img_a, vksc.img_c);
    }

    static void Reset(VulkanSwapChain &vksc, VulkanContext const &ctx)
    {
        DestroyDepthImage(vksc, ctx);
        DestroyImageViews(vksc, ctx);
        vksc.ext   ={ UINT32_MAX, UINT32_MAX };
        vksc.fmt   = VK_FORMAT_UNDEFINED;
        vksc.cs    = VK_COLOR_SPACE_MAX_ENUM_KHR;
        vksc.pm_i  = UINT32_MAX;
        vksc.img_i = 0;
        jmn::Zero(vksc.imgv_a, vksc.img_c);
        jmn::Zero(vksc.img_a, vksc.img_c);
    }

    static jmn::B8 SelectProperties(VulkanSwapChain &vksc, jmn::Allocator allocator, VulkanContext const &ctx, Settings const &settings, VkPhysicalDeviceSurfaceInfo2KHR const *pdsi2,
        VkPresentModeKHR &pm, jmn::B8 &fse_supported, VkPresentScalingFlagsKHR &scaling_flags, VkPresentGravityFlagsKHR &gravity_x_flags, VkPresentGravityFlagsKHR &gravity_y_flags, jmn::Result &result)
    {
        jmn::B8                                     success = false;
        VkSurfaceCapabilitiesFullScreenExclusiveEXT scfse   ={};
        VkSurfacePresentScalingCapabilitiesKHR      spsc    ={};
        VkSurfacePresentModeCompatibilityKHR        spmc    ={};
        VkSurfaceCapabilities2KHR                   sc2     ={};
        auto &sc = sc2.surfaceCapabilities;
        {
            sc2  .sType = VK_STRUCTURE_TYPE_SURFACE_CAPABILITIES_2_KHR;
            spmc .sType = VK_STRUCTURE_TYPE_SURFACE_PRESENT_MODE_COMPATIBILITY_KHR;
            spsc .sType = VK_STRUCTURE_TYPE_SURFACE_PRESENT_SCALING_CAPABILITIES_KHR;
            scfse.sType = VK_STRUCTURE_TYPE_SURFACE_CAPABILITIES_FULL_SCREEN_EXCLUSIVE_EXT;

            sc2  .pNext = &spmc;
            spmc .pNext = &spsc;
            spsc .pNext = &scfse;
            scfse.pNext = NULL;
        }

        if (!VulkanSwapChainInternal::SelectPresentMode(allocator, settings, ctx.pd, pdsi2, pm, result)) goto ex0;

        VK_CHECK(vkGetPhysicalDeviceSurfaceCapabilities2KHR(ctx.pd, pdsi2, &sc2), result, ex0);
        if (!allocator.Alloc(spmc.presentModeCount, spmc.pPresentModes, result)) goto ex0;
        VK_CHECK(vkGetPhysicalDeviceSurfaceCapabilities2KHR(ctx.pd, pdsi2, &sc2), result, ex1);

        vksc.mic = 3;
        vksc.mic = sc.maxImageCount ?
            JMN_CLAMP(vksc.mic, sc.minImageCount, sc.maxImageCount) :
            JMN_MAX(vksc.mic, sc.minImageCount);

        if (!VulkanSwapChainInternal::SelectSurfaceFormat(allocator, settings, ctx.pd, pdsi2, vksc.fmt, vksc.cs, result)) goto ex1;
        if (!VulkanSwapChainInternal::SetAndClampExtent(sc, vksc.window, vksc.ext, result)) goto ex1;

        vksc.ial = 1;
        vksc.ial = JMN_MIN(vksc.ial, sc.maxImageArrayLayers);

        vksc.pt = sc.currentTransform;
        JMN_CHECK(vksc.pt & sc.supportedTransforms, result, jmn::Result::ErrorNotSupported, ex1);

        vksc.ca = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        JMN_CHECK(vksc.ca & sc.supportedCompositeAlpha, result, jmn::Result::ErrorNotSupported, ex1);

        if (!allocator.Realloc(vksc.spmc_a, vksc.spmc_c, spmc.presentModeCount, vksc.spmc_a, result)) goto ex1;
        vksc.spmc_c = spmc.presentModeCount;
        jmn::Copy(vksc.spmc_a, spmc.pPresentModes, vksc.spmc_c);

        vksc.pm_i = UINT32_MAX;
        for (jmn::U32 i = 0; i < vksc.spmc_c; ++i)
        {
            if (vksc.spmc_a[i] == pm)
            {
                vksc.pm_i = i;
                break;
            }
        }
        JMN_CHECK(vksc.pm_i != UINT32_MAX, result, jmn::Result::ErrorInternal, ex1);
        fse_supported   = scfse.fullScreenExclusiveSupported == VK_TRUE;
        scaling_flags   = spsc.supportedPresentScaling;
        gravity_x_flags = spsc.supportedPresentGravityX;
        gravity_y_flags = spsc.supportedPresentGravityY;
        success = true;
    ex1:allocator.Free(spmc.pPresentModes, spmc.presentModeCount);
    ex0:return success;
    }

}

jmn::B8 Create(jmn::Allocator allocator, HWND window, HINSTANCE instance, VulkanContext const &ctx, Settings const &settings, VulkanSwapChain &vksc, jmn::Result &result)
{
    if (!VulkanSwapChainInternal::CreateWin32Surface(ctx, vksc.window = window, instance, vksc.surf, result)) goto ex0;
    if (!Recreate(vksc, allocator, ctx, settings, result)) goto ex1;
    return true;
//ex2:Destroy(vksc, ctx);
ex1:vkDestroySurfaceKHR(ctx.ins, vksc.surf, ctx.ac);
ex0:return false;
}

jmn::B8 Recreate(VulkanSwapChain &vksc, jmn::Allocator allocator, VulkanContext const &ctx, Settings const &settings, jmn::Result &result)
{
    VulkanSwapChainInternal::Reset(vksc, ctx);

    VkSurfaceFullScreenExclusiveWin32InfoEXT sfsew32i ={};
    VkSurfaceFullScreenExclusiveInfoEXT      sfsei    ={};
    VkSurfacePresentModeKHR                  spm      ={};
    VkPhysicalDeviceSurfaceInfo2KHR          pdsi2    ={};
    {
        pdsi2   .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SURFACE_INFO_2_KHR;
        spm     .sType = VK_STRUCTURE_TYPE_SURFACE_PRESENT_MODE_KHR;
        sfsei   .sType = VK_STRUCTURE_TYPE_SURFACE_FULL_SCREEN_EXCLUSIVE_INFO_EXT;
        sfsew32i.sType = VK_STRUCTURE_TYPE_SURFACE_FULL_SCREEN_EXCLUSIVE_WIN32_INFO_EXT;

        pdsi2   .pNext = &spm;
        spm     .pNext = &sfsei;
        sfsei   .pNext = &sfsew32i;
        sfsew32i.pNext = NULL;

        pdsi2   .surface             = vksc.surf;
        sfsei   .fullScreenExclusive = VK_FULL_SCREEN_EXCLUSIVE_ALLOWED_EXT;
        sfsew32i.hmonitor            = MonitorFromWindow(vksc.window, MONITOR_DEFAULTTONEAREST);
    }

    jmn::B8                  fse_supported;
    VkPresentScalingFlagsKHR scaling_flags;
    VkPresentGravityFlagsKHR gravity_x_flags, gravity_y_flags;
    if (!VulkanSwapChainInternal::SelectProperties(vksc, allocator, ctx, settings, &pdsi2,
        spm.presentMode, fse_supported, scaling_flags, gravity_x_flags, gravity_y_flags, result)) goto ex0;

    VkSwapchainPresentScalingCreateInfoKHR sps_ci;
    VkSwapchainPresentModesCreateInfoKHR   spm_ci;
    VkSwapchainCreateInfoKHR               sc_ci;
    {
        sc_ci .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        spm_ci.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_PRESENT_MODES_CREATE_INFO_KHR;
        sps_ci.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_PRESENT_SCALING_CREATE_INFO_KHR;

        sc_ci .pNext = &spm_ci;
        spm_ci.pNext = &sps_ci;
        sps_ci.pNext = fse_supported ? &sfsei : NULL;

        sc_ci.flags                 = 0;
        sc_ci.surface               = vksc.surf;
        sc_ci.minImageCount         = vksc.mic;
        sc_ci.imageFormat           = vksc.fmt;
        sc_ci.imageColorSpace       = vksc.cs;
        sc_ci.imageExtent           = vksc.ext;
        sc_ci.imageArrayLayers      = vksc.ial;
        sc_ci.imageUsage            = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        sc_ci.imageSharingMode      = VK_SHARING_MODE_EXCLUSIVE;
        sc_ci.queueFamilyIndexCount = 0;
        sc_ci.pQueueFamilyIndices   = NULL;
        sc_ci.preTransform          = vksc.pt;
        sc_ci.compositeAlpha        = vksc.ca;
        sc_ci.presentMode           = vksc.spmc_a[vksc.pm_i];
        sc_ci.clipped               = VK_TRUE;
        sc_ci.oldSwapchain          = vksc.sc;

        spm_ci.presentModeCount = vksc.spmc_c;
        spm_ci.pPresentModes    = vksc.spmc_a;

        sps_ci.scalingBehavior = VK_PRESENT_SCALING_STRETCH_BIT_KHR;
        sps_ci.presentGravityX = VK_PRESENT_GRAVITY_CENTERED_BIT_KHR;
        sps_ci.presentGravityY = VK_PRESENT_GRAVITY_CENTERED_BIT_KHR;
    }

    JMN_CHECK((sps_ci.scalingBehavior & scaling_flags) == sps_ci.scalingBehavior, result, jmn::Result::ErrorNotSupported, ex0);
    JMN_CHECK((sps_ci.presentGravityX & gravity_x_flags) == sps_ci.presentGravityX, result, jmn::Result::ErrorNotSupported, ex0);
    JMN_CHECK((sps_ci.presentGravityY & gravity_y_flags) == sps_ci.presentGravityY, result, jmn::Result::ErrorNotSupported, ex0);

    VK_CHECK(vkCreateSwapchainKHR(ctx.dev, &sc_ci, ctx.ac, &vksc.sc), result, ex0);
    if (sc_ci.oldSwapchain)
    {
        vkDestroySwapchainKHR(ctx.dev, sc_ci.oldSwapchain, ctx.ac);
    }

    if (!VulkanSwapChainInternal::CreateImages(vksc, allocator, ctx, result)) goto ex1;
    if (!VulkanSwapChainInternal::CreateImageViews(vksc, ctx, result)) goto ex2;
    if (!VulkanSwapChainInternal::CreateDepthImage(vksc, ctx, result)) goto ex3;

    vksc.flags &=~ VulkanSwapChain::OutdatedBit;
    return true;
ex3:VulkanSwapChainInternal::DestroyImageViews(vksc, ctx);
ex2:VulkanSwapChainInternal::DestroyImages(vksc, allocator);
ex1:vkDestroySwapchainKHR(ctx.dev, vksc.sc, ctx.ac);
ex0:return false;
}

void Destroy(VulkanSwapChain &vksc, jmn::Allocator allocator, VulkanContext const &ctx)
{
    VulkanSwapChainInternal::Reset(vksc, ctx);
    VulkanSwapChainInternal::DestroyImages(vksc, allocator);
    vkDestroySwapchainKHR(ctx.dev, vksc.sc, ctx.ac);
    allocator.Free(vksc.spmc_a, vksc.spmc_c);
    vkDestroySurfaceKHR(ctx.ins, vksc.surf, ctx.ac);
}

void UpdatePresentMode(VulkanSwapChain &vksc, Settings const &settings)
{
    vksc.pm_i = VulkanSwapChainInternal::SelectPresentMode(settings, vksc.spmc_a, vksc.spmc_c);
}

jmn::B8 Acquire(VulkanSwapChain &vksc, VulkanContext const &ctx, VkSemaphore semaphore, jmn::Result &result)
{
    switch (auto vkresult = vkAcquireNextImageKHR(ctx.dev, vksc.sc, UINT64_MAX, semaphore, VK_NULL_HANDLE, &vksc.img_i))
    {
        case VK_SUCCESS: break;
        case VK_SUBOPTIMAL_KHR: case VK_ERROR_OUT_OF_DATE_KHR:
        {
            vksc.flags |= VulkanSwapChain::OutdatedBit;
        } break;
        default: VK_CHECK(vkresult, result, ex0);
    }
    return true;
ex0:return false;
}

jmn::B8 Present(VulkanSwapChain &vksc, VulkanContext const &ctx, jmn::U32 wait_semaphore_count, VkSemaphore const *wait_semaphore_array, VkFence fence, jmn::Result &result)
{
    VkSwapchainPresentModeInfoKHR  spmi;
    VkSwapchainPresentFenceInfoKHR spfi;
    VkPresentInfoKHR               pi  ;
    {
        pi  .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        spfi.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_PRESENT_FENCE_INFO_KHR;
        spmi.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_PRESENT_MODE_INFO_KHR;

        pi  .pNext = &spfi;
        spfi.pNext = &spmi;
        spmi.pNext = NULL;

        pi.waitSemaphoreCount = wait_semaphore_count;
        pi.pWaitSemaphores    = wait_semaphore_array;
        pi.swapchainCount     = 1;
        pi.pSwapchains        = &vksc.sc;
        pi.pImageIndices      = &vksc.img_i;
        pi.pResults           = NULL;

        spfi.swapchainCount = 1;
        spfi.pFences        = &fence;

        spmi.swapchainCount = 1;
        spmi.pPresentModes  = vksc.spmc_a + vksc.pm_i;
    }
    switch (auto vkresult = vkQueuePresentKHR(ctx.g_q, &pi))
    {
        case VK_SUCCESS: break;
        case VK_SUBOPTIMAL_KHR: case VK_ERROR_OUT_OF_DATE_KHR:
        {
            vksc.flags |= VulkanSwapChain::OutdatedBit;
        } break;
        default: VK_CHECK(vkresult, result, ex0);
    }
    return true;
ex0:return false;
}

#  endif // VULKAN_SWAP_CHAIN_IMPLEMENTATED
#endif // VULKAN_SWAP_CHAIN_IMPLEMENTATION
