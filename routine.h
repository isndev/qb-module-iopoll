//
// Created by isndev on 12/15/18.
//

#include "types.h"
#include "event/Ready.h"

#ifndef QB_MODULE_IOPOLL_ROUTINE_H
#define QB_MODULE_IOPOLL_ROUTINE_H

namespace qbm {
    namespace iopoll {

        enum Type : uint32_t {
            READ = EPOLLIN,
            WRITE = EPOLLOUT,
            READWRITE = EPOLLIN | EPOLLOUT
        };

        // example trait to implement
        struct ExampleTrait {
            constexpr static const Type type = Type::READWRITE;
            constexpr static bool hasKeepAlive = true;

            bool onInitialize();
            qbm::iopoll::ReturnValue onRead(event::Ready &event);
            qbm::iopoll::ReturnValue onWrite(event::Ready &event);
            qbm::iopoll::ReturnValue onTimeout(event::Ready &event);
            void onDisconnect(event::Ready &event);
        };

        template<typename Derived>
        class Routine {

            uint64_t limit_time_activity;
        protected:
            Routine() : limit_time_activity(0) {}

            inline void setTimer(std::size_t const timer) {
                limit_time_activity = timer;
            }

            inline uint64_t getTimer() const {
                return limit_time_activity;
            }

            inline void repoll(Proxy &event) const {
                event.setEvents(Derived::type);
                event.repoll();
            }


        public:
            bool onInitialize() {
                return static_cast<Derived &>(*this).onInitialize();
            }

            // Actor input events
            void on(Proxy &event) {
                auto status = ReturnValue::KO;

                if constexpr (Derived::type == Type::WRITE) {
                    status = event.getEvents() & EPOLLOUT
                             ? static_cast<Derived &>(*this).onWrite(event)
                             : ReturnValue::KO;
                } else if constexpr (Derived::type == Type::READ) {
                    status = event.getEvents() & EPOLLIN
                             ? static_cast<Derived &>(*this).onRead(event)
                             : ReturnValue::KO;
                } else {
                    if (event.getEvents() & EPOLLOUT) {
                        // Socket write workflow
                        status = static_cast<Derived &>(*this).onWrite(event);
                    }
                    if (event.getEvents() & EPOLLIN)
                        status = static_cast<Derived &>(*this).onRead(event);
                }

                if constexpr (Derived::has_keepalive) {
                    // check activity
                    if (static_cast<Derived &>(*this).time() > limit_time_activity)
                        status = static_cast<Derived &>(*this).onTimeout(event);
                }

                switch (status) {
                    case ReturnValue::REPOLL:
                        repoll(event);
                        break;
                    case ReturnValue::KO:
                        static_cast<Derived &>(*this).onDisconnect(event);
                        break;
                    default:
                        break;
                };

            }

        };

    }
}

#endif //QB_MODULE_IOPOLL_ROUTINE_H
