#include <handlers/hello.hpp>

#include <handlers/greeting.hpp>

namespace app {

std::string Hello::HandleRequestThrow(
    const userver::server::http::HttpRequest& request,
    userver::server::request::RequestContext&) const {
  return SayHelloTo(request.GetArg("name"), UserType::kFirstTime);
}

}  // namespace app