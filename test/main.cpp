#include <gtest/gtest.h>

#include "./data_structure/scsp_lockfree_queue_tests.hpp"
#include "./data_structure/unique_list_tests.hpp"
#include "./threadpool/centralized_threadpool_tests.hpp"

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
