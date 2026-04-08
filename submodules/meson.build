cmake = import('cmake')

glfw = cmake.subproject('glfw')
glfw_dep = glfw.dependency('glfw')

imgui_dep = declare_dependency(
	sources: files(
		'imgui/imgui_draw.cpp',
		'imgui/imgui_tables.cpp',
		'imgui/imgui_widgets.cpp',
		'imgui/imgui.cpp',
		'imgui/backends/imgui_impl_glfw.cpp',
		'imgui_impl/imgui_impl_vulkan.cpp',
	),
	include_directories: include_directories(
		'imgui',
		'imgui/backends'
	)
)

submodules_inc = include_directories(
	'stb',
	'tinyobjloader',
	'glm',
	'VulkanMemoryAllocator/include',
	'toml.hpp',
)

submodules_dep = declare_dependency(
	dependencies: [imgui_dep, glfw_dep],
	include_directories: submodules_inc
)