
add_library(loader
	shader.cpp
	obj.cpp
	image.cpp
)
#FIXME: quickfix
target_link_libraries(loader PRIVATE
	submodules
)