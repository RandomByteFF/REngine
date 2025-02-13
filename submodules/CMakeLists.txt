cmake_minimum_required(VERSION 3.10)
project(submodules)

set(ImguiSources
	imgui/imgui_draw.cpp
	imgui/imgui_tables.cpp
	imgui/imgui_widgets.cpp
	imgui/imgui.cpp
	imgui/backends/imgui_impl_glfw.cpp
	imgui/backends/imgui_impl_vulkan.cpp
)

include_directories(imgui imgui/backends)

add_executable(ImGui 
	imgui/imgui_demo.cpp
	imgui/examples/example_glfw_vulkan/main.cpp
	${ImguiSources}
)

target_link_libraries(
	ImGui
	glfw # Use the target found by find_package if you used it
    vulkan # Best practice if you are using FindVulkan.cmake
    dl
    pthread
    X11
    Xxf86vm
    Xrandr
    Xi
)