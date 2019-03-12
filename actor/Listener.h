//
// Created by isndev on 7/8/18.
//

#include <algorithm>
#include <vector>
#include <exception>

#include "BaseRoutine.h"
#include "../service/tag.h"

#ifndef QB_MODULE_IOPOLL_ACTOR_LISTENER_H
#define QB_MODULE_IOPOLL_ACTOR_LISTENER_H

namespace qbm {
    namespace iopoll {
        namespace actor {

            // example trait to implement
            struct ExampleTrait {

                bool onInitialize();
                void onConnect(network::tcp::Socket event);
            };

            template <typename Derived>
            class Listener
                    : public BaseRoutine<Listener<Derived>> {
            public:
                constexpr static const Type type = Type::READ;
                constexpr static const bool has_keepalive = false;
            protected:
                const qb::ActorId       _io_id;
                network::tcp::Listener       _listener;

            protected:
                inline network::tcp::Listener &getListener() { return _listener; }
            public:
                Listener() = delete;
                Listener(uint8_t const coreId,
                         unsigned short port,
                         network::ip ip = network::ip::Any)
                    : _io_id(this->template getServiceId<service::Tag>(coreId))
                {
                    _listener.listen(port, ip);
                    _listener.setBlocking(false);
                    if (_listener.isBlocking())
                        throw std::runtime_error("failed to set blocking socket listener");
                }

                bool onInitialize() {
                    if (!_listener.good())
                        return false;
                    auto &e = this->template push<event::Subscribe>(_io_id);
                    e.setEvents(EPOLLIN | EPOLLONESHOT);
                    e.setHandle(getListener().raw());
                    LOG_DEBUG << "PRE subscribed=" << e.getHandle()
                              << " id=" << e.getOwner()
                              << "events=" << e.getEvents();
                    return static_cast<Derived &>(*this).onInitialize();
                }

                ReturnValue onRead(event::Ready const &event) {
                    network::tcp::Socket socket;

                    if (_listener.accept(socket) == network::SocketStatus::Done) {
                        static_cast<Derived &>(*this).onConnect(socket.raw());
                        LOG_INFO << "Accepted new connection";
                    } else {
                        LOG_WARN << "Failed to accept new connection" << _listener.raw();
                    }

                    return ReturnValue::REPOLL;
                }

                ReturnValue onDisconnect(event::Ready const &) {
                    LOG_CRIT << "Actor listener is down";
                    this->kill();
                    return ReturnValue::KO;
                }
            };

        }
    }
}

#endif //QB_MODULE_IOPOLL_ACTOR_LISTENER_H