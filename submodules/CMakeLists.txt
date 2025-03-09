cmake_minimum_required(VERSION 3.16)
project(submodules)
find_package(Vulkan REQUIRED)

set(CMAKE_CXX_STANDARD 17)

set(ImguiSources
	imgui/imgui_draw.cpp
	imgui/imgui_tables.cpp
	imgui/imgui_widgets.cpp
	imgui/imgui.cpp
	imgui/backends/imgui_impl_glfw.cpp
	imgui/backends/imgui_impl_vulkan.cpp
)

include_directories(
	imgui 
	imgui/backends 
	${Vulkan_INCLUDE_DIR}
)

add_executable(ImGui 
	imgui/imgui_demo.cpp
	imgui/examples/example_glfw_vulkan/main.cpp
	${ImguiSources}
)

add_library(submodules INTERFACE)

if (NOT MSVC)
	target_link_libraries(
		ImGui
		dl
		pthread
		X11
		Xxf86vm
		Xrandr
		Xi
)
endif()
target_link_libraries(
	ImGui
	glfw 
	${Vulkan_LIBRARY}
)

add_subdirectory(glfw)
add_subdirectory(VulkanMemoryAllocator)
target_include_directories(submodules
	INTERFACE stb
	INTERFACE tinyobjloader
	INTERFACE glfw/include
	INTERFACE glm
	INTERFACE VulkanMemoryAllocator/include
)