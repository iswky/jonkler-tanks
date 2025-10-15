cmake_minimum_required(VERSION 3.21)
project(JonklerTanks C)

find_package(PkgConfig REQUIRED)

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

pkg_check_modules(SDL2 REQUIRED sdl2)
pkg_check_modules(SDL2_IMAGE REQUIRED SDL2_image)
pkg_check_modules(SDL2_TTF REQUIRED SDL2_ttf)
pkg_check_modules(SDL2_MIXER REQUIRED SDL2_mixer)
# -----------------------------------------

target_link_libraries(${PROJECT_NAME} PRIVATE 
    ${SDL2_LIBRARIES}
    ${SDL2_IMAGE_LIBRARIES}
    ${SDL2_TTF_LIBRARIES}
    ${SDL2_MIXER_LIBRARIES}
    Threads::Threads
    m # math
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${SDL2_INCLUDE_DIRS}
    ${SDL2_IMAGE_INCLUDE_DIRS}
    ${SDL2_TTF_INCLUDE_DIRS}
    ${SDL2_MIXER_INCLUDE_DIRS}
)

target_compile_options(${PROJECT_NAME} PRIVATE
    ${SDL2_CFLAGS_OTHER}
    ${SDL2_IMAGE_CFLAGS_OTHER}
    ${SDL2_TTF_CFLAGS_OTHER}
    ${SDL2_MIXER_CFLAGS_OTHER}
)