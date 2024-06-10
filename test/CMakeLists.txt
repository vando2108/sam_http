# Gather all test source files recursively
file(GLOB TEST_MAIN_SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)
file(GLOB_RECURSE TEST_SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.hpp)

# Add GTest tests target
add_executable(${PROJECT_NAME}_test
  ${TEST_MAIN_SOURCE_FILES}
  ${TEST_SOURCE_FILES}
  ${SOURCE_FILES}
)

# Link GTest and other necessary libraries
target_link_libraries(${PROJECT_NAME}_test
  GTest::GTest
  GTest::Main
  GTest::gmock
  glog::glog
)

# Enable testing
enable_testing()

# Add tests
add_test(${PROJECT_NAME}_UnitTests AllTests COMMAND ${PROJECT_NAME}_test)
