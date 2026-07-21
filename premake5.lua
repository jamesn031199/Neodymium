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
            "$(VULKAN_SDK)/Include/"
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
        
        disablewarnings { "4061", "4201", "4505", "4533", "4577", "4710", "4711", "5045" }
        
        files {
            "main.hpp",
            "main.cpp"
        }
        
        defines {
            "WIN32_LEAN_AND_MEAN"
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
