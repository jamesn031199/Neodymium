#include "main.hpp"

#define JMN_HYDROGEN_IMPLEMENTATION
#define JMN_HELIUM_IMPLEMENTATION
#define JMN_LITHIUM_IMPLEMENTATION
#define JMN_BERYLLIUM_IMPLEMENTATION
#define JMN_BORON_IMPLEMENTATION
#define JMN_HYDROGEN_SINGLE_FILE
#define JMN_HELIUM_SINGLE_FILE
#define JMN_LITHIUM_SINGLE_FILE
#define JMN_BERYLLIUM_SINGLE_FILE
#define JMN_BORON_SINGLE_FILE

#define STB_SPRINTF_IMPLEMENTATION

#define VOLK_IMPLEMENTATION

#define SETTINGS_IMPLEMENTATION
#define VULKAN_COMMON_IMPLEMENTATION
#define VULKAN_CONTEXT_IMPLEMENTATION
#define VULKAN_SWAP_CHAIN_IMPLEMENTATION
#define VULKAN_INFLIGHT_FRAMES_IMPLEMENTATION
#define APPLICATION_IMPLEMENTATION

#include <Hydrogen.hpp>
#include <Helium.hpp>
#include <Lithium.hpp>
#include <Beryllium.hpp>
#include <Boron.hpp>

#pragma warning(push)
#pragma warning(disable:4365)
#pragma warning(disable:4820)
#pragma warning(disable:5262)
#include <stb_sprintf.h>
#pragma warning(pop)

#pragma warning(push)
#pragma warning(disable:4191)
#include <volk.h>
#pragma warning(pop)

// Prevent ImGui from implementing stb
#undef STB_SPRINTF_IMPLEMENTATION

#pragma warning(push)
#pragma warning(disable:4191)
#pragma warning(disable:4244)
#pragma warning(disable:4245)
#pragma warning(disable:4365)
#pragma warning(disable:4774)
#pragma warning(disable:4820)
#pragma warning(disable:5219)
#include <imgui.cpp>
#include <imgui_widgets.cpp>
#include <imgui_tables.cpp>
#include <imgui_draw.cpp>
#include <backends/imgui_impl_win32.cpp>
#include <backends/imgui_impl_vulkan.cpp>
#pragma warning(pop)

#include "settings.hpp"
#include "vulkan_common.hpp"
#include "vulkan_context.hpp"
#include "vulkan_swap_chain.hpp"
#include "vulkan_inflight_frames.hpp"
#include "application.hpp"

#define HR_CHECK(expr, resvar, resval, jmplbl) JMN_CHECK(SUCCEEDED(expr), resvar, resval, jmplbl)
#define SafeRelease(ptr) if (ptr) { ptr->Release(); ptr = NULL; } else {}

int wWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPWSTR /*lpCmdLine*/, int /*nShowCmd*/)
{
    using namespace jmn;

    auto result = Result::Success;

    auto const app_ptr = VirtualAlloc((LPVOID)ApplicationBaseAddr, sizeof(Application) + ApplicationArenaSize + ApplicationHeapSize, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    JMN_CHECK(app_ptr, result, Result::ErrorOutOfMemory, ex0);

    auto &app = *(Application *)app_ptr;
    if (!Create(hInstance, app, result)) goto ex1;

    result = Run(app);

    Destroy(app);

ex1:JMN_ASSERT(VirtualFree(app_ptr, 0, MEM_RELEASE));
ex0:return (int)result;
}
