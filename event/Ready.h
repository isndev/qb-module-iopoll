
#include "Base.h"

#ifndef QB_MODULE_IOPOLL_EVENT_READY_H
#define QB_MODULE_IOPOLL_EVENT_READY_H

namespace qbm {
    namespace iopoll {
        namespace event {
            // output events
            struct Ready
                    : public Event
                    , public Proxy {

                Ready() = delete;
                Ready(Proxy const &proxy)
                        : Proxy(proxy)
                {}
                Ready(network::epoll::Proxy const &proxy, epoll_event const &event)
                        : Proxy(proxy) {
                    ep_event = event;
                }
            };
        }
    }
}

#endif //QB_MODULE_IOPOLL_EVENT_READY_H
