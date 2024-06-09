#include <gtest/gtest.h>

#include "./data_structure/scsp_lockfree_queue_test.hpp"
#include "./data_structure/unique_list.hpp"

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
