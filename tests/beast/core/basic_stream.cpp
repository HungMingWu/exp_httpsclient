#include "catch.hpp"
#include <iostream>
#include <optional>
#include <string_view>
#include <thread>
#include <asio/io_context.hpp>
#include <asio/strand.hpp>
#include <asio/ip/tcp.hpp>
#include <asio/write.hpp>
#include <boost/beast/core/basic_stream.hpp>
#include <boost/beast/core/tcp_stream.hpp>
#include <boost/beast/core/role.hpp>
#include "stream_tests.hpp"

namespace net = asio;
using tcp = net::ip::tcp;
using executor = net::io_context::executor_type;
using strand = net::strand<executor>;

TEST_CASE("testSpecialMembers net::io_context::executor_type", "basic_stream") {
    net::io_context ioc;
    auto ex = ioc.get_executor();
    boost::beast::basic_stream<tcp, executor> s1(ioc);
    boost::beast::basic_stream<tcp, executor> s2(ex);
    boost::beast::basic_stream<tcp, executor> s3(ioc, tcp::v4());
    boost::beast::basic_stream<tcp, executor> s4(std::move(s1));
    s2.socket() =
        net::basic_stream_socket<tcp, executor>(ioc);
    REQUIRE(s1.get_executor() == ex);
    REQUIRE(s2.get_executor() == ex);
    REQUIRE(s3.get_executor() == ex);
    REQUIRE(s4.get_executor() == ex);

    REQUIRE((!static_cast<
        boost::beast::basic_stream<tcp, executor> const&>(
            s2).socket().is_open()));

    boost::beast::test_sync_stream<
        boost::beast::basic_stream<
        tcp, executor>>();

    boost::beast::test_async_stream<
        boost::beast::basic_stream<
        tcp, executor>>();
}

TEST_CASE("testSpecialMembers net::io_context::strand", "basic_stream") {
    net::io_context ioc;
    auto ex = net::make_strand(ioc);
    boost::beast::basic_stream<tcp, strand> s1(ex);
    boost::beast::basic_stream<tcp, strand> s2(ex, tcp::v4());
    boost::beast::basic_stream<tcp, strand> s3(std::move(s1));
    REQUIRE(s1.get_executor() == ex);
    REQUIRE(s2.get_executor() == ex);
    REQUIRE(s3.get_executor() == ex);

    boost::beast::test_sync_stream<
        boost::beast::basic_stream<
        tcp, strand>>();

    boost::beast::test_async_stream<
        boost::beast::basic_stream<
        tcp, strand>>();
}

TEST_CASE("testSpecialMembers layers", "basic_stream") {
    net::io_context ioc;
    net::socket_base::keep_alive opt;
    boost::beast::tcp_stream s(ioc);
    s.socket().open(tcp::v4());
    s.socket().get_option(opt);
    REQUIRE(!opt.value());
    opt = true;
    s.socket().set_option(opt);
    opt = false;
    REQUIRE(!opt.value());
}

TEST_CASE("testSpecialMembers rate policies 1", "basic_stream") {
    net::io_context ioc;
    boost::beast::basic_stream<tcp,
        executor,
        boost::beast::simple_rate_policy> s(ioc);
}

TEST_CASE("testSpecialMembers rate policies 2", "basic_stream") {
    net::io_context ioc;
    boost::beast::basic_stream<tcp,
        executor,
        boost::beast::unlimited_rate_policy> s(ioc);
}

TEST_CASE("testSpecialMembers rate policies 3", "basic_stream") {
    net::io_context ioc;
    boost::beast::basic_stream<tcp,
        executor,
        boost::beast::unlimited_rate_policy> s(ioc);
}

TEST_CASE("testSpecialMembers rate policies 4", "basic_stream") {
    net::io_context ioc;
    boost::beast::basic_stream<tcp,
        net::io_context::executor_type,
        boost::beast::unlimited_rate_policy> s(
            boost::beast::unlimited_rate_policy{}, ioc);
}

using stream_type = boost::beast::basic_stream<tcp, executor>;
TEST_CASE("testRead read_some", "basic_stream") {
    net::io_context ioc;
    std::error_code ec;
    stream_type s(ioc, tcp::v4());
    REQUIRE(s.read_some(net::mutable_buffer{}) == 0);
    REQUIRE(s.read_some(net::mutable_buffer{}, ec) == 0);
    REQUIRE(!ec);
}

