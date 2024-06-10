#include <glog/logging.h>
#include <netinet/in.h>
#include <sys/_endian.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cassert>
#include <cstring>

#include "http/http_server.hpp"
#include "socket/stream.hpp"

int main(int argc, char* argv[]) {
  google::InitGoogleLogging(argv[0]);

  // Set log info to console
  FLAGS_alsologtostderr = 1;

  sam::socket::ServerStream server = sam::socket::ServerStream("127.0.0.1", 3000);
  server.start();

  return 0;
}
