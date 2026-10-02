cmake_minimum_required(VERSION 3.21)
project(snake C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

# NFR-7: чистая компиляция — часть определения «собралось»
if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-Wall -Wextra -Wpedantic -Werror)
elseif(MSVC)
    add_compile_options(/W4 /WX)
endif()

# ---------------------------------------------------------------------------
# core: чистая логика, НЕ зависит от raylib (ADR: слои, docs/architecture)
# ---------------------------------------------------------------------------
add_library(core STATIC
    src/core/game.c
    src/core/snake.c
    src/core/food.c
    src/core/rules.c
)
target_include_directories(core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

# ---------------------------------------------------------------------------
# raylib: сначала cmake-конфиг пакета, fallback на pkg-config (MSYS2)
# ---------------------------------------------------------------------------
find_package(raylib CONFIG QUIET)
if(NOT raylib_FOUND)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(RAYLIB REQUIRED IMPORTED_TARGET raylib)
endif()

# ---------------------------------------------------------------------------
# игра: точка входа + слои ввода/рендера/хранения
# ---------------------------------------------------------------------------
add_executable(snake
    src/main.c
    src/input/buffer.c
    src/input/input.c
    src/render/renderer.c
    src/storage/scores.c
)
target_link_libraries(snake PRIVATE core)
if(raylib_FOUND)
    target_link_libraries(snake PRIVATE raylib)
else()
    target_link_libraries(snake PRIVATE PkgConfig::RAYLIB)
endif()

# ---------------------------------------------------------------------------
# тесты: Unity через FetchContent, линкуется только с core (NFR-3)
# ---------------------------------------------------------------------------
include(FetchContent)
FetchContent_Declare(unity
    URL https://github.com/ThrowTheSwitch/Unity/archive/refs/tags/v2.6.1.tar.gz
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE)
FetchContent_MakeAvailable(unity)

# unity.c — сторонний код, его диагностика не наша ответственность
set_source_files_properties(${unity_SOURCE_DIR}/src/unity.c PROPERTIES COMPILE_OPTIONS "-w")

enable_testing()
add_executable(core_tests
    tests/core_tests.c
    src/input/buffer.c
    ${unity_SOURCE_DIR}/src/unity.c
)
target_include_directories(core_tests SYSTEM PRIVATE ${unity_SOURCE_DIR}/src)
target_link_libraries(core_tests PRIVATE core)
add_test(NAME core_tests COMMAND core_tests)
