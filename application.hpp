#ifndef APPLICATION_INCLUDED
#  define APPLICATION_INCLUDED

#  include <Windows.h>
#  include <Hydrogen.hpp>
#  include <imgui.h>
#  include <backends/imgui_impl_win32.h>
#  include <backends/imgui_impl_vulkan.h>
#  include "settings.hpp"
#  include "vulkan_context.hpp"
#  include "vulkan_swap_chain.hpp"
#  include "vulkan_inflight_frames.hpp"
#  include "audio_capture.hpp"

struct Vertex
{
    jmn::V2S16 p;
    jmn::U16   z;
    jmn::U16   s;
    jmn::V4U8  c;
};

struct Application
{
    static inline jmn::Size constexpr RunningBitIndex = 0;

    using Flags = jmn::B64;

    enum FlagBits : Flags
    {
        RunningBit = (Flags)1 << RunningBitIndex,
    };

    HINSTANCE            hInstance;
    jmn::MemoryArena     arena;
    jmn::MemoryHeap      heap;
    Flags                flags;
    Settings             settings;
    ATOM                 window_class; jmn::U8 _pad0[2];
    WINDOWPLACEMENT      main_window_placement;
    HWND                 main_window;
    VulkanContext        vkctx;
    VulkanSwapChain      vksc;
    VulkanInflightFrames vkif;
    AudioCapture         ac;

    VkPipelineLayout     draw_pll;
    VkPipeline           draw_pl;
    VkBuffer             draw_buffer;
    VkDeviceMemory       draw_memory;
    VkBuffer             copy_buffer;
    VkDeviceMemory       copy_memory;
    jmn::Addr            copy_addr;
};

jmn::B8     Create (HINSTANCE hInstance, Application &app, jmn::Result &result);
jmn::Result Run    (Application &app);
void        Destroy(Application &app);

#endif // APPLICATION_INCLUDED

#ifdef __INTELLISENSE__
#  define APPLICATION_IMPLEMENTATION
#endif // __INTELLISENSE__

#ifdef APPLICATION_IMPLEMENTATION
#  ifndef APPLICATION_IMPLEMENTATED
#    define APPLICATION_IMPLEMENTATED

#    include <stb_sprintf.h>
#    include "constants.hpp"
#    include "draw.vert.spv"
#    include "draw.frag.spv"

namespace ApplicationInternal
{

    static char *PrintConsoleSTBSPCallback(char const *buffer, void *user_data, int length)
    {
        DWORD characters_written, characters_to_write = (DWORD)length;
        JMN_ASSERT(WriteConsoleA((HANDLE)user_data, buffer, characters_to_write, &characters_written, NULL));
        return (char *)buffer;
    }

    static void PrintConsole(HANDLE output, jmn::C8 const *format, ...)
    {
        char buffer[STB_SPRINTF_MIN];
        va_list ap;
        va_start(ap, format);
        stbsp_vsprintfcb(PrintConsoleSTBSPCallback, output, buffer, format, ap);
        va_end(ap);
    }

    static LRESULT WindowEventCallback(HWND window, UINT message_id, WPARAM wParam, LPARAM lParam)
    {
        using namespace jmn;

        if (message_id == WM_CREATE)
        {
            auto const ci = (LPCREATESTRUCT)lParam;
            SetWindowLongPtr(window, GWLP_USERDATA, (LONG_PTR)ci->lpCreateParams);
            return 0;
        }

        auto &app = *(Application *)GetWindowLongPtr(window, GWLP_USERDATA);
        switch (message_id)
        {
            case WM_CLOSE: PostQuitMessage((int)Result::Success); return 0;
            case WM_KEYDOWN: switch (wParam)
            {
                case VK_F11:
                {
                    if (BitTestAndComplement64((LONG64 *)&app.settings.flags, Settings::FullScreenBitIndex))
                    {
                        SetWindowLong(app.main_window, GWL_STYLE, WS_OVERLAPPEDWINDOW | WS_VISIBLE);
                        JMN_ASSERT(SetWindowPlacement(app.main_window, &app.main_window_placement));
                        JMN_ASSERT(SetWindowPos(app.main_window, HWND_TOP, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOZORDER | SWP_NOMOVE | SWP_NOSIZE));
                    }
                    else
                    {
                        auto const monitor = MonitorFromWindow(app.main_window, MONITOR_DEFAULTTONEAREST);
                        MONITORINFO monitor_info;
                        monitor_info.cbSize = sizeof(monitor_info);
                        JMN_ASSERT(GetMonitorInfo(monitor, &monitor_info));

                        JMN_ASSERT(GetWindowPlacement(app.main_window, &app.main_window_placement));

                        SetWindowLong(app.main_window, GWL_STYLE, WS_POPUP | WS_VISIBLE);
                        JMN_ASSERT(SetWindowPos(app.main_window, HWND_TOP,
                            monitor_info.rcMonitor.left, monitor_info.rcMonitor.top,
                            monitor_info.rcMonitor.right - monitor_info.rcMonitor.left, monitor_info.rcMonitor.bottom - monitor_info.rcMonitor.top,
                            SWP_FRAMECHANGED));
                    }
                } return 0;
            }
        }
        return DefWindowProc(window, message_id, wParam, lParam);
    }

    static VkBool32 VKAPI_CALL VulkanDebugUtilsMessengerCallback(VkDebugUtilsMessageSeverityFlagBitsEXT severity, VkDebugUtilsMessageTypeFlagsEXT, VkDebugUtilsMessengerCallbackDataEXT const *callback_data, void *)
    {
        HANDLE output = NULL;
        switch (severity)
        {
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT: case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
            {
                output = GetStdHandle(STD_ERROR_HANDLE);
            } break;

            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT: case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
            {
                output = GetStdHandle(STD_OUTPUT_HANDLE);
            } break;

            default: output = NULL; break;
        }

        if (output && output != INVALID_HANDLE_VALUE)
        {
            PrintConsole(output, "%s\n", callback_data->pMessage);
        }

        return VK_TRUE;
    }

