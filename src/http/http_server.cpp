#include "../../include/http/http_server.hpp"

namespace sam {
namespace http_server {
IHttpServer::IHttpServer(std::string_view addr, size_t port) { LOG(INFO) << "ttest"; }

void IHttpServer::start() { LOG(INFO) << "Server has started"; }
};  // namespace http_server
}  // namespace sam
