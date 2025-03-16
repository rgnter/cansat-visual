add_subdirectory(raylib)

add_library(imgui
    imgui/imgui.cpp
    imgui/imgui_demo.cpp
    imgui/imgui_draw.cpp
    imgui/imgui_tables.cpp
    imgui/imgui_widgets.cpp)
target_include_directories(imgui
    PUBLIC imgui)

add_library(rlImGui
    rlImGui/rlImGui.cpp)
target_include_directories(imgui
        PUBLIC rlImGui)
target_link_libraries(rlImGui
        PRIVATE imgui raylib)