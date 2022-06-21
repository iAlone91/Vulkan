workspace "Vulkan"
    architecture "x64"
    language "c++"
    
    configurations 
    { 
        "Debug",
        "Release" 
    }
    
    platforms { "Switch", "x64" }

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.platform}"

postbuildcommands {

  }

project "Vulkan"
    location "Vulkan"
    kind "ConsoleApp"
    
    
    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    files
    {
        "%{prj.name}/code/**.cpp",
        "%{prj.name}/code/**.h",
        "%{prj.name}/shaders/**.vert",
        "%{prj.name}/shaders/**.frag",
        "%{prj.name}/textures/.**",
    }

    includedirs { "%{prj.name}/shaders" }

   filter "configurations:Debug"
      defines { "DEBUG" }
      symbols "On"
      buildoptions "/MTd"

   filter "configurations:Release"
      defines { "NDEBUG" }
      optimize "On"
      buildoptions "/MT"
     

    filter "system:windows"
        cppdialect "c++17"
        staticruntime "off"
        systemversion "latest"
        links { "vulkan-1.lib", "glfw3_mt.lib" }
        libdirs { "$(SolutionDir)Dependencies/GLFW/lib" , "$(SolutionDir)Dependencies/Vulkan/lib" }
        includedirs { 
            "$(SolutionDir)Dependencies/Vulkan/include",
            "$(SolutionDir)Dependencies/GLFW/include", 
            "$(SolutionDir)Dependencies/glm", 
            "$(SolutionDir)Dependencies/stb_image",
            "$(SolutionDir)Dependencies/imgui",
        }

        buildcommands {
            "$(SolutionDir)Dependencies/Vulkan/Bin/glslc.exe $(SolutionDir)%{prj.name}/shaders/shader.vert -o $(SolutionDir)%{prj.name}/shaders/vert.spv",
            "$(SolutionDir)Dependencies/Vulkan/Bin/glslc.exe $(SolutionDir)%{prj.name}/shaders/shader.frag -o $(SolutionDir)%{prj.name}/shaders/frag.spv",
            "{COPY} $(SolutionDir)%{prj.name}/textures $(SolutionDir)bin/" .. outputdir .. "/%{prj.name}/textures",
            "{COPY} $(SolutionDir)%{prj.name}/models $(SolutionDir)bin/" .. outputdir .. "/%{prj.name}/models",
            "{COPY} $(SolutionDir)%{prj.name}/shaders/*.spv $(SolutionDir)bin/" .. outputdir .. "/%{prj.name}/shaders",
        }

        buildoutputs { 
            "$(SolutionDir)%{prj.name}/shaders/"
        }

    filter "platforms:Switch"
        defines { "SWITCH"}
    filter "platforms:x64"
        defines { "WINDOWS"}
      

         