    static void *ImGuiAllocCallback(size_t size, void *user_data)
    {
        auto const app = (Application *)user_data;
        return (void *)app->heap.Alloc((jmn::Size)size);
    }

    static void ImGuiFreeCallback(void *ptr, void *user_data)
    {
        auto const app = (Application *)user_data;
        app->heap.Free((jmn::Addr)ptr);
    }

    static int ImGuiCreateVkSurfaceCallback(ImGuiViewport* vp, ImU64 vk_inst, const void* vk_allocators, ImU64* out_vk_surface)
    {
        VkWin32SurfaceCreateInfoKHR ci;
        ci.sType     = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
        ci.pNext     = NULL;
        ci.flags     = 0;
        ci.hinstance = ::GetModuleHandle(NULL);
        ci.hwnd      = (HWND)vp->PlatformHandleRaw;
        return (int)vkCreateWin32SurfaceKHR((VkInstance)vk_inst, &ci, (VkAllocationCallbacks *)vk_allocators, (VkSurfaceKHR *)out_vk_surface);
    }

    static void SetImGuiMainPipelineInfo(Application &app, ImGui_ImplVulkan_PipelineInfo &info)
    {
        info.RenderPass                  = VK_NULL_HANDLE;
        info.Subpass                     = UINT32_MAX;
        info.MSAASamples                 = VK_SAMPLE_COUNT_1_BIT;
        info.ExtraDynamicStates          ={};
        info.PipelineRenderingCreateInfo.sType                   = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO_KHR; // Valid if .sType == VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO_KHR
        info.PipelineRenderingCreateInfo.pNext                   = NULL;
        info.PipelineRenderingCreateInfo.viewMask                = 0;
        info.PipelineRenderingCreateInfo.colorAttachmentCount    = 1;
        info.PipelineRenderingCreateInfo.pColorAttachmentFormats = &app.vksc.fmt;
        info.PipelineRenderingCreateInfo.depthAttachmentFormat   = VulkanSwapChain::DepthFormat;
        info.PipelineRenderingCreateInfo.stencilAttachmentFormat = VulkanSwapChain::DepthFormat;
        info.SwapChainImageUsage         = 0;
    }

    static void InitializeMemory(Application &app)
    {
        using namespace jmn;

        auto addr = (Addr)&app;
        addr += sizeof(Application);
        app.arena.Initialize(addr, ApplicationArenaSize);
        addr += ApplicationArenaSize;
        app.heap.Create(addr, ApplicationHeapSize);
        addr += ApplicationHeapSize;
    }

    static jmn::B8 CreateEnvironment(Application &app, jmn::Result &result)
    {
        using namespace jmn;

        if (app.settings.flags & Settings::DebugBit) JMN_CHECK(AllocConsole(), result, Result::ErrorGeneric, ex0);

        return true;
    ex0:return false;
    }

    static void DestroyEnvironment(Application &app)
    {
        if (app.settings.flags & Settings::DebugBit) JMN_ASSERT(FreeConsole());
    }

    static jmn::B8 CreateWindowClass(HINSTANCE hInstance, ATOM &atom, jmn::Result &result)
    {
        using namespace jmn;

        WNDCLASSEX class_def;
        class_def.cbSize        = sizeof(class_def);
        class_def.style         = 0;
        class_def.lpfnWndProc   = WindowEventCallback;
        class_def.cbClsExtra    = 0;
        class_def.cbWndExtra    = 0;
        class_def.hInstance     = hInstance;
        class_def.hIcon         = (HICON)::LoadImage(NULL, IDI_APPLICATION, IMAGE_ICON, 0, 0, LR_DEFAULTSIZE | LR_SHARED);
        class_def.hCursor       = (HCURSOR)::LoadImage(NULL, IDC_ARROW, IMAGE_CURSOR, 0, 0, LR_DEFAULTSIZE | LR_SHARED);
        class_def.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
        class_def.lpszMenuName  = NULL;
        class_def.lpszClassName = (LPCWSTR)ApplicationWindowClassName.string;
        class_def.hIconSm       = NULL;
        JMN_CHECK(atom = ::RegisterClassEx(&class_def), result, Result::ErrorGeneric, ex0);
        return true;
    ex0:return false;
    }

    static void DestroyWindowClass(ATOM atom, HINSTANCE hInstance)
    {
        JMN_ASSERT(UnregisterClass(MAKEINTATOM(atom), hInstance));
    }

    static jmn::B8 CreateMainWindow(Application &app, jmn::Result &result)
    {
        using namespace jmn;

        if (!CreateWindowClass(app.hInstance, app.window_class, result)) goto ex0;

        auto const monitor = MonitorFromWindow(GetDesktopWindow(), MONITOR_DEFAULTTOPRIMARY);
        JMN_CHECK(monitor, result, Result::ErrorGeneric, ex1);

        MONITORINFO monitor_info;
        monitor_info.cbSize = sizeof(monitor_info);
        JMN_CHECK(GetMonitorInfo(monitor, &monitor_info), result, Result::ErrorGeneric, ex1);

        DWORD style = 0, ex_style = 0;

        RECT rect, windowed_rect;
        windowed_rect.left   = monitor_info.rcWork.left + (monitor_info.rcWork.right - monitor_info.rcWork.left - (LONG)app.settings.window_size.x) / 2;
        windowed_rect.top    = monitor_info.rcWork.top  + (monitor_info.rcWork.bottom - monitor_info.rcWork.top - (LONG)app.settings.window_size.y) / 2;
        windowed_rect.right  = windowed_rect.left + (LONG)app.settings.window_size.x;
        windowed_rect.bottom = windowed_rect.top  + (LONG)app.settings.window_size.y;
        JMN_CHECK(AdjustWindowRectEx(&windowed_rect, WS_OVERLAPPEDWINDOW, FALSE, 0), result, Result::ErrorGeneric, ex1);

        if (app.settings.flags & Settings::FullScreenBit)
        {
            style    = WS_POPUP;
            ex_style = 0;

            rect = monitor_info.rcMonitor;
        }
        else
        {
            style    = WS_OVERLAPPEDWINDOW;
            ex_style = 0;

            rect = windowed_rect;
        }

        app.main_window = CreateWindowEx(ex_style, MAKEINTATOM(app.window_class), (LPCWSTR)ApplicationNameUTF16.string, style,
            rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top,
            NULL, NULL, app.hInstance, &app);
        JMN_CHECK(app.main_window, result, Result::ErrorGeneric, ex1);

        app.main_window_placement.length = sizeof(app.main_window_placement);
        JMN_CHECK(GetWindowPlacement(app.main_window, &app.main_window_placement), result, Result::ErrorGeneric, ex2);

        if (app.settings.flags & Settings::FullScreenBit)
        {
            app.main_window_placement.rcNormalPosition = windowed_rect;
        }

        return true;
    ex2:JMN_ASSERT(DestroyWindow(app.main_window));
    ex1:DestroyWindowClass(app.window_class, app.hInstance);
    ex0:return false;
    }

