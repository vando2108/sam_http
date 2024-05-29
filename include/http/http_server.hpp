#include <glog/logging.h>

#include <string_view>

namespace sam {
namespace http_server {

class IHttpServer {
 public:
  IHttpServer(std::string_view, size_t);

 public:
  virtual void start();
};

class HttpServer {};

}  // namespace http_server
}  // namespace sam