namespace {
    class test_server
    {
        std::string_view s_;
        net::io_context ioc_;
        net::ip::tcp::acceptor acceptor_;
        net::ip::tcp::socket socket_;
        std::thread t_;

        void
            fail(std::error_code const& ec, std::string_view what)
        {
            if (ec != net::error::operation_aborted)
                std::cerr << what << ": " << ec.message() << "\n";
        }

    public:
        test_server(
            std::string_view s,
            net::ip::tcp::endpoint ep)
            : s_(s)
            , ioc_(1)
            , acceptor_(ioc_)
            , socket_(ioc_)
        {
            std::error_code ec;

            acceptor_.open(ep.protocol(), ec);
            if (ec)
            {
                fail(ec, "open");
                return;
            }

            acceptor_.set_option(
                net::socket_base::reuse_address(true), ec);
            if (ec)
            {
                fail(ec, "set_option");
                return;
            }

            acceptor_.bind(ep, ec);
            if (ec)
            {
                fail(ec, "bind");
                return;
            }

            acceptor_.listen(
                net::socket_base::max_listen_connections, ec);
            if (ec)
            {
                fail(ec, "listen");
                return;
            }

            acceptor_.async_accept(socket_,
                [this](std::error_code ec)
                {
                    this->on_accept(ec);
                });

            t_ = std::thread(
                [this]
                {
                    ioc_.run();
                });
        }

        ~test_server()
        {
            ioc_.stop();
            t_.join();
        }

        net::ip::tcp::endpoint
            local_endpoint() const noexcept
        {
            return acceptor_.local_endpoint();
        }

    private:
        class session
            : public std::enable_shared_from_this<session>
        {
            std::string_view s_;
            net::ip::tcp::socket socket_;

        public:
            session(
                std::string_view s,
                net::ip::tcp::socket sock)
                : s_(s)
                , socket_(std::move(sock))
            {
            }

            void
                run()
            {
                if (s_.empty())
                    socket_.async_wait(
                        net::socket_base::wait_read,
                        boost::beast::bind_front_handler(
                            &session::on_read,
                            shared_from_this()));
                else
                    net::async_write(
                        socket_,
                        net::const_buffer(s_.data(), s_.size()),
                        boost::beast::bind_front_handler(
                            &session::on_write,
                            shared_from_this()));
            }

        protected:
            void
                on_read(std::error_code const&)
            {
            }

            void
                on_write(std::error_code const&, std::size_t)
            {
            }
        };

        void
            on_accept(std::error_code const& ec)
        {
            if (!acceptor_.is_open())
                return;
            if (ec)
                fail(ec, "accept");
            else
                std::make_shared<session>(
                    s_, std::move(socket_))->run();
            acceptor_.async_accept(socket_,
                [this](std::error_code ec)
                {
                    this->on_accept(ec);
                });
        }
    };
    class handler
    {
        std::optional<std::error_code> ec_;
        std::size_t n_;

    public:
        handler(std::error_code ec, std::size_t n)
            : ec_(ec)
            , n_(n)
        {
        }

        handler(handler&& other)
            : ec_(other.ec_)
            , n_(std::exchange(other.n_,
                (std::numeric_limits<std::size_t>::max)()))
        {
        }

        ~handler()
        {
            REQUIRE(
                n_ == (std::numeric_limits<std::size_t>::max)());
        }

        void
            operator()(std::error_code const& ec, std::size_t n)
        {
            REQUIRE(ec == *ec_);
            REQUIRE(n == n_);
            n_ = (std::numeric_limits<std::size_t>::max)();
        }
    };
}
auto const ep = net::ip::tcp::endpoint(net::ip::make_address("127.0.0.1"), 0);

TEST_CASE("testRead async_read_some success", "basic_stream") {
    net::io_context ioc;
    char buf[4];
    std::memset(buf, 0, sizeof(buf));
    net::mutable_buffer mb(buf, sizeof(buf));
    auto const ep = net::ip::tcp::endpoint(
        net::ip::make_address("127.0.0.1"), 0);
    test_server srv("*", ep);
    stream_type s(ioc);
    s.socket().connect(srv.local_endpoint());
    s.expires_never();
    s.async_read_some(mb, handler(std::error_code{0, net::error::get_system_category()}, 1));
    ioc.run();
    ioc.restart();
}

