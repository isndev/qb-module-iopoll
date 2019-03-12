
#include <qb/event.h>
#include <qb/network/epoll.h>
#include <qb/network/tcp.h>
#include <qb/network/udp.h>

#ifndef QB_MODULE_IOPOLL_EVENT_BASE_H
#define QB_MODULE_IOPOLL_EVENT_BASE_H

namespace qbm {
    namespace iopoll {
        using namespace qb;

        struct Handle {
            epoll_event ep_event;

            network::udp::Socket const &udp() const { return *reinterpret_cast<const network::udp::Socket*>(&ep_event.events + 1); }
            network::tcp::Socket const &tcp() const { return *reinterpret_cast<const network::tcp::Socket*>(&ep_event.events + 1); }
            inline void setHandle(int const fd) { ep_event.data.fd = fd; }
            inline void setOwner(uint32_t const id) { *(&ep_event.data.u32 + 1) = id; }
            inline void setEvents(uint32_t const events) { ep_event.events = events; }

            inline int getHandle() const { return ep_event.data.fd; }
            inline uint32_t getOwner() const { return *(&ep_event.data.u32 + 1); }
            inline uint32_t getEvents() const { return ep_event.events; }

        };

        struct Proxy
                : public Handle {
            network::epoll::Proxy proxy;
            Proxy() : proxy(-1) {}
            Proxy(Proxy const &) = default;
            Proxy(network::epoll::Proxy const &proxy)
                    : proxy(proxy) {}

            inline bool receive(void *data, std::size_t size, std::size_t &received) const {
                return tcp().receive(data, size, received) == network::SocketStatus::Done;
            }

            inline bool send(const void *data, std::size_t size, std::size_t &sent) const {
                return tcp().send(data, size, sent) == network::SocketStatus::Done;
            }

            inline int repoll() {
                ep_event.events |= EPOLLONESHOT;
                return proxy.ctl(ep_event);
            }

            inline int disconnect() {
                return proxy.remove(ep_event);
            }
        };

        namespace event {
            struct Base : public Event, public Handle {
            };
        }
    }
}

#endif //QB_MODULE_IOPOLL_EVENT_BASE_H