    static void DestroyMainWindow(Application &app)
    {
        JMN_ASSERT(DestroyWindow(app.main_window));
        DestroyWindowClass(app.window_class, app.hInstance);
    }

    static jmn::B8 CreateVulkanBackend(Application &app, jmn::Result &result)
    {
        using namespace jmn;

        if (!Create(MakeAllocator(app.heap), ApplicationVkVersion, (app.settings.flags & Settings::DebugBit) ? VulkanDebugUtilsMessengerCallback : NULL, &app, app.vkctx, result)) goto ex0;
        if (!Create(MakeAllocator(app.heap), app.main_window, app.hInstance, app.vkctx, app.settings, app.vksc, result)) goto ex1;
        if (!Create(MakeAllocator(app.heap), app.vkctx, 2, app.vkif, result)) goto ex2;

        return true;
    //ex3:Destroy(app.vkif, MakeAllocator(app.heap), app.vkctx);
    ex2:Destroy(app.vksc, MakeAllocator(app.heap), app.vkctx);
    ex1:Destroy(app.vkctx);
    ex0:return false;
    }

    static void DestroyVulkanBackend(Application &app)
    {
        Destroy(app.vkif, MakeAllocator(app.heap), app.vkctx);
        Destroy(app.vksc, MakeAllocator(app.heap), app.vkctx);
        Destroy(app.vkctx);
    }

    static jmn::B8 CreateGUIBackend(Application &app, jmn::Result &result)
    {
        using namespace jmn;

        JMN_CHECK(IMGUI_CHECKVERSION(), result, Result::ErrorNotSupported, ex0);
        ImGui::SetAllocatorFunctions(ImGuiAllocCallback, ImGuiFreeCallback, &app);
        JMN_CHECK(ImGui::CreateContext(), result, Result::ErrorGeneric, ex0);

        auto &IO = ImGui::GetIO();

        IO.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        IO.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
        IO.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        IO.IniFilename  = NULL;

        ImGui::StyleColorsDark();

        JMN_CHECK(ImGui_ImplWin32_Init(app.main_window), result, Result::ErrorGeneric, ex1);

        {
            auto &PIO = ImGui::GetPlatformIO();
            PIO.Platform_CreateVkSurface = ImGuiCreateVkSurfaceCallback;

            ImGui_ImplVulkan_InitInfo ii ={};
            ii.ApiVersion          = ApplicationVkVersion;
            ii.Instance            = app.vkctx.ins;
            ii.PhysicalDevice      = app.vkctx.pd;
            ii.Device              = app.vkctx.dev;
            ii.QueueFamily         = app.vkctx.g_qfi;
            ii.Queue               = app.vkctx.g_q;
            ii.DescriptorPool      = VK_NULL_HANDLE;
            ii.DescriptorPoolSize  = 1024;
            ii.MinImageCount       = app.vksc.mic;
            ii.ImageCount          = app.vksc.img_c;
            ii.PipelineCache       = VK_NULL_HANDLE;
            SetImGuiMainPipelineInfo(app, ii.PipelineInfoMain);
            ii.UseDynamicRendering = true;
            ii.Allocator           = app.vkctx.ac;
            ii.CheckVkResultFn     = [](VkResult x) { JMN_ASSERT(x == VK_SUCCESS); };
            ii.MinAllocationSize   = (VkDeviceSize)MiB(1);
            JMN_CHECK(ImGui_ImplVulkan_Init(&ii), result, Result::ErrorGeneric, ex2);
        }

        return true;
    //ex3:ImGui_ImplVulkan_Shutdown();
    ex2:ImGui_ImplWin32_Shutdown();
    ex1:ImGui::DestroyContext();
    ex0:return false;
    }

    static void DestroyGUIBackend(Application &)
    {
        ImGui_ImplVulkan_Shutdown();
        ImGui_ImplWin32_Shutdown();
        ImGui::DestroyContext();
    }

