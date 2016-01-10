project("Rock Remakers")

cmake_minimum_required(VERSION 2.8)

SET(GCC_COVERAGE_LINKER_FLAGS "-lGL -lX11 -lXi -lXrandr -lXxf86vm -lXinerama -lXcursor -lrt -lm -ldl -DGLEW_STATIC")
SET(GCC_COVERAGE_COMPILER_FLAGS "-std=c++11 -g -O2")
SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} .)
add_definitions(${GCC_COVERAGE_LINKER_FLAGS})
add_definitions(${GCC_COVERAGE_COMPILER_FLAGS})

find_package(GLEW)
find_package(GLFW)
find_package(OpenGL)

set(INCLUDE_DIRS ${INCLUDE_DIRS} ${GLFW_INCLUDE_DIRS} ${OPENGL_INCLUDE_DIRS} ${GLEW_INCLUDE_PATH} "../../externals/SimpleOpenGLImageLibrary/src" "../../externals/glm")
set(LIBS ${LIBS} ${OPENGL_LIBRARY} ${GLEW_LIBRARIES} ${GLFW_LIBRARIES} X11 -ldl)

link_directories(/usr/lib64)

include_directories(${INCLUDE_DIRS})
add_executable("Rock Remakers" Source.cpp)

target_link_libraries("Rock Remakers" ${LIBS})
