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
	object.cpp
	renderer.cpp
)
target_link_libraries(core PRIVATE
	submodules
)

target_precompile_headers(core PUBLIC 
	headers.h
)