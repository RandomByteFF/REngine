add_library(core
	store.cpp
	windowManager.cpp
)
target_link_libraries(core PRIVATE
	submodules
)