#pragma once
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/strand.hpp>
#include <memory>
#include <vector>

#include "../model/dog.h"
#include "../model/map.h"
#include "../other/tagged.h"
#include "../other/utils.h"

namespace model {

namespace net = boost::asio;

class GameSession {
 public:
  using SessionStrand = net::strand<net::io_context::executor_type>;
  using Id = util::Tagged<std::string, GameSession>;

  GameSession(std::shared_ptr<model::Map> map, net::io_context& ioc)
      : map_(map),
        strand_(std::make_shared<SessionStrand>(net::make_strand(ioc))),
        id_(*(map->GetId())) {};

  const Id& GetId() const noexcept;
  const std::shared_ptr<model::Map> GetMap();
  std::shared_ptr<SessionStrand> GetStrand();

 private:
  std::shared_ptr<model::Map> map_;
  std::shared_ptr<SessionStrand> strand_;
  Id id_;
};

}  // namespace model