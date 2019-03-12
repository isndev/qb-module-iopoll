//
// Created by isndev on 7/8/18.
//

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
