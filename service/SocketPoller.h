
#include <qb/actor.h>
#include <qb/network/epoll.h>
#include "../event/all.h"
#include "tag.h"

#ifndef QB_MODULE_IOPOLL_SERVICE_ACTOR_H
#define QB_MODULE_IOPOLL_SERVICE_ACTOR_H

namespace qbm {
    namespace iopoll {
        namespace service {

            class SocketPoller
                    : public qb::ServiceActor<Tag>
                    , public qb::ICallback
            {
                network::epoll::Poller<> _epoll;
            public:
                SocketPoller() = default;

                virtual bool onInit() override final {
                    registerEvent<event::Subscribe>(*this);
                    this->registerCallback(*this);

                    return true;
                }

                virtual void onCallback() override final {
                    _epoll.wait([this](auto &event){
                        push<event::Ready>(*(&event.data.u32 + 1), _epoll, event);
                    });
                }

                void on(event::Subscribe &event) {
                    if (!event.getOwner())
                        event.setOwner(event.getSource());
                    LOG_DEBUG << "subscribed=" << event.getHandle()
                              << " id=" << event.getOwner()
                              << "events=" << event.getEvents();
                    if (_epoll.add(event.ep_event))
                        throw std::runtime_error("failed add");
                }
            };

        }
    }
}

#endif //QB_MODULE_IOPOLL_SERVICE_ACTOR_H
