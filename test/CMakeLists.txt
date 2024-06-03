# Gather all test source files recursively
file(GLOB_RECURSE TEST_SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

# Add GTest tests target
add_executable(${PROJECT_NAME}_test
  ${TEST_SOURCE_FILES}
  ${SOURCE_FILES}
)

# Link GTest and other necessary libraries
target_link_libraries(${PROJECT_NAME}_test
  GTest::GTest
  GTest::Main
  glog::glog
)

# Enable testing
enable_testing()

# Add tests
add_test(${PROJECT_NAME}_test AllTests COMMAND ${PROJECT_NAME}_test)

# Enable testing
enable_testing()