TEST_CASE("testRead async_read_some success, with timeout", "basic_stream") {
    net::io_context ioc;
    char buf[4];
    std::memset(buf, 0, sizeof(buf));
    net::mutable_buffer mb(buf, sizeof(buf));
    auto const ep = net::ip::tcp::endpoint(
        net::ip::make_address("127.0.0.1"), 0);
    test_server srv("*", ep);
    stream_type s(ioc);
    s.socket().connect(srv.local_endpoint());
    s.expires_after(std::chrono::seconds(30));
    s.async_read_some(mb, handler(std::error_code{ 0, net::error::get_system_category() }, 1));
    ioc.run();
    ioc.restart();
}

TEST_CASE("testRead async_read_some empty buffer", "basic_stream") {
    net::io_context ioc;
    auto const ep = net::ip::tcp::endpoint(
        net::ip::make_address("127.0.0.1"), 0);
    test_server srv("*", ep);
    stream_type s(ioc);
    s.socket().connect(srv.local_endpoint());
    s.expires_never();
    s.async_read_some(
        net::mutable_buffer{}, handler(std::error_code{ 0, net::error::get_system_category() }, 0));
    ioc.run();
    ioc.restart();
}

TEST_CASE("testRead async_read_some empty buffer, timeout", "basic_stream") {
    net::io_context ioc;
    auto const ep = net::ip::tcp::endpoint(
        net::ip::make_address("127.0.0.1"), 0);
    test_server srv("*", ep);
    stream_type s(ioc);
    s.socket().connect(srv.local_endpoint());
    s.expires_after(std::chrono::seconds(0));
    s.async_read_some(net::mutable_buffer{},
        handler(boost::beast::error::timeout, 0));
    ioc.run();
    ioc.restart();
}

TEST_CASE("testRead async_read_some expires_after", "basic_stream") {
    net::io_context ioc;
    char buf[4];
    std::memset(buf, 0, sizeof(buf));
    net::mutable_buffer mb(buf, sizeof(buf));
    auto const ep = net::ip::tcp::endpoint(
        net::ip::make_address("127.0.0.1"), 0);
    test_server srv("", ep);
    stream_type s(ioc);
    s.socket().connect(srv.local_endpoint());
    s.expires_after(std::chrono::seconds(0));
    s.async_read_some(mb, handler(boost::beast::error::timeout, 0));
    ioc.run();
    ioc.restart();
}

TEST_CASE("testRead async_read_some expires_at", "basic_stream") {
    net::io_context ioc;
    char buf[4];
    std::memset(buf, 0, sizeof(buf));
    net::mutable_buffer mb(buf, sizeof(buf));
    auto const ep = net::ip::tcp::endpoint(
        net::ip::make_address("127.0.0.1"), 0);
    test_server srv("", ep);
    stream_type s(ioc);
    s.socket().connect(srv.local_endpoint());
    s.expires_at(std::chrono::steady_clock::now());
    s.async_read_some(mb, handler(boost::beast::error::timeout, 0));
    ioc.run();
    ioc.restart();
}

TEST_CASE("testRead stream destroyed", "basic_stream") {
    net::io_context ioc;
    char buf[4];
    std::memset(buf, 0, sizeof(buf));
    net::mutable_buffer mb(buf, sizeof(buf));
    auto const ep = net::ip::tcp::endpoint(
        net::ip::make_address("127.0.0.1"), 0);
    test_server srv("*", ep);
    {
        stream_type s(ioc);
        s.socket().connect(srv.local_endpoint());
        s.expires_after(std::chrono::seconds(0));
        s.async_read_some(mb,
            [](std::error_code, std::size_t)
            {
            });
    }
    ioc.run();
    ioc.restart();
}

TEST_CASE("testRead stale timer", "basic_stream") {
    net::io_context ioc;
    char buf[4];
    std::memset(buf, 0, sizeof(buf));
    net::mutable_buffer mb(buf, sizeof(buf));
    stream_type s(ioc);
    s.expires_after(std::chrono::milliseconds(50));
    s.async_read_some(mb,
        [](std::error_code, std::size_t)
        {
        });
    std::this_thread::sleep_for(
        std::chrono::milliseconds(100));
    ioc.run();
    ioc.restart();
}


