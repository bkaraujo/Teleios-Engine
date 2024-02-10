project "Engine"
    kind "SharedLib"
    language "C"
    cdialect "C17"
    cppdialect "C++17"
    staticruntime "off"

    targetdir ("%{wks.location}/build/bin/" .. outputdir .. "/%{prj.name}")
    objdir ("%{wks.location}/build/int/" .. outputdir .. "/%{prj.name}")

    files {
        "src/**.h",
        "src/**.c",
        "src/**.cpp",
        "vendor/**.h",
        "vendor/**.c",
        "vendor/**.cpp",
    }

    defines {
        "_CRT_SECURE_NO_WARNINGS",
        "CGLM_STATIC",
        "TELEIOS_EXPORT",
    }

    undefines {
        "UNICODE",
        "_UNICODE",
    }

    includedirs {
        "src",
        "vendor",
    }

    links {
        "opengl32.lib"
    }

    filter "system:windows"
        systemversion "latest"
        defines {
            "TL_PLATFORM_WINDOWS",
        }

        postbuildcommands {
            "{COPY} %{wks.location}/build/bin/" .. outputdir .. "/%{prj.name}/Engine.dll %{wks.location}/build/bin/" .. outputdir .. "/Editor/"
        }

    filter "configurations:Debug"
        runtime "Debug"
        symbols "on"
        defines {
            "TELEIOS_DEBUG",
        }

    filter "configurations:Release"
        runtime "Release"
        optimize "on"

        defines {
            "TELEIOS_RELEASE",
        }

    filter "configurations:Dist"
        runtime "Release"
        optimize "on"

        defines {
            "TELEIOS_DIST",
        }
