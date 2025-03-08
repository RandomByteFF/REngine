add_library(core
	store.cpp
	windowManager.cpp
	instance.cpp
	queueFamily.cpp
	swapchain.cpp
)
target_link_libraries(core PRIVATE
	submodules
)

target_precompile_headers(core PUBLIC 
	headers.h
)