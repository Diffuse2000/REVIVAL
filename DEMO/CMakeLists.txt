set(PROJECT_NAME DEMO)

find_package(SDL2 REQUIRED)

################################################################################
# Source groups
################################################################################
set(no_group_source_files
    "CHASE.CPP"
    "CHASE.H"
    "CITY.CPP"
    "CITY.H"
    "Config.cpp"
    "Config.h"
    "CRASH.CPP"
    "CRASH.H"
    "CREDITS.CPP"
    "CREDITS.H"
    "END.CPP"
#    "FillerTest.cpp"
    "FillerTest.h"
    "FOUNTAIN.CPP"
    "FOUNTAIN.H"
    # "Gdi.cpp"
    # "Gdi.h"
    "Glat.cpp"
    "GLAT.H"
    "GREETS.CPP"
    "GREETS.H"
    "ImageCompression.cpp"
    "ImageCompression.h"
#    "Koch.cpp"
#    "Koch.h"
#    "Nova.cpp"
#    "Nova.h"
    "PhotonTracer.cpp"
    "PhotonTracer.h"
    "Raytracer.cpp"
    "Raytracer.h"
    "REV.CPP"
    "Rev.h"
    "SDL2.cpp"
    "SDL2.h"
    "Win32Display.h"
)
source_group("" FILES ${no_group_source_files})

set(ALL_FILES
    ${no_group_source_files}
)

################################################################################
# Target
################################################################################
add_executable(${PROJECT_NAME} ${ALL_FILES})

use_props(${PROJECT_NAME} "${CMAKE_CONFIGURATION_TYPES}" "${DEFAULT_CXX_PROPS}")
set(ROOT_NAMESPACE DEMO)

################################################################################
# Target name
################################################################################
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Win32")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        TARGET_NAME_DEBUG   "DEMO"
        TARGET_NAME_LLVM    "${TARGET_NAME}$ENV{TargetExt}"
        TARGET_NAME_RELEASE "${TARGET_NAME}$ENV{TargetExt}"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        TARGET_NAME_DEBUG   "DEMO"
        TARGET_NAME_LLVM    "${TARGET_NAME}$ENV{TargetExt}"
        TARGET_NAME_RELEASE "${TARGET_NAME}$ENV{TargetExt}"
    )
endif()
################################################################################
# Output directory
################################################################################
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Win32")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        OUTPUT_DIRECTORY_DEBUG   "${CMAKE_CURRENT_SOURCE_DIR}/./Debug"
        OUTPUT_DIRECTORY_LLVM    "${CMAKE_SOURCE_DIR}/${CMAKE_VS_PLATFORM_NAME}/$<CONFIG>"
        OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/${CMAKE_VS_PLATFORM_NAME}/$<CONFIG>"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        OUTPUT_DIRECTORY_DEBUG   "${CMAKE_CURRENT_SOURCE_DIR}/./Debug"
        OUTPUT_DIRECTORY_LLVM    "${CMAKE_SOURCE_DIR}/${CMAKE_VS_PLATFORM_NAME}/$<CONFIG>"
        OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/${CMAKE_VS_PLATFORM_NAME}/$<CONFIG>"
    )
endif()
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Win32")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        PDB_OUTPUT_DIRECTORY_DEBUG   "${CMAKE_CURRENT_SOURCE_DIR}/./Debug"
        PDB_OUTPUT_DIRECTORY_LLVM    "${CMAKE_CURRENT_SOURCE_DIR}/./llvm"
        PDB_OUTPUT_DIRECTORY_RELEASE "${OUTPUT_DIRECTORY}"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        PDB_OUTPUT_DIRECTORY_DEBUG   "${CMAKE_CURRENT_SOURCE_DIR}/./Debug"
        PDB_OUTPUT_DIRECTORY_LLVM    "${CMAKE_CURRENT_SOURCE_DIR}/./llvm"
        PDB_OUTPUT_DIRECTORY_RELEASE "${OUTPUT_DIRECTORY}"
    )
endif()
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Win32")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        INTERPROCEDURAL_OPTIMIZATION_LLVM    "FALSE"
        INTERPROCEDURAL_OPTIMIZATION_RELEASE "TRUE"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        INTERPROCEDURAL_OPTIMIZATION_LLVM    "FALSE"
        INTERPROCEDURAL_OPTIMIZATION_RELEASE "TRUE"
    )
