/*
 * qb - C++ Actor Framework
 * Copyright (C) 2011-2019 isndev (www.qbaf.io). All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *         http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 *         limitations under the License.
 */

#include <qb/event.h>
#include <qb/io/epoll.h>
#include <qb/io/tcp.h>
#include <qb/io/udp.h>

#ifndef QB_MODULE_IOPOLL_EVENT_BASE_H
#define QB_MODULE_IOPOLL_EVENT_BASE_H

namespace qbm {
    namespace iopoll {

        struct Handle {
            epoll_event ep_event;

            io::udp::Socket const &udp() const { return *reinterpret_cast<const io::udp::Socket*>(&ep_event.events + 1); }
            io::tcp::Socket const &tcp() const { return *reinterpret_cast<const io::tcp::Socket*>(&ep_event.events + 1); }
            inline void setHandle(int const fd) { ep_event.data.fd = fd; }
            inline void setOwner(uint32_t const id) { *(&ep_event.data.u32 + 1) = id; }
            inline void setEvents(uint32_t const events) { ep_event.events = events; }

            inline int getHandle() const { return ep_event.data.fd; }
            inline uint32_t getOwner() const { return *(&ep_event.data.u32 + 1); }
            inline uint32_t getEvents() const { return ep_event.events; }

        };

        struct Proxy
                : public Handle {
            io::epoll::Proxy proxy;
            Proxy() : proxy(-1) {}
            Proxy(Proxy const &) = default;
            Proxy(io::epoll::Proxy const &proxy)
                    : proxy(proxy) {}

            inline bool receive(void *data, std::size_t size, std::size_t &received) const {
                return tcp().receive(data, size, received) == io::SocketStatus::Done;
            }

            inline bool send(const void *data, std::size_t size, std::size_t &sent) const {
                return tcp().send(data, size, sent) == io::SocketStatus::Done;
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
