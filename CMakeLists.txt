cmake_minimum_required(VERSION 3.16)

# SET
set (CMAKE_CXX_STANDARD 20)
set (CMAKE_EXPORT_COMPILE_COMMANDS ON)
set (CMAKE_TOOLCHAIN_FILE ./build/conan_toolchain.cmake)

# BUILD OPTIONS
add_compile_options(-fsanitize=address)
add_link_options(-fsanitize=address)

project(sam_http VERSION 0.1)

# FIND PACKAGE
find_package(glog 0.7.0 REQUIRED)
# find_package(absl REQUIRED)

# MAIN SOURCE FILE
file (GLOB MAIN_SOURCE_FILE ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp)

# HTTP SOURCE FILES
file(GLOB HTTP_HEADER_FILES ${CMAKE_CURRENT_SOURCE_DIR}/include/http/*.hpp)
file(GLOB HTTP_SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/http/*.cpp)

# SOCKET SOURCE FILES
file(GLOB SOCKET_HEADER_FILES ${CMAKE_CURRENT_SOURCE_DIR}/include/socket/*.hpp)
file(GLOB SOCKET_SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/socket/*.cpp)

# EVENT SOURCE FILES
file(GLOB EVENT_HEADER_FILES ${CMAKE_CURRENT_SOURCE_DIR}/include/event/*.hpp)
file(GLOB EVENT_SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/event/*.cpp)

# DATA_STRUCTURE SOURCE FILES
file(GLOB DATA_STRUCTURE_HEADER_FILES ${CMAKE_CURRENT_SOURCE_DIR}/include/data_structure/*.hpp)
file(GLOB DATA_STRUCTURE_SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/data_structure/*.cpp)

# Include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include/http
    ${CMAKE_CURRENT_SOURCE_DIR}/include/socket
    ${CMAKE_CURRENT_SOURCE_DIR}/include/event
    ${CMAKE_CURRENT_SOURCE_DIR}/include/data_structure
)

# Create target executable
add_executable(${PROJECT_NAME}
    ${MAIN_SOURCE_FILE}
    ${HTTP_SOURCE_FILES}
    ${HTTP_HEADER_FILES}
    ${SOCKET_SOURCE_FILES}
    ${SOCKET_HEADER_FILES}
    ${EVENT_SOURCE_FILES}
    ${EVENT_HEADER_FILES}
    ${DATA_STRUCTURE_SOURCE_FILES}
    ${DATA_STRUCTURE_HEADER_FILES}
)

# LINK
target_link_libraries(${PROJECT_NAME} glog::glog)
