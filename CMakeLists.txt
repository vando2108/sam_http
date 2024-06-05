cmake_minimum_required(VERSION 3.22.6)
project(sam_http VERSION 0.1)

message("1. Set build options")
set (CMAKE_CXX_STANDARD 20)
set (CMAKE_EXPORT_COMPILE_COMMANDS ON)
set (CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/build/conan_toolchain.cmake)
message(${CMAKE_TOOLCHAIN_FILE})

message("2. Include external libraries")
include(cmake/options.cmake)
include(cmake/dependencies.cmake)

message("3. Load source and header files")
# MAIN SOURCE FILE
file (GLOB MAIN_SOURCE_FILE ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)

# ALL HEADER AND SOURCE FILES
file(GLOB_RECURSE HEADER_FILES ${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp)
file(GLOB_RECURSE SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

message("4. Include directories")
# Include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

message("5. Add main executable")
# Create target executable
add_executable(${PROJECT_NAME}
    ${MAIN_SOURCE_FILE}
    ${HEADER_FILES}
    ${SOURCE_FILES}
)

# LINK
target_link_libraries(${PROJECT_NAME} glog::glog)

message("6. Add add_subdirectory")
add_subdirectory(test)
add_subdirectory(benchmark)
