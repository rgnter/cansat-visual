cmake_minimum_required(VERSION 3.20)
project(cansat_visual)

find_package(Boost REQUIRED)
add_subdirectory(3rd-party)

add_executable(cansat_visual
    src/main.cpp)
target_compile_features(cansat_visual
    PRIVATE cxx_std_23)
target_link_libraries(cansat_visual
    PRIVATE raylib imgui rlImGui Boost::headers)