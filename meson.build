project('REngine', 'cpp', 
	version: '0.1', 
	default_options: ['cpp_std=c++20']
)

subdir('core')
subdir('editor')
subdir('input')
subdir('loader')
subdir('scene')
subdir('submodules')

sources = [
	core_src,
	editor_src,
	input_src,
	loader_src,
	scene_src,
]

includes = include_directories(
	'.',
	'pch',
)

definitions = [
	'-DGLM_ENABLE_EXPERIMENTAL',
	'-DGLM_FORCE_RADIANS',
	'-DGLM_FORCE_DEPTH_ZERO_TO_ONE',
]

if get_option('editor')
	definitions += '-DEDITOR'
endif

dependencies = [
	dependency('vulkan'),
	submodules_dep
]

executable('REngine', 
	sources: ['main.cpp'] + sources, 
	include_directories: includes, 
	dependencies: dependencies,
	cpp_pch: 'pch/headers.h',
	cpp_args: definitions
)