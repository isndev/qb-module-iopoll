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

#include <algorithm>
#include <vector>
#include <qb/actor.h>

#include "../routine.h"

#ifndef QB_MODULE_IOPOLL_ACTOR_BASEROUTINE_H
#define QB_MODULE_IOPOLL_ACTOR_BASEROUTINE_H

namespace qbm {
    namespace iopoll {
        namespace actor {

            template<typename Derived>
            class BaseRoutine
                    : public Routine<Derived>
                    , public qb::Actor {
            protected:
                BaseRoutine() {}

                inline void reset_timer(std::size_t const seconds) {
                    this->setTimer(this->time() +
                                   static_cast<uint64_t>(qb::Timespan::seconds(seconds + (seconds / 2)).nanoseconds()));
                }

            public:
                virtual bool onInit() override final {
                    registerEvent<event::Ready>(*this);
                    return static_cast<Routine<Derived> &>(*this).onInitialize();
                }

                // Actor input events
                void on(event::Ready &event) {
                    static_cast<Routine<Derived> &>(*this).on(event);
                }

            };

        }
    }
}

#endif //QB_MODULE_IOPOLL_ACTOR_BASEROUTINE_H
