cmake_minimum_required(VERSION 3.17)
project(2048 C)

set(CMAKE_C_STANDARD 11)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")
set(CMAKE_LINKER_FLAGS_DEBUG "${CMAKE_LINKER_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")

set(CMAKE_EXE_LINKER_FLAGS_INIT "-fsanitize=address -fno-omit-frame-pointer")

if (WIN32)
    set(SDL2MAIN_LIBRARY "${CMAKE_SOURCE_DIR}/libs/SDL2/x86_64-w64-mingw32/lib/libSDL2main.a")

    set(SDL2_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/libs/SDL2/x86_64-w64-mingw32/include/SDL2")
    set(SDL2_LIBRARY "${CMAKE_SOURCE_DIR}/libs/SDL2/x86_64-w64-mingw32/lib/libSDL2.dll.a")

    set(SDL2_TTF_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/libs/SDL2_ttf/x86_64-w64-mingw32/include/SDL2")
    set(SDL2_TTF_LIBRARY "${CMAKE_SOURCE_DIR}/libs/SDL2_ttf/x86_64-w64-mingw32/lib/libSDL2_ttf.dll.a")

    file(COPY
            "${CMAKE_SOURCE_DIR}/libs/SDL2/x86_64-w64-mingw32/bin/SDL2.dll"
            "${CMAKE_SOURCE_DIR}/libs/SDL2_ttf/x86_64-w64-mingw32/bin/SDL2_ttf.dll"
            "${CMAKE_SOURCE_DIR}/libs/SDL2_ttf/x86_64-w64-mingw32/bin/libfreetype-6.dll"
            "${CMAKE_SOURCE_DIR}/libs/SDL2_ttf/x86_64-w64-mingw32/bin/zlib1.dll"
            DESTINATION ${CMAKE_BINARY_DIR})
else()
    set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")
    list(APPEND CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/libs/sanitizers/cmake")

    set(SDL2MAIN_LIBRARY "")

    find_package(SDL2 REQUIRED)
    find_package(SDL2_ttf REQUIRED)
    find_package(Sanitizers REQUIRED)
endif ()

include_directories(${SDL2_INCLUDE_DIR} ${SDL2_TTF_INCLUDE_DIR})

file(GLOB SOURCES "src/*.c" "src/*.h")
file(GLOB GFX_SOURCES "src/gfx/*.c" "src/gfx/*.h")
add_executable(${PROJECT_NAME} ${SOURCES} ${GFX_SOURCES})
target_link_libraries(${PROJECT_NAME}
        ${SDL2MAIN_LIBRARY}
        ${SDL2_LIBRARY}
        ${SDL2_TTF_LIBRARY})
