# Gather all benchmark source files
file(GLOB BENCHMARK_MAIN_SOURCE_FILE ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)
file(GLOB_RECURSE BENCHMARK_SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.hpp)

# Add benchmark executable
add_executable(${PROJECT_NAME}_benchmark
  ${BENCHMARK_MAIN_SOURCE_FILE}
  ${BENCHMARK_SOURCE_FILES}
  ${SOURCE_FILES}  # Link all source files to the benchmark executable
)

# Link Benchmark and other necessary libraries
target_link_libraries(${PROJECT_NAME}_benchmark
  benchmark::benchmark
  glog::glog
)

# Link Google Benchmark
find_package(benchmark REQUIRED)
target_link_libraries(${PROJECT_NAME}_benchmark benchmark::benchmark)
