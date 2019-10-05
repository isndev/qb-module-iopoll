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

#include <qb/actor.h>
#include <qb/io/epoll.h>
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
                io::epoll::Poller<> _epoll;
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
                    LOG_DEBUG("subscribed=" << event.getHandle()
                              << " id=" << event.getOwner()
                              << "events=" << event.getEvents());
                    if (_epoll.add(event.ep_event))
                        throw std::runtime_error("failed add");
                }
            };

        }
    }
}

#endif //QB_MODULE_IOPOLL_SERVICE_ACTOR_H
