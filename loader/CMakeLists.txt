
add_library(loader
	shader.cpp
	obj.cpp
	image.cpp
)

target_link_libraries(loader PRIVATE
	submodules
)