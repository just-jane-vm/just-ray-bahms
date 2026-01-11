cmake_minimum_required(VERSION 3.16)
project(just-wordcloud LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(jwc src/main.cpp)

# Prefer pkg-config since Cairo configs vary across platforms/distros.
find_package(PkgConfig REQUIRED)

# freetype2 + cairo
pkg_check_modules(FREETYPE2 REQUIRED IMPORTED_TARGET freetype2)
pkg_check_modules(CAIRO REQUIRED IMPORTED_TARGET cairo)

target_link_libraries(jwc
  PRIVATE
    PkgConfig::FREETYPE2
    PkgConfig::CAIRO
)

# Nice-to-have warnings
if (MSVC)
  target_compile_options(jwc PRIVATE /W4)
else()
  target_compile_options(jwc PRIVATE -Wall -Wextra -Wpedantic)
endif()