endif()
################################################################################
# MSVC runtime library
################################################################################
get_property(MSVC_RUNTIME_LIBRARY_DEFAULT TARGET ${PROJECT_NAME} PROPERTY MSVC_RUNTIME_LIBRARY)
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Win32")
    string(CONCAT "MSVC_RUNTIME_LIBRARY_STR"
        $<$<CONFIG:Debug>:
            MultiThreadedDebug
        >
        $<$<CONFIG:llvm>:
            MultiThreaded
        >
        $<$<CONFIG:Release>:
            MultiThreaded
        >
        $<$<NOT:$<OR:$<CONFIG:Debug>,$<CONFIG:llvm>,$<CONFIG:Release>>>:${MSVC_RUNTIME_LIBRARY_DEFAULT}>
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    string(CONCAT "MSVC_RUNTIME_LIBRARY_STR"
        $<$<CONFIG:Debug>:
            MultiThreadedDebug
        >
        $<$<CONFIG:llvm>:
            MultiThreaded
        >
        $<$<CONFIG:Release>:
            MultiThreaded
        >
        $<$<NOT:$<OR:$<CONFIG:Debug>,$<CONFIG:llvm>,$<CONFIG:Release>>>:${MSVC_RUNTIME_LIBRARY_DEFAULT}>
    )
endif()
set_target_properties(${PROJECT_NAME} PROPERTIES MSVC_RUNTIME_LIBRARY ${MSVC_RUNTIME_LIBRARY_STR})

################################################################################
# Include directories
################################################################################
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Win32")
    target_include_directories(${PROJECT_NAME} PUBLIC
        "$<$<CONFIG:Debug>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/Users/gil-ad.benor/Work/REVIVAL/fmod"
        ">"
        "${CMAKE_CURRENT_SOURCE_DIR}/Users/gil-ad.benor/Work/REVIVAL/FDS"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    target_include_directories(${PROJECT_NAME} PUBLIC
        "$<$<CONFIG:Debug>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/Users/gil-ad.benor/Work/REVIVAL/fmod"
        ">"
        "${CMAKE_CURRENT_SOURCE_DIR}/Users/gil-ad.benor/Work/REVIVAL/FDS"
    )
endif()

################################################################################
# Compile definitions
################################################################################
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Win32")
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        "$<$<CONFIG:Debug>:"
            "PORTABLE_CODE=1;"
            "_DEBUG"
        ">"
        "$<$<CONFIG:llvm>:"
            "PORTABLE_CODE=1;"
            "NDEBUG"
        ">"
        "$<$<CONFIG:Release>:"
            "PORTABLE_CODE=1;"
            "NDEBUG"
        ">"
        "_CRT_SECURE_NO_WARNINGS;"
        # "WIN32;"
        "PORTABLE_CODE=1;"
        "_WINDOWS;"
        "_MBCS"
        "SIMDE_ENABLE_NATIVE_ALIASES"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        "$<$<CONFIG:Debug>:"
            "_DEBUG"
        ">"
        "$<$<CONFIG:llvm>:"
            "NDEBUG"
        ">"
        "$<$<CONFIG:Release>:"
            "NDEBUG"
        ">"
        "PORTABLE_CODE=1;"
        "_CRT_SECURE_NO_WARNINGS;"
        # "WIN32;"
        "_WINDOWS;"
        "_MBCS"
        "SIMDE_ENABLE_NATIVE_ALIASES"
    )
endif()

################################################################################
# Compile and link options
################################################################################
if(MSVC)
    if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Win32")
        target_compile_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Debug>:
                /Od;
                /RTC1;
                /arch:SSE2;
                /Oy-;
                /fp:except;
                /ZI;
                /fp:strict;
                /EHa;
                /GS
            >
            $<$<CONFIG:llvm>:
                /GL;
                /Ob2;
                /Oi;
                /GF;
                /arch:AVX;
                /Oy;
                /Gy;
                /W3;
                /fp:except-;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
                /Ot;
                /TP;
                /fp:fast;
                /J;
                ${DEFAULT_CXX_EXCEPTION_HANDLING};
                /GS-
            >
            $<$<CONFIG:Release>:
                /GL;
                /Ob2;
                /Oi;
                /sdl-;
                /GF;
                /arch:AVX;
                /Gy-;
                /W3;
                /fp:except-;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
                /Ot;
                /fp:fast;
                /GR-;
                /J;
                /GS-
            >
            /MP;
            /std:c++latest;
            /Y-
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
        target_compile_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Debug>:
                /Od;
                /RTC1;
                /arch:SSE2;
                /Oy-;
                /fp:except;
                /ZI;
                /fp:strict;
                /EHa;
                /GS
            >
            $<$<CONFIG:llvm>:
                /GL;
                /Ob2;
                /Oi;
                /GF;
                /arch:AVX;
                /Oy;
                /Gy;
                /W3;
                /fp:except-;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
                /Ot;
                /TP;
                /fp:fast;
                /J;
                ${DEFAULT_CXX_EXCEPTION_HANDLING};
                /GS-
            >
            $<$<CONFIG:Release>:
                /GL;
                /Ob2;
                /Oi;
                /sdl-;
                /GF;
                /arch:AVX;
                /Gy-;
                /W3;
                /fp:except-;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
                /Ot;
                /fp:fast;
                /GR-;
                /J;
                /GS-
            >
            /MP;
            /std:c++latest;
            /Y-
        )
    endif()
    if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Win32")
        target_link_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Debug>:
                /NODEFAULTLIB:libci.lib;
                /DEBUG;
                /INCREMENTAL
            >
            $<$<CONFIG:llvm>:
                /NODEFAULTLIB:LIBC;
                /NODEFAULTLIB:LIBCI;
                /DEBUG:FULL;
                /INCREMENTAL:NO
            >
            $<$<CONFIG:Release>:
                /NODEFAULTLIB:LIBC;
                /NODEFAULTLIB:LIBCI;
                /DEBUG:FULL;
                /LTCG;
                /PROFILE;
                /INCREMENTAL:NO
            >
            /NOLOGO;
            /SAFESEH:NO;
            /SUBSYSTEM:WINDOWS;
            /MACHINE:I386
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
        target_link_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Debug>:
                /NODEFAULTLIB:libci.lib;
                /DEBUG;
                /INCREMENTAL;
                /MACHINE:I386
            >
            $<$<CONFIG:llvm>:
                /NODEFAULTLIB:LIBC;
                /NODEFAULTLIB:LIBCI;
                /DEBUG:FULL;
                /MACHINE:X64;
                /INCREMENTAL:NO
            >
            $<$<CONFIG:Release>:
                /NODEFAULTLIB:LIBC;
                /NODEFAULTLIB:LIBCI;
                /DEBUG:FULL;
                /LTCG;
                /PROFILE;
                /INCREMENTAL:NO
            >
            /NOLOGO;
            /SAFESEH:NO;
            /SUBSYSTEM:WINDOWS
        )
    endif()
