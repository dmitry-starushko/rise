cmake_minimum_required(VERSION 3.30)
project(rise VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -march=x86-64-v2")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -mtune=generic")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -msse4.2")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -funroll-loops")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -ffast-math")
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELEASE ON)

set(PROFILE_DIR "${CMAKE_BINARY_DIR}/profiles")

option(PROF_GEN "Generate profiling information" OFF)
option(PROF_USE "Use profiling information" OFF)

if(PROF_GEN)
    add_compile_options(-fprofile-generate -fprofile-dir=${PROFILE_DIR})
    add_link_options(-fprofile-generate -lgcov -fprofile-dir=${PROFILE_DIR})
    message(STATUS "Profile generation enabled in ${PROFILE_DIR}")
endif()

if(PROF_USE)
    add_compile_options(-fprofile-use -fprofile-dir=${PROFILE_DIR})
    add_link_options(-fprofile-use -fprofile-dir=${PROFILE_DIR})
    message(STATUS "Profile usage enabled from ${PROFILE_DIR}")
endif()

add_definitions(-DQT_DISABLE_DEPRECATED_BEFORE=0x060000)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Multimedia)

if(WIN32)
    if(NOT BOOST_ROOT)
        message(FATAL_ERROR "
===============================================================================
Boost not found!
Windows users MUST specify Boost location when running CMake:
    cmake -DBOOST_ROOT=C:/path/to/boost ...
===============================================================================
")
    endif()
    if(NOT EXISTS ${BOOST_ROOT}/boost/version.hpp)
        message(FATAL_ERROR "Boost not found at specified BOOST_ROOT: ${BOOST_ROOT}")
    endif()

    set(Boost_INCLUDE_DIR ${BOOST_ROOT})
    set(Boost_LIBRARY_DIR ${BOOST_ROOT}/lib)
    set(Boost_NO_BOOST_CMAKE ON)

    find_package(Boost REQUIRED)
    message(STATUS "Boost configured successfully from: ${BOOST_ROOT}")
else()
    find_package(Boost REQUIRED CONFIG)
endif()

set(SOURCES
    src/main.cpp
    src/z80cpu.cpp
    src/zxbeeper.cpp
    src/zxbridge.cpp
    src/zxcontrolpanel.cpp
    src/zxjoystick.cpp
    src/zxjoystick_cursor.cpp
    src/zxjoystick_default.cpp
    src/zxjoystick_kempston.cpp
    src/zxjoystick_sinclair_1.cpp
    src/zxjoystick_sinclair_2.cpp
    src/zxkeyboard.cpp
    src/sdlgamepad.cpp
    src/zxmemory.cpp
    src/zxmouse.cpp
    src/zxmouse_inv_kempston.cpp
    src/zxmouse_kempston.cpp
    src/zxmouse_null.cpp
    src/zxports.cpp
    src/zxprinter.cpp
    src/zxprinter_null.cpp
    src/zxs.cpp
    src/zxs_firmware.cpp
    src/zxs_mgc.cpp
    src/zxs_sna.cpp
    src/zxs_z80.cpp
    src/zxtape.cpp
    src/zxtape_tap.cpp
    src/zxtape_tzx.cpp
    src/zxtapeactivator.cpp
    src/zxula.cpp
    src/zxwindow.cpp
)

set(HEADERS
    src/df.h
    src/djrs.h
    src/events.h
    src/forge.h
    src/inline.h
    src/keyscancodes.h
    src/sdlgamepad.h
    src/tabulated.h
    src/tzx.h
    src/z80cpu.h
    src/zxbeeper.h
    src/zxbridge.h
    src/zxcontrolpanel.h
    src/zxdefs.h
    src/zxevents.h
    src/zxjoystick.h
    src/zxjoystick_cursor.h
    src/zxjoystick_default.h
    src/zxjoystick_kempston.h
    src/zxjoystick_sinclair_1.h
    src/zxjoystick_sinclair_2.h
    src/zxkeyboard.h
    src/zxmemory.h
    src/zxmouse.h
    src/zxmouse_inv_kempston.h
    src/zxmouse_kempston.h
    src/zxmouse_null.h
    src/zxports.h
    src/zxprinter.h
    src/zxprinter_null.h
    src/zxs.h
    src/zxtape.h
    src/zxtapeactivator.h
    src/zxula.h
    src/zxutils.h
    src/zxwindow.h
)

set(UIS
    src/zxcontrolpanel.ui
    src/zxwindow.ui
)

set(RESOURCES
    rise.qrc
)

add_executable(${PROJECT_NAME}
    ${SOURCES}
    ${HEADERS}
    ${UIS}
    ${RESOURCES}
)

if(WIN32)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        WIN32_EXECUTABLE TRUE
    )
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Multimedia
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    Boost::headers
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

