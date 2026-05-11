cmake = import('cmake')

glfw = cmake.subproject('glfw')
glfw_dep = glfw.dependency('glfw')
lua_dep = dependency('lua5.4')
sol2_dep = dependency('sol2', fallback: ['sol2', 'sol2_dep'])
submodules_args = [
	'-DSOL_ALL_SAFETIES_ON=1',
]

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
	'../subprojects/sol2/include',
)

submodules_dep = declare_dependency(
	dependencies: [imgui_dep, glfw_dep, lua_dep, sol2_dep],
	include_directories: submodules_inc
)