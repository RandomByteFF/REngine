cmake_minimum_required(VERSION 3.10)
project(REngine)

set(Boost_USE_STATIC_LIBS OFF) 
set(Boost_USE_MULTITHREADED ON)  
set(Boost_USE_STATIC_RUNTIME OFF) 
find_package(Boost 1.87.0 REQUIRED COMPONENTS python313)
find_package(Python REQUIRED COMPONENTS Development)

set(SOURCE_FILES
	main.cpp
)

if (Python_Development_FOUND)
	include_directories(${Python_INCLUDE_DIRS})
endif()

if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIR}) 
	add_executable(REngine ${SOURCE_FILES})
	target_compile_definitions(REngine PRIVATE BOOST_ALL_NO_LIB)
    target_link_libraries(REngine ${Boost_LIBRARIES} ${PYTHON_LIB} ${Boost_PYTHON_LIB})
endif()

