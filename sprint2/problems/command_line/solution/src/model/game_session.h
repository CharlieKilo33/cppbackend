#pragma once
#include "../model/map.h"
#include "../model/dog.h"
#include "../other/tagged.h"
#include "../other/utils.h"

#include <vector>
#include <memory>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/strand.hpp>

namespace app {

    namespace net = boost::asio;

    class GameSession {
    public:
        using SessionStrand = net::strand<net::io_context::executor_type>;
        using Id = util::Tagged<std::string, GameSession>;

        GameSession(std::shared_ptr<model::Map> map, net::io_context& ioc) :
            map_(map),
            strand_(std::make_shared<SessionStrand>(net::make_strand(ioc))),
            id_(*(map->GetId())) {};

        /*Геттеры на айди, мар и стренд*/
        const Id& GetId() const noexcept;
        const std::shared_ptr<model::Map> GetMap();
        std::shared_ptr<SessionStrand> GetStrand();


    private:
        std::shared_ptr<model::Map> map_;                       //мапа
        std::shared_ptr<SessionStrand> strand_;                 //стренд
        Id id_;                                                 //айди

    };

}