if(WIN32)
    set(ZSTD_BUILD_STATIC ON CACHE BOOL "Build static libraries" FORCE)
    set(ZSTD_BUILD_SHARED OFF CACHE BOOL "Build shared libraries" FORCE)
    set(ZSTD_BUILD_PROGRAMS OFF CACHE BOOL "Build zstd CLI" FORCE)
    set(ZSTD_LEGACY_SUPPORT OFF CACHE BOOL "Legacy support" FORCE)

    include(FetchContent)
    FetchContent_Declare(
        zstd
        URL https://github.com/facebook/zstd/releases/download/v1.5.7/zstd-1.5.7.tar.gz
        URL_HASH SHA256=eb33e51f49a15e023950cd7825ca74a4a2b43db8354825ac24fc1b7ee09e6fa3
    )

    if(NOT zstd_POPULATED)
        message(STATUS "Downloading and building zstd...")
        FetchContent_Populate(zstd)
        add_subdirectory(${zstd_SOURCE_DIR}/build/cmake ${zstd_BINARY_DIR})
    endif()

    target_link_libraries(${PROJECT_NAME} PRIVATE libzstd_static)
    target_include_directories(${PROJECT_NAME} PRIVATE
        ${zstd_SOURCE_DIR}/lib
        ${zstd_BINARY_DIR}/lib
    )

    message(STATUS "zstd configured for Windows build")
else()
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(ZSTD REQUIRED IMPORTED_TARGET libzstd)
    if(TARGET PkgConfig::ZSTD)
        target_link_libraries(${PROJECT_NAME} PRIVATE PkgConfig::ZSTD)
        message(STATUS "Using system zstd")
    else()
        message(FATAL_ERROR "zstd not found. Please install libzstd-dev")
    endif()
endif()

if(WIN32)
    set(SDL_SHARED OFF CACHE BOOL "Build shared library" FORCE)
    set(SDL_STATIC ON CACHE BOOL "Build static library" FORCE)
    set(SDL_DISABLE_INSTALL ON CACHE BOOL "Disable installation" FORCE)
    set(SDL_VIDEO OFF CACHE BOOL "Disable video subsystem" FORCE)
    set(SDL_AUDIO OFF CACHE BOOL "Disable audio subsystem" FORCE)
    set(SDL_RENDER OFF CACHE BOOL "Disable render subsystem" FORCE)
    set(SDL_FILESYSTEM OFF CACHE BOOL "Disable filesystem subsystem" FORCE)
    set(SDL_LOADSO OFF CACHE BOOL "Disable dynamic library loading" FORCE)
    set(SDL_THREAD OFF CACHE BOOL "Disable thread subsystem" FORCE)
    set(SDL_TIMER OFF CACHE BOOL "Disable timer subsystem" FORCE)
    set(SDL_SENSOR OFF CACHE BOOL "Disable sensor subsystem" FORCE)
    set(SDL_CAMERA OFF CACHE BOOL "Disable camera subsystem" FORCE)
    set(SDL_HAPTIC OFF CACHE BOOL "Disable haptic subsystem" FORCE)
    set(SDL_POWER OFF CACHE BOOL "Disable power management" FORCE)
    set(SDL_LOCALE OFF CACHE BOOL "Disable locale subsystem" FORCE)
    set(SDL_MISC OFF CACHE BOOL "Disable misc subsystem" FORCE)
    set(SDL_JOYSTICK ON CACHE BOOL "Enable joystick subsystem" FORCE)
    set(SDL_HIDAPI ON CACHE BOOL "Enable HIDAPI for modern gamepads" FORCE)
    set(SDL_TESTS OFF CACHE BOOL "Disable tests" FORCE)
    set(SDL_TEST_LIBRARY OFF CACHE BOOL "Disable test library" FORCE)
    set(SDL_XINPUT ON CACHE BOOL "Enable XInput support" FORCE)
    set(SDL_DINPUT ON CACHE BOOL "Enable DirectInput support" FORCE)
    set(SDL_RAWINPUT ON CACHE BOOL "Enable RawInput support" FORCE)
    set(SDL_WASAPI OFF CACHE BOOL "Disable WASAPI audio" FORCE)
    set(SDL_D3D OFF CACHE BOOL "Disable Direct3D" FORCE)
    set(SDL_OPENGL OFF CACHE BOOL "Disable OpenGL" FORCE)
    set(SDL_OPENGLES OFF CACHE BOOL "Disable OpenGL ES" FORCE)

    include(FetchContent)
    FetchContent_Declare(
        sdl3
        URL https://github.com/libsdl-org/SDL/releases/download/release-3.4.2/SDL3-3.4.2.tar.gz
        URL_HASH SHA256=ef39a2e3f9a8a78296c40da701967dd1b0d0d6e267e483863ce70f8a03b4050c
    )

    if(NOT sdl3_POPULATED)
        message(STATUS "Downloading and building SDL3 (joystick-only)...")
        FetchContent_Populate(sdl3)
        add_subdirectory(${sdl3_SOURCE_DIR} ${sdl3_BINARY_DIR} EXCLUDE_FROM_ALL)
    endif()

    target_link_libraries(${PROJECT_NAME} PRIVATE SDL3::SDL3-static)

    target_include_directories(${PROJECT_NAME} PRIVATE
        ${sdl3_SOURCE_DIR}/include
        ${sdl3_BINARY_DIR}/include-config
    )

    target_link_libraries(${PROJECT_NAME} PRIVATE
        setupapi
        hid
        winmm
    )

    target_compile_definitions(${PROJECT_NAME} PRIVATE SDL_STATIC_LIB)
    message(STATUS "SDL3 configured for Windows (joystick-only, static)")
else()
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(SDL3 REQUIRED IMPORTED_TARGET sdl3)

    if(TARGET PkgConfig::SDL3)
        target_link_libraries(${PROJECT_NAME} PRIVATE PkgConfig::SDL3)
        message(STATUS "Using system SDL3")
    else()
        message(FATAL_ERROR "SDL3 not found. Please install libsdl3-dev")
    endif()
endif()

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)