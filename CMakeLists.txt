cmake_minimum_required(VERSION 3.13)


set(PROJECT_NAME dir_choice)
project(${PROJECT_NAME})
set(CMAKE_PREFIX_PATH ${PROJECT_SOURCE_DIR})

# find_package(OpenCV REQUIRED)

# Set up the compiler flags
set(CMAKE_CXX_FLAGS "-g -O3")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(COMPILE_OPTS "")
set(LINK_OPTS "")

# GFlags
add_subdirectory(third-party/gflags)
file(GLOB TARGET_SOURCES
        ${PROJECT_SOURCE_DIR}/src/*.cpp)
message(STATUS "TARGET SOURCES: ${TARGET_SOURCES}")

# wxWidgets
add_subdirectory(third-party/wxWidgets)

# Main executable
add_executable(dir_choice ${TARGET_SOURCES})
target_include_directories(dir_choice PRIVATE ${PROJECT_SOURCE_DIR}/include)
# target_include_directories(dir_choice PRIVATE ${OpenCV_INCLUDE_DIRS})
target_compile_options(dir_choice PRIVATE ${COMPILE_OPTS})
target_link_options(dir_choice PRIVATE ${LINK_OPTS})
# target_link_libraries(dir_choice PRIVATE ${OpenCV_LIBS})
target_link_libraries(dir_choice PRIVATE gflags::gflags wx::core wx::base)

# Tests
add_subdirectory(test)
