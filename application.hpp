#ifndef APPLICATION_INCLUDED
#  define APPLICATION_INCLUDED

#  include <Windows.h>
#  include <Hydrogen.hpp>
#  include "settings.hpp"
#  include "vulkan_context.hpp"
#  include "vulkan_swap_chain.hpp"

struct Application
{
    HINSTANCE        hInstance;
    jmn::MemoryArena arena;
    jmn::MemoryHeap  heap;
    Settings         settings;
    ATOM             window_class; jmn::U8 _pad0[2];
    WINDOWPLACEMENT  main_window_placement;
    HWND             main_window;
    VulkanContext    vkctx;
    VulkanSwapChain  vksc;
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

#    include "constants.hpp"

namespace ApplicationInternal
{

    void InitializeMemory(Application &app)
    {
        using namespace jmn;

        auto addr = (Addr)&app;
        addr += sizeof(Application);
        app.arena.Initialize(addr, ApplicationArenaSize);
        addr += ApplicationArenaSize;
        app.heap.Create(addr, ApplicationHeapSize);
        addr += ApplicationHeapSize;
    }

    jmn::B8 CreateEnvironment(Application &app, jmn::Result &result)
    {
        using namespace jmn;

        if (app.settings.flags & Settings::DebugBit) JMN_CHECK(AllocConsole(), result, Result::ErrorGeneric, ex0);

        return true;
    ex0:return false;
    }

    void DestroyEnvironment(Application &app)
    {
        if (app.settings.flags & Settings::DebugBit) JMN_ASSERT(FreeConsole());
    }

    jmn::B8 CreateWindowClass(HINSTANCE hInstance, WNDPROC callback, jmn::CString8 class_name, ATOM &atom, jmn::Result &result)
    {
        using namespace jmn;

        WNDCLASSEX class_def;
        class_def.cbSize        = sizeof(class_def);
        class_def.style         = 0;
        class_def.lpfnWndProc   = callback;
        class_def.cbClsExtra    = 0;
        class_def.cbWndExtra    = 0;
        class_def.hInstance     = hInstance;
        class_def.hIcon         = (HICON)::LoadImage(NULL, IDI_APPLICATION, IMAGE_ICON, 0, 0, LR_DEFAULTSIZE | LR_SHARED);
        class_def.hCursor       = (HCURSOR)::LoadImage(NULL, IDC_ARROW, IMAGE_CURSOR, 0, 0, LR_DEFAULTSIZE | LR_SHARED);
        class_def.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
        class_def.lpszMenuName  = NULL;
        class_def.lpszClassName = class_name.string;
        class_def.hIconSm       = NULL;
        JMN_CHECK(atom = ::RegisterClassEx(&class_def), result, Result::ErrorGeneric, ex0);
        return true;
    ex0:return false;
    }

    void DestroyWindowClass(ATOM atom, HINSTANCE hInstance)
    {
        JMN_ASSERT(UnregisterClass(MAKEINTATOM(atom), hInstance));
    }

    jmn::B8 CreateMainWindow(Application &app, jmn::Result &result)
    {

    }

}

jmn::B8 Create(HINSTANCE hInstance, Application &app, jmn::Result &result)
{
    using namespace jmn;

    app.hInstance = hInstance;

    ApplicationInternal::InitializeMemory(app);
    if (!Create(app.arena.MakeTemporaryMemory(), SettingsFileName, app.settings, result)) goto ex0;
    if (!ApplicationInternal::CreateEnvironment(app, result)) goto ex1;

    return true;
//ex2:ApplicationInternal::DestroyEnvironment(app);
ex1:Destroy(app.settings, SettingsFileName);
ex0:return false;
}

jmn::Result Run(Application &app)
{
    using namespace jmn;

    auto result = Result::Success;



    return result;
}

void Destroy(Application &app)
{
    ApplicationInternal::DestroyEnvironment(app);
    Destroy(app.settings, SettingsFileName);
}

#  endif // APPLICATION_IMPLEMENTATED
#endif // APPLICATION_IMPLEMENTATION
