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
                Ready(io::epoll::Proxy const &proxy, epoll_event const &event)
                        : Proxy(proxy) {
                    ep_event = event;
                }
            };
        }
    }
}

#endif //QB_MODULE_IOPOLL_EVENT_READY_H