endif()

################################################################################
# Post build events
################################################################################
# if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Win32")
#     add_custom_command_if(
#         TARGET ${PROJECT_NAME}
#         POST_BUILD
#         COMMANDS
#         COMMAND   $<CONFIG:Debug> cp DEMO/DEMO ../Runtime
#         COMMAND   $<CONFIG:Debug> cp DEMO/DEMO ../Runtime
#         COMMAND    $<CONFIG:llvm> cp $<SHELL_PATH:${OUTPUT_DIRECTORY}>${TARGET_NAME}$ENV{TargetExt} ..\\Runtime
#         COMMAND $<CONFIG:Release> cp $<SHELL_PATH:${OUTPUT_DIRECTORY}>${TARGET_NAME}$ENV{TargetExt} ..\\Runtime
#     )
# elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
#     add_custom_command_if(
#         TARGET ${PROJECT_NAME}
#         POST_BUILD
#         COMMANDS
#         COMMAND   $<CONFIG:Debug> cp $<SHELL_PATH:${OUTPUT_DIRECTORY}>/DEMO ../Runtime
#         COMMAND   $<CONFIG:Debug> cp $<SHELL_PATH:${OUTPUT_DIRECTORY}>/DEMO ../Runtime
#         COMMAND    $<CONFIG:llvm> cp $<SHELL_PATH:${OUTPUT_DIRECTORY}>${TARGET_NAME}$ENV{TargetExt} ..\\Runtime
#         COMMAND $<CONFIG:Release> cp $<SHELL_PATH:${OUTPUT_DIRECTORY}>${TARGET_NAME}$ENV{TargetExt} ..\\Runtime
#     )
# endif()

################################################################################
# Dependencies
################################################################################
# Link with other targets.
target_link_libraries(${PROJECT_NAME} PRIVATE
    FDS
#    Modplayer
)

if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Win32")
    set(ADDITIONAL_LIBRARY_DEPENDENCIES
        "$<$<CONFIG:llvm>:"
            "delayimp;"
            "d3dx8;"
            "ddraw;"
            "dxguid;"
            "d3d8"
        ">"
        "$<$<CONFIG:Release>:"
            "delayimp;"
            "ddraw"
        ">"
        # "winmm;"
        # "legacy_stdio_definitions"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    set(ADDITIONAL_LIBRARY_DEPENDENCIES
        "$<$<CONFIG:llvm>:"
            "delayimp"
        ">"
        "$<$<CONFIG:Release>:"
            "delayimp"
        ">"
        # "winmm;"
        # "legacy_stdio_definitions"
    )
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE "${ADDITIONAL_LIBRARY_DEPENDENCIES}" "${SDL2_LIBRARIES}")
include_directories(${PROJECT_NAME} "${SDL2_INCLUDE_DIRS}")

if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Win32")
    target_link_directories(${PROJECT_NAME} PRIVATE
        "$<$<CONFIG:llvm>:"
            "C:/Projects/REVIVAL"
        ">"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    target_link_directories(${PROJECT_NAME} PRIVATE
        "$<$<CONFIG:llvm>:"
            "C:/Projects/REVIVAL"
        ">"
    )
endif()

