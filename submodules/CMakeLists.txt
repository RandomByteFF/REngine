cmake_minimum_required(VERSION 3.16)
project(submodules)
find_package(Vulkan REQUIRED)

set(CMAKE_CXX_STANDARD 17)

set(Imgui_sources
	${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui_draw.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui_tables.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui_widgets.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/imgui/backends/imgui_impl_glfw.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/imgui/backends/imgui_impl_vulkan.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui_demo.cpp
)

set (Imgui_sources ${Imgui_sources} PARENT_SCOPE)

include_directories(
	imgui 
	imgui/backends 
	${Vulkan_INCLUDE_DIR}
)

add_executable(ImGui 
	imgui/imgui_demo.cpp
	imgui/examples/example_glfw_vulkan/main.cpp
	${Imgui_sources}
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
	INTERFACE imgui
	INTERFACE imgui/backends
	INTERFACE toml.hpp
)