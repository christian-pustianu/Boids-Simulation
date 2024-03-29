workspace "Boids-Simulation"
	language "C++"
	cppdialect "C++17"

	platforms { "x64" }
	configurations { "debug", "release" }

	flags "NoPCH"
	flags "MultiProcessorCompile"

	startproject "main"

	debugdir "%{wks.location}"
	objdir "_build_/%{cfg.buildcfg}-%{cfg.platform}-%{cfg.toolset}"
	targetsuffix "-%{cfg.buildcfg}-%{cfg.platform}-%{cfg.toolset}"
	
	-- Default toolset options
	filter "toolset:gcc or toolset:clang"
		linkoptions { "-pthread" }
		buildoptions { "-march=native", "-Wall", "-pthread" }
		buildoptions { "-Werror=vla" }

	filter "toolset:msc-*"
		warnings "extra" -- this enables /W4; default is /W3
		--buildoptions { "/W4" }
		buildoptions { "/utf-8" }
		buildoptions { "/permissive-" }
		defines { "_CRT_SECURE_NO_WARNINGS=1" }
		defines { "_SCL_SECURE_NO_WARNINGS=1" }
	
	filter "*"

	-- default libraries
	filter "system:linux"
		links "dl"
	
	filter "system:windows"
		links "OpenGL32"

	filter "*"

	-- default outputs
	filter "kind:StaticLib"
		targetdir "lib/"

	filter "kind:ConsoleApp"
		targetdir "bin/"
		targetextension ".exe"
	
	filter "*"

	--configurations
	filter "debug"
		symbols "On"
		defines { "_DEBUG=1" }

	filter "release"
		optimize "On"
		defines { "NDEBUG=1" }

	filter "*"


-- Third party dependencies
include "third_party" 

-- Projects
project "main"
	local sources = { 
		"main/**.cpp",
		"main/**.hpp",
		"main/**.hxx",
		"main/**.inl"
	}

	kind "ConsoleApp"
	location "main"

	files( sources )

	links "math"

	links "x-stb"
	links "x-glad"
	links "x-glfw"
	links "x-imgui"

	files( sources )

project "main-shaders"
	local shaders = { 
		"assets/shaders/*.vert",
		"assets/shaders/*.frag",
		"assets/shaders/*.geom",
		"assets/shaders/*.tesc",
		"assets/shaders/*.tese",
		"assets/shaders/*.comp"
	}

	kind "Utility"
	location "assets"

	files( shaders )

project "math"
	local sources = { 
		"math/**.cpp",
		"math/**.hpp",
		"math/**.hxx",
		"math/**.inl"
	}

	kind "StaticLib"
	location "math"

	files( sources )

--EOF
