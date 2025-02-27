cmake_minimum_required(VERSION 3.10)
project(REngine)

set (CMAKE_CXX_STANDARD 17)
set(Boost_USE_STATIC_LIBS OFF) 
set(Boost_USE_MULTITHREADED ON)  
set(Boost_USE_STATIC_RUNTIME OFF) 
find_package(Boost 1.87.0 REQUIRED COMPONENTS python313)
find_package(Python REQUIRED COMPONENTS Development)
find_package(Vulkan REQUIRED)

add_subdirectory(submodules)

set(SOURCE_FILES
	main.cpp
)

if (NOT Python_Development_FOUND)
	message(FATAL_ERROR "Python development not found!")
endif()
include_directories(${Python_INCLUDE_DIRS})

if(NOT Boost_FOUND)
	message(FATAL_ERROR "Boost not found!")
endif()

include_directories(${Boost_INCLUDE_DIR} ${Vulkan_INCLUDE_DIR})
add_executable(REngine ${SOURCE_FILES})
target_compile_definitions(REngine PRIVATE BOOST_ALL_NO_LIB)
target_link_libraries(REngine ${Python_LIBRARIES} ${Boost_LIBRARIES}
	glfw
    ${Vulkan_LIBRARY}
    submodules
)
if (NOT MSVC)
    target_link_libraries(
		REngine
        dl
        pthread
        X11
        Xxf86vm
        Xrandr
        Xi
    )
endif()

