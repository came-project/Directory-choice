cmake_minimum_required(VERSION 3.13)

set(PROJECT_NAME template)
project(${PROJECT_NAME})

# find_package(OpenCV REQUIRED)

# Set up the compiler flags
set(CMAKE_CXX_FLAGS "-g -O3")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(COMPILE_OPTS "")
set(LINK_OPTS "")

# GFlags
add_subdirectory(third-party/gflags)

# Main executable
add_executable(template ${PROJECT_SOURCE_DIR}/src/main.cpp)
target_include_directories(template PRIVATE ${PROJECT_SOURCE_DIR}/include)
# target_include_directories(template PRIVATE ${OpenCV_INCLUDE_DIRS})
target_compile_options(template PRIVATE ${COMPILE_OPTS})
target_link_options(template PRIVATE ${LINK_OPTS})
# target_link_libraries(template PRIVATE ${OpenCV_LIBS})
target_link_libraries(template PRIVATE gflags::gflags)

# Tests
add_subdirectory(test)
