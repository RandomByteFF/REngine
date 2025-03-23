add_library(core
	store.cpp
	windowManager.cpp
	instance.cpp
	queueFamily.cpp
	swapchain.cpp
	image.cpp
	buffer.cpp
	pipeline.cpp
	descriptorPool.cpp
	renderer.cpp
	commandBuffer.cpp
	time.cpp
	runner.cpp
	camera.cpp
)
add_subdirectory(drawable)

target_link_libraries(core PRIVATE
	submodules
	drawable
)

target_precompile_headers(core PRIVATE
	headers.h
)