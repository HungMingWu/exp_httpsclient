#include "catch.hpp"
#include <optional>
#include <boost/asio/io_context.hpp>
#include <boost/asio/read.hpp>
#include <boost/beast/core/buffered_read_stream.hpp>
#include "stream.hpp"

namespace net = boost::asio;
using namespace boost::beast;
TEST_CASE("testSpecialMembers 1", "buffered_read_stream") {
    net::io_context ioc;
    {
        buffered_read_stream<test::stream, multi_buffer> srs(ioc);
        buffered_read_stream<test::stream, multi_buffer> srs2(std::move(srs));
        srs = std::move(srs2);
#if defined(BOOST_ASIO_USE_TS_EXECUTOR_AS_DEFAULT)
        REQUIRE(&srs.get_executor().context() == &ioc);
        REQUIRE(
            &srs.get_executor().context() ==
            &srs2.get_executor().context());
#else
        REQUIRE(&net::query(srs.get_executor(), net::execution::context) == &ioc);
        REQUIRE(
            &net::query(srs.get_executor(), net::execution::context) ==
            &net::query(srs2.get_executor(), net::execution::context));
#endif
    }
}

struct loop : std::enable_shared_from_this<loop>
{
    static std::size_t constexpr limit = 100;
    std::string s_;
    std::size_t n_ = 0;
    std::size_t cap_;
    net::io_context& ioc_;
    std::optional<test::stream> ts_;
    std::optional<test::fail_count> fc_;
    std::optional<buffered_read_stream<
        test::stream&, multi_buffer>> brs_;

    loop(
        net::io_context& ioc,
        std::size_t cap)
        : cap_(cap)
        , ioc_(ioc)
    {
    }

    void
        run()
    {
        do_read();
    }

    void
        on_read(error_code ec, std::size_t)
    {
        if (!ec)
        {
            REQUIRE(s_ == "Hello, world!");
            return;
        }
        ++n_;
        REQUIRE(n_ < limit);
        s_.clear();
        do_read();
    }

    void
        do_read()
    {
        s_.resize(13);
        fc_.emplace(n_);
        ts_.emplace(ioc_, *fc_, ", world!");
        brs_.emplace(*ts_);
        brs_->buffer().commit(net::buffer_copy(
            brs_->buffer().prepare(5), net::buffer("Hello", 5)));
        net::async_read(*brs_,
            net::buffer(&s_[0], s_.size()),
            bind_front_handler(
                &loop::on_read,
                shared_from_this()));
    }
};

TEST_CASE("testAsyncLoop 1", "buffered_read_stream") {
    net::io_context ioc_;
    std::make_shared<loop>(ioc_, 0)->run();
    ioc_.run();
}

TEST_CASE("testAsyncLoop 2", "buffered_read_stream") {
    net::io_context ioc_;
    std::make_shared<loop>(ioc_, 3)->run();
    ioc_.run();
}