TEST_CASE("testRead abandoned operation", "basic_stream") {
    net::io_context ioc;
    stream_type s(ioc);
    bool checked = true;
    s.async_read_some(net::mutable_buffer{},
        [&](std::error_code, std::size_t)
        {
            checked = false;
        });
    REQUIRE(checked);
}

TEST_CASE("testWrite write_some", "basic_stream") {
    net::io_context ioc;
    std::error_code ec;
    stream_type s(ioc, tcp::v4());
    REQUIRE(s.write_some(net::const_buffer{}) == 0);
    REQUIRE(s.write_some(net::const_buffer{}, ec) == 0);
    REQUIRE(!ec);
}

TEST_CASE("testWrite async_write_some success", "basic_stream") {
    net::io_context ioc;
    char buf[4];
    std::memset(buf, 0, sizeof(buf));
    net::const_buffer cb(buf, sizeof(buf));
    auto const ep = net::ip::tcp::endpoint(
        net::ip::make_address("127.0.0.1"), 0);
    test_server srv("*", ep);
    stream_type s(ioc);
    s.socket().connect(srv.local_endpoint());
    s.expires_never();
    s.async_write_some(cb, handler(std::error_code{ 0, net::error::get_system_category() }, 4));
    ioc.run();
    ioc.restart();
}

TEST_CASE("testWrite async_write_some success, with timeout", "basic_stream") {
    net::io_context ioc;
    char buf[4];
    std::memset(buf, 0, sizeof(buf));
    net::const_buffer cb(buf, sizeof(buf));
    auto const ep = net::ip::tcp::endpoint(
        net::ip::make_address("127.0.0.1"), 0);
    test_server srv("*", ep);
    stream_type s(ioc);
    s.socket().connect(srv.local_endpoint());
    s.expires_after(std::chrono::seconds(30));
    s.async_write_some(cb, handler(std::error_code{ 0, net::error::get_system_category() }, 4));
    ioc.run();
    ioc.restart();
}

TEST_CASE("testWrite async_write_some empty buffer", "basic_stream") {
    net::io_context ioc;
    auto const ep = net::ip::tcp::endpoint(
        net::ip::make_address("127.0.0.1"), 0);
    test_server srv("*", ep);
    stream_type s(ioc);
    s.socket().connect(srv.local_endpoint());
    s.expires_never();
    s.async_write_some(
        net::const_buffer{}, handler(std::error_code{ 0, net::error::get_system_category() }, 0));
    ioc.run();
    ioc.restart();
}

TEST_CASE("testWrite async_write_some empty buffer, timeout", "basic_stream") {
    net::io_context ioc;
    auto const ep = net::ip::tcp::endpoint(
        net::ip::make_address("127.0.0.1"), 0);
    test_server srv("*", ep);
    stream_type s(ioc);
    s.socket().connect(srv.local_endpoint());
    s.expires_after(std::chrono::seconds(0));
    s.async_write_some(net::const_buffer{},
        handler(boost::beast::error::timeout, 0));
    ioc.run();
    ioc.restart();
}

TEST_CASE("testWrite abandoned operation", "basic_stream") {
    net::io_context ioc;
    stream_type s(ioc);
    bool checked = true;
    s.async_write_some(net::mutable_buffer{},
        [&](std::error_code, std::size_t)
        {
            checked = false;
        });
    REQUIRE(checked);
}

namespace {
    struct test_acceptor
    {
        net::io_context ioc;
        net::ip::tcp::acceptor a;
        net::ip::tcp::endpoint ep;

        test_acceptor()
            : a(ioc)
            , ep(net::ip::make_address_v4("127.0.0.1"), 0)
        {
            a.open(ep.protocol());
            a.set_option(
                net::socket_base::reuse_address(true));
            a.bind(ep);
            a.listen(net::socket_base::max_listen_connections);
            ep = a.local_endpoint();
            a.async_accept(
                [](std::error_code, net::ip::tcp::socket)
                {
                });
        }
    };

    struct range
    {
        tcp::endpoint ep;

        using iterator =
            tcp::endpoint const*;

