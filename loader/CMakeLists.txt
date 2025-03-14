
add_library(loader
	shader.cpp
)
#FIXME: quickfix
target_link_libraries(loader PRIVATE
	submodules
)