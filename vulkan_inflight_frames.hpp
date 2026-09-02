#ifndef VULKAN_INFLIGHT_FRAMES_INCLUDED
#  define VULKAN_INFLIGHT_FRAMES_INCLUDED

#  include <Hydrogen.hpp>
#  include "vulkan_context.hpp"

struct VulkanInflightFrames
{
    jmn::U32         index, count;
    VkCommandPool    cp;
    VkCommandBuffer *cb;
    VkSemaphore     *sc_imga;
    VkSemaphore     *sc_imgr;
    VkFence         *sc_imgp;
};

jmn::B8 Create(jmn::Allocator allocator, VulkanContext const &vkctx, jmn::U32 count, VulkanInflightFrames &vkif, jmn::Result &result);
void    Destroy(VulkanInflightFrames &vkif, jmn::Allocator allocator, VulkanContext const &vkctx);

#endif // VULKAN_INFLIGHT_FRAMES_INCLUDED

#ifdef __INTELLISENSE__
#  define VULKAN_INFLIGHT_FRAMES_IMPLEMENTATION
#endif // __INTELLISENSE__

#ifdef VULKAN_INFLIGHT_FRAMES_IMPLEMENTATION
#  ifndef VULKAN_INFLIGHT_FRAMES_IMPLEMENTATED
#    define VULKAN_INFLIGHT_FRAMES_IMPLEMENTATED

jmn::B8 Create(jmn::Allocator allocator, VulkanContext const &vkctx, jmn::U32 count, VulkanInflightFrames &vkif, jmn::Result &result)
{
    using namespace jmn;

    vkif.index = 0;
    vkif.count = count;

    if (!allocator.Alloc(vkif.count, vkif.cb, result)) goto ex0;
    if (!allocator.Alloc(vkif.count, vkif.sc_imga, result)) goto ex1;
    if (!allocator.Alloc(vkif.count, vkif.sc_imgr, result)) goto ex2;
    if (!allocator.Alloc(vkif.count, vkif.sc_imgp, result)) goto ex3;

    if (!Create(vkctx.ac, vkctx.dev, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT | VK_COMMAND_POOL_CREATE_TRANSIENT_BIT, vkctx.g_qfi, vkif.cp, result)) goto ex4;
    if (!Allocate(vkctx.dev, vkif.cp, VK_COMMAND_BUFFER_LEVEL_PRIMARY, vkif.count, vkif.cb, result)) goto ex5;

    U32 scimga_i, scimgr_i, scimgp_i;
    for (scimga_i = 0; scimga_i < vkif.count; ++scimga_i) if (!Create(vkctx.ac, vkctx.dev, vkif.sc_imga[scimga_i], result)) goto ex6;
    for (scimgr_i = 0; scimgr_i < vkif.count; ++scimgr_i) if (!Create(vkctx.ac, vkctx.dev, vkif.sc_imgr[scimgr_i], result)) goto ex7;
    for (scimgp_i = 0; scimgp_i < vkif.count; ++scimgp_i) if (!Create(vkctx.ac, vkctx.dev, VK_FENCE_CREATE_SIGNALED_BIT, vkif.sc_imgp[scimgp_i], result)) goto ex8;

    return true;
ex8:while (scimgp_i --> 0) vkDestroyFence    (vkctx.dev, vkif.sc_imgp[scimgp_i], vkctx.ac);
ex7:while (scimgr_i --> 0) vkDestroySemaphore(vkctx.dev, vkif.sc_imgr[scimgr_i], vkctx.ac);
ex6:while (scimga_i --> 0) vkDestroySemaphore(vkctx.dev, vkif.sc_imga[scimga_i], vkctx.ac);
ex5:vkDestroyCommandPool(vkctx.dev, vkif.cp, vkctx.ac);
ex4:allocator.Free(vkif.sc_imgp, vkif.count);
ex3:allocator.Free(vkif.sc_imgr, vkif.count);
ex2:allocator.Free(vkif.sc_imga, vkif.count);
ex1:allocator.Free(vkif.cb, vkif.count);
ex0:return false;
}

void Destroy(VulkanInflightFrames &vkif, jmn::Allocator allocator, VulkanContext const &vkctx)
{
    using namespace jmn;
    for (U32 i = vkif.count; i --> 0;) vkDestroyFence    (vkctx.dev, vkif.sc_imgp[i], vkctx.ac);
    for (U32 i = vkif.count; i --> 0;) vkDestroySemaphore(vkctx.dev, vkif.sc_imgr[i], vkctx.ac);
    for (U32 i = vkif.count; i --> 0;) vkDestroySemaphore(vkctx.dev, vkif.sc_imga[i], vkctx.ac);
    vkDestroyCommandPool(vkctx.dev, vkif.cp, vkctx.ac);
    allocator.Free(vkif.sc_imgp, vkif.count);
    allocator.Free(vkif.sc_imgr, vkif.count);
    allocator.Free(vkif.sc_imga, vkif.count);
    allocator.Free(vkif.cb, vkif.count);
}

#  endif // VULKAN_INFLIGHT_FRAMES_IMPLEMENTATED
#endif // VULKAN_INFLIGHT_FRAMES_IMPLEMENTATION