        // VFALCO This is here because asio mistakenly requires it
        using const_iterator =
            tcp::endpoint const*;

        iterator begin() const noexcept
        {
            return &ep;
        }

        // VFALCO need to use const_iterator to silence
        //        warning about unused types
        const_iterator end() const noexcept
        {
            return begin() + 1;
        }
    };

    struct connect_condition
    {
        bool operator()(std::error_code, tcp::endpoint) const
        {
            return true;
        };
    };

    class connect_handler
    {
        bool pass_ = false;
        std::optional<std::error_code> expected_ = {};

    public:
        ~connect_handler()
        {
            REQUIRE(pass_);
        }

        connect_handler()
            : expected_({})
        {
        }

        explicit
            connect_handler(std::error_code expected)
            : expected_(expected)
        {
        }

        explicit
            connect_handler(std::nullopt_t)
        {
        }

        connect_handler(connect_handler&& other)
            : pass_(std::exchange(other.pass_, true))
            , expected_(other.expected_)
        {
        }

        void operator()(std::error_code ec)
        {
            pass_ = true;
            if (expected_)
                REQUIRE(ec == expected_);
        }
    };

    struct range_handler
    {
        bool pass = false;

        range_handler() = default;

        range_handler(range_handler&& other)
            : pass(std::exchange(other.pass, true))
        {
        }

        ~range_handler()
        {
            REQUIRE(pass);
        }

        void operator()(std::error_code ec, tcp::endpoint)
        {
            pass = true;
            REQUIRE(!ec);
        }
    };

    struct iterator_handler
    {
        bool pass = false;

        iterator_handler() = default;

        iterator_handler(iterator_handler&& other)
            : pass(std::exchange(other.pass, true))
        {
        }

        ~iterator_handler()
        {
            REQUIRE(pass);
        }

        void operator()(std::error_code ec, tcp::endpoint const*)
        {
            pass = true;
            REQUIRE(!ec);
        }
    };
}

TEST_CASE("testConnect connect (member)", "basic_stream") {
    net::io_context ioc;
    test_acceptor a;
    stream_type s(ioc);
    std::error_code ec;
    s.connect(a.ep);
    s.socket().close();
    s.connect(a.ep, ec);
    REQUIRE(!ec);
}

TEST_CASE("testConnect connect 1", "basic_stream") {
    net::io_context ioc;
    test_acceptor a;
    stream_type s(ioc);
    std::error_code ec;
    range r;
    r.ep = a.ep;
    s.connect(r);
    s.socket().close();
    s.connect(r, ec);
    REQUIRE(!ec);
}

TEST_CASE("testConnect connect 2", "basic_stream") {
    net::io_context ioc;
    test_acceptor a;
    stream_type s(ioc);
    std::error_code ec;
    range r;
    connect_condition cond;
    r.ep = a.ep;
    s.connect(r, cond);
    s.socket().close();
    s.connect(r, cond, ec);
    REQUIRE(!ec);
}

TEST_CASE("testConnect connect 3", "basic_stream") {
    net::io_context ioc;
    test_acceptor a;
    stream_type s(ioc);
    std::error_code ec;
    range r;
    r.ep = a.ep;
    s.connect(r.begin(), r.end());
    s.socket().close();
    s.connect(r.begin(), r.end(), ec);
    REQUIRE(!ec);
}

TEST_CASE("testConnect connect 4", "basic_stream") {
    net::io_context ioc;
    test_acceptor a;
    stream_type s(ioc);
    std::error_code ec;
    range r;
    connect_condition cond;
    r.ep = a.ep;
    s.connect(r.begin(), r.end(), cond);
    s.socket().close();
    s.connect(r.begin(), r.end(), cond, ec);
    REQUIRE(!ec);
}

TEST_CASE("testConnect async_connect (member)", "basic_stream") {
    net::io_context ioc;
    test_acceptor a;
    stream_type s(ioc);
    s.expires_never();
    s.async_connect(a.ep, connect_handler{});
    ioc.run();
    ioc.restart();
    s.socket().close();
    s.expires_after(std::chrono::seconds(30));
    s.async_connect(a.ep, connect_handler{});
    ioc.run();
    ioc.restart();
}

