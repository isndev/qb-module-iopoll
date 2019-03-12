#include <gtest/gtest.h>
#include <qb/main.h>

#include "../service/SocketPoller.h"
#include "../actor/Listener.h"

class TestSession
        : public qbm::iopoll::actor::BaseRoutine<TestSession> {
public:
    constexpr static const qbm::iopoll::Type type = qbm::iopoll::Type::READWRITE;
    constexpr static const bool has_keepalive = true;
public:
    TestSession() = default;

    bool onInitialize() {
        reset_timer(3);
        return true;
    }

    qbm::iopoll::ReturnValue onWrite(qbm::iopoll::event::Ready const &event) {
        return qbm::iopoll::ReturnValue::REPOLL;
    }

    qbm::iopoll::ReturnValue onRead(qbm::iopoll::event::Ready const &event) {
        char buffer[255];
        std::size_t received;
        event.tcp().receive(buffer, 255, received);

        EXPECT_EQ(std::strcmp("hello world !", buffer), 0);

        return qbm::iopoll::ReturnValue::KO;
    }

    qbm::iopoll::ReturnValue onTimeout(qbm::iopoll::event::Ready const &) {
        return qbm::iopoll::ReturnValue::KO;
    }

    qbm::iopoll::ReturnValue onDisconnect(qbm::iopoll::event::Ready const &) {
        push<qb::KillEvent>(getServiceId<qbm::iopoll::service::Tag>(0));
        kill();
        return qbm::iopoll::ReturnValue::KO;
    }
};

class TestListener : public qbm::iopoll::actor::Listener<TestListener>
{
public:
    TestListener(unsigned short const port)
        : Listener(0, port) {}

    bool onInitialize() {
        return true;
    }
    void onConnect(qb::network::tcp::Socket event) {
        event.setBlocking(false);
        auto session = addRefActor<TestSession>();
        auto &e = push<qbm::iopoll::event::Subscribe>(_io_id);
        e.setHandle(event.raw());
        e.setEvents(EPOLLIN | EPOLLOUT | EPOLLONESHOT);
        e.setOwner(session->id());
        kill();
    }
};

TEST(Example, TestIOPollingOnListener) {
    qb::Main main({0});

    main.addActor<qbm::iopoll::service::SocketPoller>(0);
    main.addActor<TestListener>(0u, 12399);

    main.start();

    qb::network::tcp::Socket client;
    client.connect("127.0.0.1", 12399);
    client.send("hello world !", 14);

    main.join();
    EXPECT_FALSE(main.hasError());
}

TEST(Example, TestIOPollingTimeoutKeepAlive) {
    qb::Main main({0});

    main.addActor<qbm::iopoll::service::SocketPoller>(0);
    main.addActor<TestListener>(0u, 12399);

    main.start();

    qb::network::tcp::Socket client;
    client.connect("127.0.0.1", 12399);

    main.join();
    EXPECT_FALSE(main.hasError());
}