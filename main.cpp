#include <glog/logging.h>
#include <netinet/in.h>
#include <sys/_endian.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cassert>
#include <cstring>

#include "../include/http/http_server.hpp"
#include "utils/rand.hpp"

int main(int argc, char* argv[]) {
  google::InitGoogleLogging(argv[0]);

  // Set log info to console
  FLAGS_alsologtostderr = 1;

  // sam::socket::ServerStream server = sam::socket::ServerStream("127.0.0.1", 3000);
  // server.start();

  auto temp = sam::utils::rand::rand_list_int(10, 10);
  for (auto it : temp) {
    LOG(INFO) << it << ' ';
  }

  return 0;
}