TEST_CASE("testConnect async_connect 1", "basic_stream") {
    net::io_context ioc;
    test_acceptor a;
    stream_type s(ioc);
    range r;
    r.ep = a.ep;
    s.expires_never();
    s.async_connect(r, range_handler{});
    ioc.run();
    ioc.restart();
    s.socket().close();
    s.expires_after(std::chrono::seconds(30));
    s.async_connect(r, range_handler{});
    ioc.run();
    ioc.restart();
}

TEST_CASE("testConnect async_connect 2", "basic_stream") {
    net::io_context ioc;
    test_acceptor a;
    stream_type s(ioc);
    range r;
    connect_condition cond;
    r.ep = a.ep;
    s.expires_never();
    s.async_connect(r, cond, range_handler{});
    ioc.run();
    ioc.restart();
    s.socket().close();
    s.expires_after(std::chrono::seconds(30));
    s.async_connect(r, cond, range_handler{});
    ioc.run();
    ioc.restart();
}

TEST_CASE("testConnect async_connect 3", "basic_stream") {
    net::io_context ioc;
    test_acceptor a;
    stream_type s(ioc);
    range r;
    r.ep = a.ep;
    s.expires_never();
    s.async_connect(r.begin(), r.end(),
        iterator_handler{});
    ioc.run();
    ioc.restart();
    s.socket().close();
    s.expires_after(std::chrono::seconds(30));
    s.async_connect(r.begin(), r.end(),
        iterator_handler{});
    ioc.run();
    ioc.restart();
}

TEST_CASE("testConnect async_connect 4", "basic_stream") {
    net::io_context ioc;
    test_acceptor a;
    stream_type s(ioc);
    range r;
    connect_condition cond;
    r.ep = a.ep;
    s.expires_never();
    s.async_connect(r.begin(), r.end(), cond,
        iterator_handler{});
    ioc.run();
    ioc.restart();
    s.socket().close();
    s.expires_after(std::chrono::seconds(30));
    s.async_connect(r.begin(), r.end(), cond,
        iterator_handler{});
    ioc.run();
    ioc.restart();
}

TEST_CASE("testConnect async_connect normal timeout", "basic_stream") {
    net::io_context ioc;
    // Requires timeout happen before ECONNREFUSED 
    stream_type s(ioc);
    auto const ep = net::ip::tcp::endpoint(
#if 1
        // This address _should_ be unconnectible
        net::ip::make_address("72.5.65.111"), 1);
#else
        // On Travis ECONNREFUSED happens before the timeout
        net::ip::make_address("127.0.0.1"), 1);
#endif
    s.expires_after(std::chrono::seconds(0));
    s.async_connect(ep, connect_handler{ boost::beast::error::timeout });
    ioc.run_for(std::chrono::seconds(1));
    ioc.restart();
}

TEST_CASE("testConnect async_connect stream destroyed", "basic_stream") {
    net::io_context ioc;
    {
        stream_type s(ioc);
        auto const ep = net::ip::tcp::endpoint(
            net::ip::make_address("127.0.0.1"), 1);
        s.expires_after(std::chrono::seconds(0));
        s.async_connect(ep, connect_handler{ std::nullopt });
    }
    ioc.run();
    ioc.restart();
}

TEST_CASE("testConnect async_connect stale timer", "basic_stream") {
    net::io_context ioc;
    test_acceptor a;
    stream_type s(ioc);
    s.expires_after(std::chrono::milliseconds(50));
    s.async_connect(a.ep, connect_handler{});
    std::this_thread::sleep_for(
        std::chrono::milliseconds(100));
    ioc.run();
    ioc.restart();
}

TEST_CASE("testConnect async_connect abandoned operation", "basic_stream") {
    net::io_context ioc;
    stream_type s(ioc);
    net::ip::tcp::endpoint ep(
        net::ip::make_address_v4("127.0.0.1"), 1);
    bool checked = true;
    s.async_connect(ep,
        [&](std::error_code)
        {
            checked = false;
        });
    REQUIRE(checked);
}

namespace {
    class member_handler
    {
        bool pass_ = false;
        std::optional<std::error_code> expected_ = {};

    public:
        ~member_handler()
        {
            REQUIRE(pass_);
        }

        member_handler()
            : expected_({})
        {
        }

        explicit
            member_handler(std::error_code expected)
            : expected_(expected)
        {
        }

