workspace "Teleios-Engine"
    architecture "x86_64"
    startproject "Editor"

    configurations {
        "Debug",
        "Release",
        "Dist"
    }

    outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

    include "Editor"
    include "Engine"