    static jmn::B8 CreateDrawBuffers(Application &app, jmn::Result &result)
    {
        VkDeviceSize const size = sizeof(Vertex) * app.ac.fmt->nSamplesPerSec;

        if (!Create(app.vkctx.ac, app.vkctx.dev, size, VK_BUFFER_USAGE_2_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_2_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_2_TRANSFER_DST_BIT, app.draw_buffer, result)) goto ex0;
        if (!AllocateDedicated(app.vkctx.ac, app.vkctx.pd_mp2.memoryProperties, app.vkctx.dev, app.draw_buffer, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, app.draw_memory, result)) goto ex1;
        VK_CHECK(vkBindBufferMemory(app.vkctx.dev, app.draw_buffer, app.draw_memory, 0), result, ex2);

        if (!Create(app.vkctx.ac, app.vkctx.dev, size, VK_BUFFER_USAGE_2_TRANSFER_SRC_BIT, app.copy_buffer, result)) goto ex2;
        if (!AllocateDedicated(app.vkctx.ac, app.vkctx.pd_mp2.memoryProperties, app.vkctx.dev, app.copy_buffer, VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, app.copy_memory, result))
            if (!AllocateDedicated(app.vkctx.ac, app.vkctx.pd_mp2.memoryProperties, app.vkctx.dev, app.copy_buffer, VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, app.copy_memory, result)) goto ex3;
        VK_CHECK(vkBindBufferMemory(app.vkctx.dev, app.copy_buffer, app.copy_memory, 0), result, ex4);
        VK_CHECK(vkMapMemory(app.vkctx.dev, app.copy_memory, 0, size, 0, (void **)&app.copy_addr), result, ex4);

        return true;
    ex4:vkFreeMemory(app.vkctx.dev, app.copy_memory, app.vkctx.ac);
    ex3:vkDestroyBuffer(app.vkctx.dev, app.copy_buffer, app.vkctx.ac);
    ex2:vkFreeMemory(app.vkctx.dev, app.draw_memory, app.vkctx.ac);
    ex1:vkDestroyBuffer(app.vkctx.dev, app.draw_buffer, app.vkctx.ac);
    ex0:return false;
    }

    static void DestroyDrawBuffers(Application &app)
    {
        vkFreeMemory(app.vkctx.dev, app.copy_memory, app.vkctx.ac);
        vkDestroyBuffer(app.vkctx.dev, app.copy_buffer, app.vkctx.ac);
        vkFreeMemory(app.vkctx.dev, app.draw_memory, app.vkctx.ac);
        vkDestroyBuffer(app.vkctx.dev, app.draw_buffer, app.vkctx.ac);
    }

