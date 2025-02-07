cmake_minimum_required(VERSION 3.10)
project(REngine)
# set(BOOST_ROOT "C:/Users/Bit/bin/boost")

set(Boost_USE_STATIC_LIBS OFF) 
set(Boost_USE_MULTITHREADED ON)  
set(Boost_USE_STATIC_RUNTIME OFF) 
find_package(Boost 1.87.0 REQUIRED COMPONENTS python313)

set(SOURCE_FILES
	main.cpp
)

if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIR}) 
	add_executable(REngine ${SOURCE_FILES})
	include_directories("C:/Program Files/Python313/include")
	set(Boost_PYTHON_LIB "C:/local/boost_1_87_0/stage/lib/boost_python313-vc143-mt-gd-x64-1_87.lib")
    target_link_libraries(REngine ${Boost_LIBRARIES} ${PYTHON_LIB} ${Boost_PYTHON_LIB})
endif()