        explicit
            member_handler(std::nullopt_t)
        {
        }

        member_handler(member_handler&& other)
            : pass_(std::exchange(other.pass_, true))
            , expected_(other.expected_)
        {
        }

        void operator()(std::error_code ec, std::size_t)
        {
            pass_ = true;
            if (expected_)
                REQUIRE(ec == expected_);
        }
    };
}

TEST_CASE("testMembers cancel", "basic_stream") {
    net::io_context ioc;
    char buf[4];
    std::memset(buf, 0, sizeof(buf));
    net::mutable_buffer mb(buf, sizeof(buf));
    auto const ep = net::ip::tcp::endpoint(
        net::ip::make_address("127.0.0.1"), 0);
    test_server srv("", ep);
    stream_type s(ioc);
    s.connect(srv.local_endpoint());
    s.expires_never();
    s.socket().async_read_some(mb, member_handler(
        net::error::operation_aborted));
    s.cancel();
    ioc.run();
    ioc.restart();
}

TEST_CASE("testMembers close", "basic_stream") {
    net::io_context ioc;
    char buf[4];
    std::memset(buf, 0, sizeof(buf));
    net::mutable_buffer mb(buf, sizeof(buf));
    auto const ep = net::ip::tcp::endpoint(
        net::ip::make_address("127.0.0.1"), 0);
    test_server srv("", ep);
    stream_type s(ioc);
    s.connect(srv.local_endpoint());
    s.expires_never();
    s.socket().async_read_some(mb,
        member_handler(std::nullopt));
    s.close();
    ioc.run();
    ioc.restart();
}

TEST_CASE("testMembers destructor", "basic_stream") {
    net::io_context ioc;
    char buf[4];
    auto mb = net::buffer(buf);
    std::memset(buf, 0, sizeof(buf));
    auto const ep = net::ip::tcp::endpoint(
        net::ip::make_address("127.0.0.1"), 0);
    test_server srv("", ep);
    {
        stream_type s(ioc);
        s.connect(srv.local_endpoint());
        s.expires_never();
        s.socket().async_read_some(mb,
            member_handler(std::nullopt));
    }
    ioc.run();
    ioc.restart();
}

TEST_CASE("testMembers customization points 1", "basic_stream") {
    net::io_context ioc;
    stream_type s(ioc);
    boost::beast::close_socket(s);
}

TEST_CASE("testMembers customization points 2", "basic_stream") {
    net::io_context ioc;
    std::error_code ec;
    stream_type s(ioc);
    boost::beast::teardown(boost::beast::role_type::client, s, ec);
}

TEST_CASE("testMembers customization points 3", "basic_stream") {
    net::io_context ioc;
    stream_type s(ioc);
    boost::beast::async_teardown(boost::beast::role_type::server, s,
        [](std::error_code)
        {
        });
}

TEST_CASE("testIssue1589", "basic_stream") {
    net::io_context ioc;
    // the timer needlessly used polymorphic executor
    boost::beast::basic_stream<tcp, executor>{ioc};

    // make sure strands work
    boost::beast::basic_stream<
        tcp,
        net::strand<
        executor>>{
        net::make_strand(ioc)};

    // address the problem in the issue
    {
        net::basic_stream_socket<tcp, strand> sock(net::make_strand(ioc));
        boost::beast::basic_stream<
            tcp,
            strand,
            boost::beast::unlimited_rate_policy> stream(std::move(sock));
        static_assert(
            std::is_convertible<
            decltype(sock)::executor_type,
            decltype(stream)::executor_type>::value);
    }
}

TEST_CASE("testIssue2065", "basic_stream") {
    net::io_context ioc;
    char buf[4];
    std::memset(buf, 0, sizeof(buf));
    net::mutable_buffer mb(buf, sizeof(buf));
    auto const ep = net::ip::tcp::endpoint(
        net::ip::make_address("127.0.0.1"), 0);

    // async_read_some
    {
        // success
        test_server srv("*", ep);
        stream_type s(ioc);
        s.socket().connect(srv.local_endpoint());
        s.expires_never();
        s.async_read_some(mb, handler(std::error_code{ 0, net::error::get_system_category() }, 1));
        s.async_read_some(net::buffer(buf, 0), handler({}, 0));
        ioc.run();
        ioc.restart();
    }
}