    static jmn::B8 CreateDrawPipeline(Application &app, jmn::Result &result)
    {
        using namespace jmn;

        VkShaderModuleCreateInfo        sm_ci[2];
        VkPipelineShaderStageCreateInfo pss_ci[2];
        {
            pss_ci[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            sm_ci[0] .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;

            pss_ci[0].pNext = &sm_ci[0];
            sm_ci[0] .pNext = NULL;

            pss_ci[0].flags               = 0;
            pss_ci[0].stage               = VK_SHADER_STAGE_VERTEX_BIT;
            pss_ci[0].module              = VK_NULL_HANDLE;
            pss_ci[0].pName               = "main";
            pss_ci[0].pSpecializationInfo = NULL;

            sm_ci[0].flags    = 0;
            sm_ci[0].codeSize = sizeof(draw_vert_spv);
            sm_ci[0].pCode    = draw_vert_spv;
        }
        {
            pss_ci[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            sm_ci[1] .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;

            pss_ci[1].pNext = &sm_ci[1];
            sm_ci[1] .pNext = NULL;

            pss_ci[1].flags               = 0;
            pss_ci[1].stage               = VK_SHADER_STAGE_FRAGMENT_BIT;
            pss_ci[1].module              = VK_NULL_HANDLE;
            pss_ci[1].pName               = "main";
            pss_ci[1].pSpecializationInfo = NULL;

            sm_ci[1].flags    = 0;
            sm_ci[1].codeSize = sizeof(draw_frag_spv);
            sm_ci[1].pCode    = draw_frag_spv;
        }

        VkVertexInputBindingDescription vibd[1];
        vibd[0].binding   = 0;
        vibd[0].stride    = sizeof(Vertex);
        vibd[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        VkVertexInputAttributeDescription viad[4];
        viad[0].location = 0;
        viad[0].binding  = vibd[0].binding;
        viad[0].format   = VK_FORMAT_R16G16B16_SNORM;
        viad[0].offset   = offsetof(Vertex, p);

        viad[1].location = 1;
        viad[1].binding  = vibd[0].binding;
        viad[1].format   = VK_FORMAT_R16_UNORM;
        viad[1].offset   = offsetof(Vertex, z);

        viad[2].location = 2;
        viad[2].binding  = vibd[0].binding;
        viad[2].format   = VK_FORMAT_R16_UINT;
        viad[2].offset   = offsetof(Vertex, s);

        viad[3].location = 3;
        viad[3].binding  = vibd[0].binding;
        viad[3].format   = VK_FORMAT_R8G8B8A8_UNORM;
        viad[3].offset   = offsetof(Vertex, c);

        VkPipelineVertexInputStateCreateInfo pvsis_ci;
        pvsis_ci.sType                           = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        pvsis_ci.pNext                           = NULL;
        pvsis_ci.flags                           = 0;
        pvsis_ci.vertexBindingDescriptionCount   = (U32)Length(vibd);
        pvsis_ci.pVertexBindingDescriptions      = vibd;
        pvsis_ci.vertexAttributeDescriptionCount = (U32)Length(viad);
        pvsis_ci.pVertexAttributeDescriptions    = viad;

        VkPipelineInputAssemblyStateCreateInfo pvias_ci;
        pvias_ci.sType                  = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        pvias_ci.pNext                  = NULL;
        pvias_ci.flags                  = 0;
        pvias_ci.topology               = VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
        pvias_ci.primitiveRestartEnable = VK_FALSE;

        VkViewport viewport;
        viewport.x        = 0.0f;
        viewport.y        = (F32)app.vksc.ext.height;
        viewport.width    = (F32)app.vksc.ext.width;

        viewport.height   = -(F32)app.vksc.ext.height;
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;

        VkRect2D scissor;
        scissor.offset.x      = 0;
        scissor.offset.y      = 0;
        scissor.extent.width  = app.vksc.ext.width;
        scissor.extent.height = app.vksc.ext.height;

        VkPipelineViewportStateCreateInfo pvs_ci;
        pvs_ci.sType         = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        pvs_ci.pNext         = NULL;
        pvs_ci.flags         = 0;
        pvs_ci.viewportCount = 1;
        pvs_ci.pViewports    = &viewport;
        pvs_ci.scissorCount  = 1;
        pvs_ci.pScissors     = &scissor;

        VkPipelineRasterizationStateCreateInfo prs_ci;
        prs_ci.sType                   = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        prs_ci.pNext                   = NULL;
        prs_ci.flags                   = 0;
        prs_ci.depthClampEnable        = VK_FALSE;
        prs_ci.rasterizerDiscardEnable = VK_FALSE;
        prs_ci.polygonMode             = VK_POLYGON_MODE_FILL;
        prs_ci.cullMode                = VK_CULL_MODE_NONE;
        prs_ci.frontFace               = VK_FRONT_FACE_COUNTER_CLOCKWISE;
        prs_ci.depthBiasEnable         = VK_FALSE;
        prs_ci.depthBiasConstantFactor = 0.0f;
        prs_ci.depthBiasClamp          = 0.0f;
        prs_ci.depthBiasSlopeFactor    = 0.0f;
        prs_ci.lineWidth               = 1.0f;

        VkPipelineMultisampleStateCreateInfo pms_ci;
        pms_ci.sType                 = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        pms_ci.pNext                 = NULL;
        pms_ci.flags                 = 0;
        pms_ci.rasterizationSamples  = VK_SAMPLE_COUNT_1_BIT;
        pms_ci.sampleShadingEnable   = VK_FALSE;
        pms_ci.minSampleShading      = 0.0f;
        pms_ci.pSampleMask           = NULL;
        pms_ci.alphaToCoverageEnable = VK_FALSE;
        pms_ci.alphaToOneEnable      = VK_FALSE;

        VkPipelineDepthStencilStateCreateInfo pdss_ci;
        pdss_ci.sType                 = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        pdss_ci.pNext                 = NULL;
        pdss_ci.flags                 = 0;
        pdss_ci.depthTestEnable       = VK_TRUE;
        pdss_ci.depthWriteEnable      = VK_TRUE;
        pdss_ci.depthCompareOp        = VK_COMPARE_OP_GREATER_OR_EQUAL;
        pdss_ci.depthBoundsTestEnable = VK_FALSE;
        pdss_ci.stencilTestEnable     = VK_FALSE;
        pdss_ci.front.failOp          = VK_STENCIL_OP_KEEP;
        pdss_ci.front.passOp          = VK_STENCIL_OP_KEEP;
        pdss_ci.front.depthFailOp     = VK_STENCIL_OP_KEEP;
        pdss_ci.front.compareOp       = VK_COMPARE_OP_NEVER;
        pdss_ci.front.compareMask     = 0;
        pdss_ci.front.writeMask       = 0;
        pdss_ci.front.reference       = 0;
        pdss_ci.back.failOp           = VK_STENCIL_OP_KEEP;
        pdss_ci.back.passOp           = VK_STENCIL_OP_KEEP;
        pdss_ci.back.depthFailOp      = VK_STENCIL_OP_KEEP;
        pdss_ci.back.compareOp        = VK_COMPARE_OP_NEVER;
        pdss_ci.back.compareMask      = 0;
        pdss_ci.back.writeMask        = 0;
        pdss_ci.back.reference        = 0;
        pdss_ci.minDepthBounds        = 0.0f;
        pdss_ci.maxDepthBounds        = 1.0f;

        VkPipelineColorBlendAttachmentState attachments[1];
        attachments[0].blendEnable         = VK_TRUE;
        attachments[0].srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
        attachments[0].dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
        attachments[0].colorBlendOp        = VK_BLEND_OP_ADD;
        attachments[0].srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
        attachments[0].dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
        attachments[0].alphaBlendOp        = VK_BLEND_OP_ADD;
        attachments[0].colorWriteMask      = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

        VkPipelineColorBlendStateCreateInfo pcbs_ci;
        pcbs_ci.sType             = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        pcbs_ci.pNext             = NULL;
        pcbs_ci.flags             = 0;
        pcbs_ci.logicOpEnable     = VK_FALSE;
        pcbs_ci.logicOp           = VK_LOGIC_OP_CLEAR;
        pcbs_ci.attachmentCount   = (U32)Length(attachments);
        pcbs_ci.pAttachments      = attachments;
        pcbs_ci.blendConstants[0] = 0.0f;
        pcbs_ci.blendConstants[1] = 0.0f;
        pcbs_ci.blendConstants[2] = 0.0f;
        pcbs_ci.blendConstants[3] = 0.0f;

        VkPipelineDynamicStateCreateInfo pds_ci;
        pds_ci.sType             = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        pds_ci.pNext             = NULL;
        pds_ci.flags             = 0;
        pds_ci.dynamicStateCount = 0;
        pds_ci.pDynamicStates    = NULL;

        VkPipelineCreationFeedback pipeline_feedback;
        VkPipelineCreationFeedback stages_feedback[2];

        VkPipelineRenderingCreateInfo        pr_ci;
        VkPipelineCreationFeedbackCreateInfo pcf_ci;
        VkGraphicsPipelineCreateInfo         gp_ci;
        gp_ci .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pcf_ci.sType = VK_STRUCTURE_TYPE_PIPELINE_CREATION_FEEDBACK_CREATE_INFO;
        pr_ci .sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;

        gp_ci .pNext = &pcf_ci;
        pcf_ci.pNext = &pr_ci;
        pr_ci .pNext = NULL;

        gp_ci.flags               = 0;
        gp_ci.stageCount          = (U32)Length(pss_ci);
        gp_ci.pStages             = pss_ci;
        gp_ci.pVertexInputState   = &pvsis_ci;
        gp_ci.pInputAssemblyState = &pvias_ci;
        gp_ci.pTessellationState  = NULL;
        gp_ci.pViewportState      = &pvs_ci;
        gp_ci.pRasterizationState = &prs_ci;
        gp_ci.pMultisampleState   = &pms_ci;
        gp_ci.pDepthStencilState  = &pdss_ci;
        gp_ci.pColorBlendState    = &pcbs_ci;
        gp_ci.pDynamicState       = &pds_ci;
        gp_ci.layout              = app.draw_pll;
        gp_ci.renderPass          = VK_NULL_HANDLE;
        gp_ci.subpass             = 0;
        gp_ci.basePipelineHandle  = VK_NULL_HANDLE;
        gp_ci.basePipelineIndex   = 0;

        pcf_ci.pPipelineCreationFeedback          = &pipeline_feedback;
        pcf_ci.pipelineStageCreationFeedbackCount = (U32)Length(stages_feedback);
        pcf_ci.pPipelineStageCreationFeedbacks    = stages_feedback;

        pr_ci.viewMask                = 0;
        pr_ci.colorAttachmentCount    = 1;
        pr_ci.pColorAttachmentFormats = &app.vksc.fmt;
        pr_ci.depthAttachmentFormat   = VulkanSwapChain::DepthFormat;
        pr_ci.stencilAttachmentFormat = VulkanSwapChain::DepthFormat;

        VK_CHECK(vkCreateGraphicsPipelines(app.vkctx.dev, VK_NULL_HANDLE, 1, &gp_ci, app.vkctx.ac, &app.draw_pl), result, ex0);

        if (app.settings.flags & Settings::DebugBit)
        {
            if ((pipeline_feedback.flags & VK_PIPELINE_CREATION_FEEDBACK_VALID_BIT) &&
                (stages_feedback[0].flags & VK_PIPELINE_CREATION_FEEDBACK_VALID_BIT) &&
                (stages_feedback[1].flags & VK_PIPELINE_CREATION_FEEDBACK_VALID_BIT))
            {
                auto const total = pipeline_feedback.duration + stages_feedback[0].duration + stages_feedback[1].duration;
                PrintConsole(GetStdHandle(STD_OUTPUT_HANDLE), "Draw Pipeline took %dus to compile (%d+(%d+%d))\n",
                    total / 1000, pipeline_feedback.duration / 1000, stages_feedback[0].duration / 1000, stages_feedback[1].duration / 1000);
            }
        }

        return true;
    ex0:return false;
    }

    static void DestroyDrawPipeline(Application &app)
    {
        vkDestroyPipeline(app.vkctx.dev, app.draw_pl, app.vkctx.ac);
    }

    static jmn::B8 CreateDrawObjects(Application &app, jmn::Result &result)
    {
        if (!CreateDrawBuffers(app, result)) goto ex0;
        if (!Create(app.vkctx.ac, app.vkctx.dev, 0, NULL, 0, NULL, app.draw_pll, result)) goto ex1;
        if (!CreateDrawPipeline(app, result)) goto ex2;
        return true;
    ex2:vkDestroyPipelineLayout(app.vkctx.dev, app.draw_pll, app.vkctx.ac);
    ex1:DestroyDrawBuffers(app);
    ex0:return false;
    }

    static void DestroyDrawObjects(Application &app)
    {
        DestroyDrawPipeline(app);
        vkDestroyPipelineLayout(app.vkctx.dev, app.draw_pll, app.vkctx.ac);
        DestroyDrawBuffers(app);
    }

    static void TransitionSwapChainImageToDraw(Application &app)
    {
        using namespace jmn;

        auto const cb = app.vkif.cb[app.vkif.index];

        VkImageMemoryBarrier2 imb2[2];
        imb2[0].sType               = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
        imb2[0].pNext               = NULL;
        imb2[0].srcStageMask        = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
        imb2[0].srcAccessMask       = VK_ACCESS_2_NONE;
        imb2[0].dstStageMask        = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
        imb2[0].dstAccessMask       = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT;
        imb2[0].oldLayout           = VK_IMAGE_LAYOUT_UNDEFINED;
        imb2[0].newLayout           = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        imb2[0].srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        imb2[0].dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        imb2[0].image               = app.vksc.img_a[app.vksc.img_i];
        imb2[0].subresourceRange    ={ VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };

        imb2[1].sType               = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
        imb2[1].pNext               = NULL;
        imb2[1].srcStageMask        = VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_2_LATE_FRAGMENT_TESTS_BIT;
        imb2[1].srcAccessMask       = 0;
        imb2[1].dstStageMask        = VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_2_LATE_FRAGMENT_TESTS_BIT;
        imb2[1].dstAccessMask       = VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
        imb2[1].oldLayout           = VK_IMAGE_LAYOUT_UNDEFINED;
        imb2[1].newLayout           = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        imb2[1].srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        imb2[1].dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        imb2[1].image               = app.vksc.dps_img;
        imb2[1].subresourceRange    ={ VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT, 0, 1, app.vksc.img_i, 1 };

        VkDependencyInfo di;
        di.sType                    = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
        di.pNext                    = NULL;
        di.dependencyFlags          = 0;
        di.memoryBarrierCount       = 0;
        di.pMemoryBarriers          = NULL;
        di.bufferMemoryBarrierCount = 0;
        di.pBufferMemoryBarriers    = NULL;
        di.imageMemoryBarrierCount  = (U32)Length(imb2);
        di.pImageMemoryBarriers     = imb2;
        vkCmdPipelineBarrier2(cb, &di);
    }

    static void TransitionSwapChainImageToPresent(Application &app)
    {
        using namespace jmn;

        auto const cb = app.vkif.cb[app.vkif.index];

        VkImageMemoryBarrier2 imb2[1];
        imb2[0].sType               = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
        imb2[0].pNext               = NULL;
        imb2[0].srcStageMask        = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
        imb2[0].srcAccessMask       = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT;
        imb2[0].dstStageMask        = VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT;
        imb2[0].dstAccessMask       = VK_ACCESS_2_MEMORY_READ_BIT;
        imb2[0].oldLayout           = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        imb2[0].newLayout           = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        imb2[0].srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        imb2[0].dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        imb2[0].image               = app.vksc.img_a[app.vksc.img_i];
        imb2[0].subresourceRange    ={ VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
        VkDependencyInfo di;
        di.sType                    = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
        di.pNext                    = NULL;
        di.dependencyFlags          = 0;
        di.memoryBarrierCount       = 0;
        di.pMemoryBarriers          = NULL;
        di.bufferMemoryBarrierCount = 0;
        di.pBufferMemoryBarriers    = NULL;
        di.imageMemoryBarrierCount  = (U32)Length(imb2);
        di.pImageMemoryBarriers     = imb2;
        vkCmdPipelineBarrier2(cb, &di);
    }

    static void BeginDraw(Application &app)
    {
        using namespace jmn;

        auto const cb = app.vkif.cb[app.vkif.index];

        VkRenderingAttachmentInfo color_attachments[1];
        color_attachments[0].sType              = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
        color_attachments[0].pNext              = NULL;
        color_attachments[0].imageView          = app.vksc.imgv_a[app.vksc.img_i];
        color_attachments[0].imageLayout        = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        color_attachments[0].resolveMode        = VK_RESOLVE_MODE_NONE;
        color_attachments[0].resolveImageView   = VK_NULL_HANDLE;
        color_attachments[0].resolveImageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        color_attachments[0].loadOp             = VK_ATTACHMENT_LOAD_OP_CLEAR;
        color_attachments[0].storeOp            = VK_ATTACHMENT_STORE_OP_STORE;
        color_attachments[0].clearValue.color.float32[0] = 0.0f;
        color_attachments[0].clearValue.color.float32[1] = 0.0f;
        color_attachments[0].clearValue.color.float32[2] = 0.0f;
        color_attachments[0].clearValue.color.float32[3] = 0.0f;

        VkRenderingAttachmentInfo depth_attachment;
        depth_attachment.sType              = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
        depth_attachment.pNext              = NULL;
        depth_attachment.imageView          = app.vksc.dps_imgv_a[app.vksc.img_i];
        depth_attachment.imageLayout        = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        depth_attachment.resolveMode        = VK_RESOLVE_MODE_NONE;
        depth_attachment.resolveImageView   = VK_NULL_HANDLE;
        depth_attachment.resolveImageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        depth_attachment.loadOp             = VK_ATTACHMENT_LOAD_OP_CLEAR;
        depth_attachment.storeOp            = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depth_attachment.clearValue.depthStencil.depth   = 0.0f;
        depth_attachment.clearValue.depthStencil.stencil = 0;

        VkRenderingInfo ri;
        ri.sType                = VK_STRUCTURE_TYPE_RENDERING_INFO;
        ri.pNext                = NULL;
        ri.flags                = 0;
        ri.renderArea           ={ { 0, 0 }, app.vksc.ext };
        ri.layerCount           = app.vksc.ial;
        ri.viewMask             = 0;
        ri.colorAttachmentCount = (U32)Length(color_attachments);
        ri.pColorAttachments    = color_attachments;
        ri.pDepthAttachment     = &depth_attachment;
        ri.pStencilAttachment   = &depth_attachment;
        vkCmdBeginRendering(cb, &ri);
    }

    static void EndDraw(Application &app)
    {
        using namespace jmn;

        auto const cb = app.vkif.cb[app.vkif.index];

        vkCmdEndRendering(cb);
    }

    static jmn::B8 ProcessMessages(jmn::Result &result)
    {
        using namespace jmn;

        for (MSG message; PeekMessage(&message, NULL, 0, 0, PM_REMOVE);)
        {
            TranslateMessage(&message);
            DispatchMessage(&message);
            if (message.message == WM_QUIT)
            {
                result = (Result)message.wParam;
                return false;
            }
        }
        return true;
    }

    static jmn::B8 UpdateSwapChain(Application &app, jmn::Result &result)
    {
        if (app.vksc.flags & VulkanSwapChain::OutdatedBit)
        {
            VK_CHECK(vkWaitForFences(app.vkctx.dev, app.vkif.count, app.vkif.sc_imgp, VK_TRUE, UINT64_MAX), result, ex0);

            if (!Recreate(app.vksc, MakeAllocator(app.heap), app.vkctx, app.settings, result)) goto ex0;

            ImGui_ImplVulkan_PipelineInfo new_pipeline_info;
            SetImGuiMainPipelineInfo(app, new_pipeline_info);
            ImGui_ImplVulkan_CreateMainPipeline(&new_pipeline_info);

            //vkDestroyPipeline(app.vkc.dev, app.draw_pl, app.vkc.ac);
            //if (!CreateDrawPipeline(app, result)) goto ex0;
        }
        return true;
    ex0:return false;
    }

    static void ProcessGUI(Application &)
    {
        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();
        //RecordImGuiFrame(app);
        ImGui::EndFrame();
        ImGui::Render();
    }

    static jmn::B8 AcquireFrame(Application &app, jmn::Result &result)
    {
        VK_CHECK(vkWaitForFences(app.vkctx.dev, 1, app.vkif.sc_imgp + app.vkif.index, VK_TRUE, UINT64_MAX), result, ex0);
        VK_CHECK(vkResetFences(app.vkctx.dev, 1, app.vkif.sc_imgp + app.vkif.index), result, ex0);

        if (!Acquire(app.vksc, app.vkctx, app.vkif.sc_imga[app.vkif.index], result)) goto ex0;

        return true;
    ex0:return false;
    }

    static jmn::B8 RecordFrame(Application &app, jmn::Result &result)
    {
        using namespace jmn;

        auto const cb = app.vkif.cb[app.vkif.index];

        {
            VkCommandBufferBeginInfo cbbi;
            cbbi.sType            = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
            cbbi.pNext            = NULL;
            cbbi.flags            = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
            cbbi.pInheritanceInfo = NULL;
            VK_CHECK(vkBeginCommandBuffer(cb, &cbbi), result, ex0);
        }

        TransitionSwapChainImageToDraw(app);

        BeginDraw(app);
        {
            //VkDeviceSize offset = 0, size = app.draw_vtx_count * sizeof(Vertex), stride = sizeof(Vertex);
            //vkCmdBindVertexBuffers2(cb, 0, 1, &app.draw_buffer, &offset, &size, &stride);
            //vkCmdBindPipeline(cb, VK_PIPELINE_BIND_POINT_GRAPHICS, app.draw_pl);
            //vkCmdDraw(cb, (U32)app.draw_vtx_count, 1, 0, 0);
            ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), cb);
        }
        EndDraw(app);

        TransitionSwapChainImageToPresent(app);

        VK_CHECK(vkEndCommandBuffer(cb), result, ex0);
        return true;
    ex0:return false;
    }

    static jmn::B8 PresentFrame(Application &app, jmn::Result &result)
    {
        using namespace jmn;

        VkSemaphoreSubmitInfo     wssi[1];
        VkCommandBufferSubmitInfo cbsi[1];
        VkSemaphoreSubmitInfo     sssi[1];

        wssi[0].sType       = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
        wssi[0].pNext       = NULL;
        wssi[0].semaphore   = app.vkif.sc_imga[app.vkif.index];
        wssi[0].value       = 0;
        wssi[0].stageMask   = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
        wssi[0].deviceIndex = 0;

        cbsi[0].sType         = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO;
        cbsi[0].pNext         = NULL;
        cbsi[0].commandBuffer = app.vkif.cb[app.vkif.index];
        cbsi[0].deviceMask    = 0;

        sssi[0].sType       = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
        sssi[0].pNext       = NULL;
        sssi[0].semaphore   = app.vkif.sc_imgr[app.vkif.index];
        sssi[0].value       = 0;
        sssi[0].stageMask   = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
        sssi[0].deviceIndex = 0;

        VkSubmitInfo2 si2;
        si2.sType                    = VK_STRUCTURE_TYPE_SUBMIT_INFO_2;
        si2.pNext                    = NULL;
        si2.flags                    = 0;
        si2.waitSemaphoreInfoCount   = (U32)Length(wssi);
        si2.pWaitSemaphoreInfos      = wssi;
        si2.commandBufferInfoCount   = (U32)Length(cbsi);
        si2.pCommandBufferInfos      = cbsi;
        si2.signalSemaphoreInfoCount = (U32)Length(sssi);
        si2.pSignalSemaphoreInfos    = sssi;
        VK_CHECK(vkQueueSubmit2(app.vkctx.g_q, 1, &si2, VK_NULL_HANDLE), result, ex0);

        if (!Present(app.vksc, app.vkctx, 1, app.vkif.sc_imgr + app.vkif.index, app.vkif.sc_imgp[app.vkif.index], result)) goto ex0;

        app.vkif.index = (app.vkif.index + 1) % app.vkif.count;
        return true;
    ex0:return false;

    }

}

jmn::B8 Create(HINSTANCE hInstance, Application &app, jmn::Result &result)
{
    using namespace jmn;

    app.hInstance = hInstance;

    ApplicationInternal::InitializeMemory(app);
    if (!Create(MakeAllocator(app.heap), SettingsFileName, app.settings, result)) goto ex0;
    if (!ApplicationInternal::CreateEnvironment(app, result)) goto ex1;
    if (!ApplicationInternal::CreateMainWindow(app, result)) goto ex2;
    if (!ApplicationInternal::CreateVulkanBackend(app, result)) goto ex3;
    if (!ApplicationInternal::CreateGUIBackend(app, result)) goto ex4;
    if (!Create(app.ac, result)) goto ex5;
    if (!ApplicationInternal::CreateDrawObjects(app, result)) goto ex6;

    return true;
//ex7:ApplicationInternal::DestroyDrawObjects(app);
ex6:Destroy(app.ac);
ex5:ApplicationInternal::DestroyGUIBackend(app);
ex4:ApplicationInternal::DestroyVulkanBackend(app);
ex3:ApplicationInternal::DestroyMainWindow(app);
ex2:ApplicationInternal::DestroyEnvironment(app);
ex1:Destroy(app.settings, SettingsFileName);
ex0:return false;
}

jmn::Result Run(Application &app)
{
    using namespace jmn;

    auto result = Result::Success;

    ShowWindow(app.main_window, SW_SHOW);

    app.flags |= Application::RunningBit;
    while (app.flags & Application::RunningBit)
    {
        if (!ApplicationInternal::ProcessMessages(result)) break;
        if (!ApplicationInternal::UpdateSwapChain(app, result)) break;
        ApplicationInternal::ProcessGUI(app);
        if (!ApplicationInternal::AcquireFrame(app, result)) break;
        if (!ApplicationInternal::RecordFrame (app, result)) break;
        if (!ApplicationInternal::PresentFrame(app, result)) break;
    }
    app.flags &=~ Application::RunningBit;

    ShowWindow(app.main_window, SW_HIDE);

    VK_CHECK(vkDeviceWaitIdle(app.vkctx.dev), result, ex0);

ex0:return result;
}

void Destroy(Application &app)
{
    ApplicationInternal::DestroyDrawObjects(app);
    Destroy(app.ac);
    ApplicationInternal::DestroyGUIBackend(app);
    ApplicationInternal::DestroyVulkanBackend(app);
    ApplicationInternal::DestroyMainWindow(app);
    ApplicationInternal::DestroyEnvironment(app);
    Destroy(app.settings, SettingsFileName);
}

#  endif // APPLICATION_IMPLEMENTATED
#endif // APPLICATION_IMPLEMENTATION
