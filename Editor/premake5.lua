project "Editor"
    kind "ConsoleApp"
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
        "TELEIOS_EXPORT",
    }

    undefines {
        "UNICODE",
        "_UNICODE",
    }

    includedirs {
        "src",
        "vendor",
        "%{wks.location}/Engine/src",
        "%{wks.location}/Engine/vendor",
    }

    links {
        "Engine"
    }

    filter "system:windows"
        systemversion "latest"
        defines {
            "TL_PLATFORM_WINDOWS",
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
