cmake_minimum_required(VERSION 3.21)
project(JonklerTanks C)

include(FetchContent)

# ---------- SOURCE & HEADER FiLES -----------------
file(GLOB_RECURSE SRC_FILES "${CMAKE_SOURCE_DIR}/src/*.c")

include_directories(${CMAKE_SOURCE_DIR}/third_party)
add_executable(${PROJECT_NAME} 
               ${SRC_FILES}
               ${CMAKE_SOURCE_DIR}/third_party/log/log.c
)

# -----------------------------------------


# ----------- DIFF OPTS --------------------
option(DEBUG "Enable dEbUg mode" ON)

# for more prettier output in logs and enable all warnings
target_compile_options(JonklerTanks PRIVATE 
                       -fmacro-prefix-map=${CMAKE_SOURCE_DIR}=.
                       -Wall -Wextra
)

if(DEBUG)
    target_compile_definitions(${PROJECT_NAME} PRIVATE DEBUG)
endif()

if(UNIX AND NOT APPLE)
    target_compile_definitions(${PROJECT_NAME} PRIVATE LOG_USE_COLOR)
endif()

set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}"
)

set(SDL_THREAD ON)

# ---------------------------------------

# ----------- EXTERNAL LiBRARiES ----------
find_package(Threads)

find_package(SDL2)
if(NOT SDL2_FOUND)
    message(STATUS "downloading sdl2")
    FetchContent_Declare(
        sdl2
        GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
        GIT_TAG        release-2.32.10
        GIT_SHALLOW    TRUE
    )
    FetchContent_MakeAvailable(sdl2)
endif()

find_package(SDL2_image)
if(NOT SDL2_image_FOUND)
    message(STATUS "downloading sdl2_image")
    FetchContent_Declare(
        sdl2_image
        GIT_REPOSITORY https://github.com/libsdl-org/SDL_image.git
        GIT_TAG        release-2.8.8
        GIT_SHALLOW    TRUE
    )
    FetchContent_MakeAvailable(sdl2_image)
endif()

find_package(SDL2_ttf)
if(NOT SDL2_ttf_FOUND)
    message(STATUS "downloading sdl2_ttf")
    FetchContent_Declare(
        sdl2_image
        GIT_REPOSITORY https://github.com/libsdl-org/SDL_ttf.git
        GIT_TAG        release-2.24.0
        GIT_SHALLOW    TRUE
    )
    FetchContent_MakeAvailable(sdl2_ttf)
endif()

find_package(SDL2_mixer)
if(NOT SDL2_mixer_FOUND)
    message(STATUS "downloading sdl2_mixer")
    FetchContent_Declare(
        sdl2_image
        GIT_REPOSITORY https://github.com/libsdl-org/SDL_mixer.git
        GIT_TAG        release-2.8.1
        GIT_SHALLOW    TRUE
    )
    FetchContent_MakeAvailable(sdl2_mixer)
endif()
# -----------------------------------------

target_link_libraries(${PROJECT_NAME} PRIVATE 
    SDL2::SDL2
    SDL2_image::SDL2_image
    SDL2_ttf::SDL2_ttf
    SDL2_mixer::SDL2_mixer
    Threads::Threads
    m # math
)
