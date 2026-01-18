cmake_minimum_required(VERSION 3.25)
project(brew CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(WIN32)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static -static-libgcc -static-libstdc++")
    add_compile_definitions(
        NOMINMAX
        _WIN32_WINNT=0x0A00
        CPPHTTPLIB_OPENSSL_SUPPORT
    )
endif()

include_directories(include)
include_directories(include/external)
include_directories(libs/openssl/include)

set(CLIP_OS_SOURCE include/external/clip_win.cpp)

set(SOURCES
    src/main.cpp
    src/core.cpp
    src/manager.cpp
    include/external/clip.cpp
    ${CLIP_OS_SOURCE}
)

add_executable(brew ${SOURCES})
target_link_libraries(brew PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/libs/openssl/lib-mingw/libssl.a"
    "${CMAKE_CURRENT_SOURCE_DIR}/libs/openssl/lib-mingw/libcrypto.a"
    ws2_32
    crypt32
    shlwapi
    windowscodecs
    bcrypt
    gdi32
    imm32
    ole32
    uuid
    winmm
)