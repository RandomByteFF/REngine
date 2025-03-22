add_library(core
	store.cpp
	windowManager.cpp
	instance.cpp
	queueFamily.cpp
	swapchain.cpp
	image.cpp
	queue.cpp
	buffer.cpp
	pipeline.cpp
	descriptorPool.cpp
	mesh.cpp
	renderer.cpp
	commandBuffer.cpp
	time.cpp
	runner.cpp
)
target_link_libraries(core PRIVATE
	submodules
)

target_precompile_headers(core PUBLIC 
	headers.h
)