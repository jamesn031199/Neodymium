workspace "Neodymium"
    location "build"
    preferredtoolarchitecture "x86_64"
    configurations { "Dbg", "Opt", "Rel" }

    project "Neodymium"
        kind "WindowedApp"
        system "windows"
        
        language "C++"
        compileas "C++"
        cppdialect "C++17"
        
        includedirs {
            "$(VULKAN_SDK)/Include/",
            "jmn/",
            "stb/",
            "volk/",
            "imgui/"
        }
        
        targetdir "bin/%{cfg.buildcfg}"
        targetname "Neodymium"
        targetextension ".exe"
        
        architecture "x64"
        characterset "Unicode"
        conformancemode "Off"
        dpiawareness "HighPerMonitor"
        editandcontinue "Off"
        justmycode "Off"
        minimalrebuild "Off"
        multiprocessorcompile "On"
        nativewchar "On"
        unsignedchar "On"
        usestandardpreprocessor "On"
        warnings "Everything"
        
        buffersecuritycheck "Off"
        exceptionhandling "Off"
        runtimechecks "Off"
        staticruntime "On"
        
        floatabi "hard"
        floatingpoint "Strict"
        floatingpointexceptions "Off"
        vectorextensions "AVX2"
        
        incrementallink "Off"
        
        intrinsics "On"
        
        disablewarnings { "4061", "4201", "4505", "4533", "4577", "4623", "4626", "4710", "4711", "5027", "5045" }
        
        files {
            "main.cpp",
            "main.hpp",
            "application.hpp",
            "audio_capture.hpp",
            "vulkan_inflight_frames.hpp",
            "vulkan_swap_chain.hpp",
            "vulkan_context.hpp",
            "vulkan_common.hpp",
            "settings.hpp",
            "imgui_user_config.hpp",
            "common.hpp",
            "constants.hpp",
        }
        
        defines {
            "WIN32_LEAN_AND_MEAN",
            "NOMINMAX",
            "STB_SPRINTF_STATIC",
            "VK_USE_PLATFORM_WIN32_KHR",
            'IMGUI_USER_CONFIG="imgui_user_config.hpp"'
        }
        
        prebuildcommands  
            {
                'glslangValidator --target-env vulkan1.4 --vn draw_vert_spv ../res/shaders/draw.vert.glsl -o ../draw.vert.spv',
                'glslangValidator --target-env vulkan1.4 --vn draw_frag_spv ../res/shaders/draw.frag.glsl -o ../draw.frag.spv'
            }
        
        filter "configurations:Dbg"
            defines { "DEBUG", "DEBUG_MODE" }
            symbols "Full"
            optimize "Debug"
            inlining "Disabled"
            linktimeoptimization "Off"
            omitframepointer "Off"
            runtime "Debug"
            prebuildcommands  
            {}
            
        filter "configurations:Opt"	
            defines { "NDEBUG", "DEBUG_MODE" }
            symbols "Full"
            optimize "Full"
            inlining "Auto"
            linktimeoptimization "On"
            omitframepointer "On"
            runtime "Release"
            prebuildcommands  
            {}
        
        filter "configurations:Rel"
            defines { "NDEBUG" }
            symbols "Off"
            optimize "Full"
            inlining "Auto"
            linktimeoptimization "On"
            omitframepointer "On"
            runtime "Release"
            prebuildcommands  
            {}
