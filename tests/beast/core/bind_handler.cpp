#include "catch.hpp"
#include <asio/io_context.hpp>
#include <asio/bind_executor.hpp>
#include <asio/post.hpp>
#include <asio/strand.hpp>
#include <boost/beast/core/bind_handler.hpp>
#include "test_handler.hpp"

namespace net = asio;

namespace {
    struct move_only
    {
        move_only() = default;
        move_only(move_only&&) = default;
        move_only(move_only const&) = delete;
        void operator()() const {};
    };

    // A move-only parameter
    template<std::size_t I>
    struct move_arg
    {
        move_arg() = default;
        move_arg(move_arg&&) = default;
        move_arg(move_arg const&) = delete;
        void operator()() const
        {
        };
    };

    class test_cb
    {
        bool fail_ = true;

    public:
        test_cb() = default;
        test_cb(test_cb const&) = delete;
        test_cb(test_cb&& other)
            : fail_(std::exchange(
                other.fail_, false))
        {
        }

        ~test_cb()
        {
            REQUIRE(!fail_);
        }

        void
            operator()()
        {
            fail_ = false;
            REQUIRE(true);
        }

        void
            operator()(int v)
        {
            fail_ = false;
            REQUIRE(v == 42);
        }

        void
            operator()(int v, std::string_view s)
        {
            fail_ = false;
            REQUIRE(v == 42);
            REQUIRE(s == "s");
        }

        void
            operator()(int v, std::string_view s, move_arg<1>)
        {
            fail_ = false;
            REQUIRE(v == 42);
            REQUIRE(s == "s");
        }

        void
            operator()(int v, std::string_view s, move_arg<1>, move_arg<2>)
        {
            fail_ = false;
            REQUIRE(v == 42);
            REQUIRE(s == "s");
        }

        void
            operator()(std::error_code, std::size_t n)
        {
            fail_ = false;
            REQUIRE(n == 256);
        }

        void
            operator()(
                std::error_code, std::size_t n, std::string_view s)
        {
            boost::ignore_unused(s);
            fail_ = false;
            REQUIRE(n == 256);
        }

        void
            operator()(std::shared_ptr<int> const& sp)
        {
            fail_ = false;
            REQUIRE(sp.get() != nullptr);
        }
    };

    class bind_handler_test {
        bool invoked_ = false;
    public:
        void on_invoke()
        {
            invoked_ = true;
        }

        template<class F>
        void testHooks(net::io_context& ioc, F&& f)
        {
            invoked_ = false;
            net::post(ioc, std::forward<F>(f));
            ioc.run();
            ioc.restart();
            REQUIRE(invoked_);
        }
    };

    class test_executor
    {
        bind_handler_test& s_;

#if defined(ASIO_NO_TS_EXECUTORS)
        net::any_io_executor ex_;

        // Storing the blocking property as a member is not strictly necessary,
        // as we could simply forward the calls
        //   require(ex_, blocking.possibly)
        // and
        //   require(ex_, blocking.never)
        // to the underlying executor, and then
        //   query(ex_, blocking)
        // when required. This forwarding approach is used here for the
        // outstanding_work property.
        net::execution::blocking_t blocking_;

#else  // defined(ASIO_NO_TS_EXECUTORS)
        net::io_context::executor_type ex_;
#endif // defined(ASIO_NO_TS_EXECUTORS)
    public:
        test_executor(
            test_executor const&) = default;

        test_executor(
            bind_handler_test& s,
            net::io_context& ioc)
            : s_(s)
            , ex_(ioc.get_executor())
#if defined(ASIO_NO_TS_EXECUTORS)
            , blocking_(net::execution::blocking.possibly)
#endif
        {
        }

        bool operator==(
            test_executor const& other) const noexcept
        {
            return ex_ == other.ex_;
        }

        bool operator!=(
            test_executor const& other) const noexcept
        {
            return ex_ != other.ex_;
        }

#if defined(ASIO_NO_TS_EXECUTORS)

        net::execution_context&
            query(
                net::execution::context_t c) const noexcept
        {
            return net::query(ex_, c);
        }

        net::execution::blocking_t
            query(
                net::execution::blocking_t) const noexcept
        {
            return blocking_;
        }

        net::execution::outstanding_work_t
            query(
                net::execution::outstanding_work_t w) const noexcept
        {
            return net::query(ex_, w);
        }

        test_executor
            require(
                net::execution::blocking_t::possibly_t b) const
        {
            test_executor new_ex(*this);
            new_ex.blocking_ = b;
            return new_ex;
        }

        test_executor
            require(
                net::execution::blocking_t::never_t b) const
        {
            test_executor new_ex(*this);
            new_ex.blocking_ = b;
            return new_ex;
        }

        test_executor prefer(net::execution::outstanding_work_t::untracked_t w) const
        {
            test_executor new_ex(*this);
            new_ex.ex_ = net::prefer(ex_, w);
            return new_ex;
        }

        test_executor prefer(net::execution::outstanding_work_t::tracked_t w) const
        {
            test_executor new_ex(*this);
            new_ex.ex_ = net::prefer(ex_, w);
            return new_ex;
        }

        template<class F>
        void execute(F&& f) const
        {
            if (blocking_ == net::execution::blocking.possibly)
            {
                s_.on_invoke();
                net::execution::execute(ex_, std::forward<F>(f));
            }
            else
            {
                // shouldn't be called since the enclosing
                // networking wrapper only uses dispatch
                REQUIRE(false);
            }
        }
#endif
#if !defined(ASIO_NO_TS_EXECUTORS)
        net::execution_context&
            context() const noexcept
        {
            return ex_.context();
        }

        void
            on_work_started() const noexcept
        {
            ex_.on_work_started();
        }

        void
            on_work_finished() const noexcept
        {
            ex_.on_work_finished();
        }

        template<class F, class Alloc>
        void
            dispatch(F&& f, Alloc const& a)
        {
            s_.on_invoke();
            net::execution::execute(
                net::prefer(ex_,
                    net::execution::blocking.possibly,
                    net::execution::allocator(a)),
                std::forward<F>(f));
            // previously equivalent to
            // ex_.dispatch(std::forward<F>(f), a);
        }

        template<class F, class Alloc>
        void
            post(F&& f, Alloc const& a)
        {
            // shouldn't be called since the enclosing
            // networking wrapper only uses dispatch
            REQUIRE(false);;
        }

        template<class F, class Alloc>
        void
            defer(F&& f, Alloc const& a)
        {
            // shouldn't be called since the enclosing
            // networking wrapper only uses dispatch
            REQUIRE(false);
        }
#endif // !defined(ASIO_NO_TS_EXECUTORS)
    };
}

TEST_CASE("testBindHandler", "bind_handler") {
    using m1 = move_arg<1>;
    using m2 = move_arg<2>;

    {
        using namespace std::placeholders;
        using namespace boost::beast;
        // 0-ary
        bind_handler(test_cb{})();

        // 1-ary
        bind_handler(test_cb{}, 42)();
        bind_handler(test_cb{}, _1)(42);
        bind_handler(test_cb{}, _2)(0, 42);

        // 2-ary
        bind_handler(test_cb{}, 42, "s")();
        bind_handler(test_cb{}, 42, "s")(0);
        bind_handler(test_cb{}, _1, "s")(42);
        bind_handler(test_cb{}, 42, _1) ("s");
        bind_handler(test_cb{}, _1, _2)(42, "s");
        bind_handler(test_cb{}, _1, _2)(42, "s", "X");
        bind_handler(test_cb{}, _2, _1)("s", 42);
        bind_handler(test_cb{}, _3, _2)("X", "s", 42);

        // 3-ary
        bind_handler(test_cb{}, 42, "s")(m1{});
        bind_handler(test_cb{}, 42, "s", _1)(m1{});
        bind_handler(test_cb{}, 42, _1, m1{})("s");

        // 4-ary
        bind_handler(test_cb{}, 42, "s")(m1{}, m2{});
        bind_handler(test_cb{}, 42, "s", m1{})(m2{});
        bind_handler(test_cb{}, 42, "s", m1{}, m2{})();
        bind_handler(test_cb{}, 42, _1, m1{})("s", m2{});
        bind_handler(test_cb{}, _3, _1, m1{})("s", m2{}, 42);
    }
}

TEST_CASE("testBindHandler perfect forwarding", "bind_handler") {
    std::shared_ptr<int> const sp =
        std::make_shared<int>(42);
    {
        boost::beast::bind_handler(test_cb{}, sp)();
        REQUIRE(sp.get() != nullptr);
    }
    {
        boost::beast::bind_handler(test_cb{})(sp);
        REQUIRE(sp.get() != nullptr);
    }
}

TEST_CASE("testBindHandler associated executor", "bind_handler") {
    net::io_context ioc;
    bind_handler_test test;
    test.testHooks(ioc, boost::beast::bind_handler(net::bind_executor(
        test_executor(test, ioc), test_cb{})));
}

TEST_CASE("testBindHandler asio_handler_invoke", "bind_handler") {
    // make sure things compile, also can set a
    // breakpoint in asio_handler_invoke to make sure
    // it is instantiated.
    net::io_context ioc;
    net::strand<
        net::io_context::executor_type> s{
            ioc.get_executor() };
    net::post(s,
        boost::beast::bind_handler(test_cb{}, 42));
    ioc.run();
}

TEST_CASE("testBindHandler legacy hooks", "bind_handler") {
    boost::beast::legacy_handler::test(
        [](boost::beast::legacy_handler h)
        {
            return boost::beast::bind_handler(h);
        });
}

TEST_CASE("testBindFrontHandler", "bind_handler") {
    using m1 = move_arg<1>;
    using m2 = move_arg<2>;

    using namespace boost::beast;
    // 0-ary
    bind_front_handler(test_cb{})();

    // 1-ary
    bind_front_handler(test_cb{}, 42)();
    bind_front_handler(test_cb{})(42);

    // 2-ary
    bind_front_handler(test_cb{}, 42, "s")();
    bind_front_handler(test_cb{}, 42)("s");
    bind_front_handler(test_cb{})(42, "s");

    // 3-ary
    bind_front_handler(test_cb{}, 42, "s", m1{})();
    bind_front_handler(test_cb{}, 42, "s")(m1{});
    bind_front_handler(test_cb{}, 42)("s", m1{});
    bind_front_handler(test_cb{})(42, "s", m1{});

    // 4-ary
    bind_front_handler(test_cb{}, 42, "s", m1{}, m2{})();
    bind_front_handler(test_cb{}, 42, "s", m1{})(m2{});
    bind_front_handler(test_cb{}, 42, "s")(m1{}, m2{});
    bind_front_handler(test_cb{}, 42)("s", m1{}, m2{});
    bind_front_handler(test_cb{})(42, "s", m1{}, m2{});

    error_code ec;
    std::size_t n = 256;

    // void(error_code, size_t)
    bind_front_handler(test_cb{}, ec, n)();

    // void(error_code, size_t)(string_view)
    bind_front_handler(test_cb{}, ec, n)("s");
}

TEST_CASE("testBindFrontHandler associated executor", "bind_handler") {
    net::io_context ioc;
    bind_handler_test test;
    using m1 = move_arg<1>;
    using m2 = move_arg<2>;
    using namespace boost::beast;

    test.testHooks(ioc, bind_front_handler(net::bind_executor(
        test_executor(test, ioc), test_cb{})
    ));
    test.testHooks(ioc, bind_front_handler(net::bind_executor(
        test_executor(test, ioc), test_cb{}),
        42));
    test.testHooks(ioc, bind_front_handler(net::bind_executor(
        test_executor(test, ioc), test_cb{}),
        42, "s"));
    test.testHooks(ioc, bind_front_handler(net::bind_executor(
        test_executor(test, ioc), test_cb{}),
        42, "s", m1{}));
    test.testHooks(ioc, bind_front_handler(net::bind_executor(
        test_executor(test, ioc), test_cb{}),
        42, "s", m1{}, m2{}));

    error_code ec;
    std::size_t n = 256;
    test.testHooks(ioc, bind_front_handler(net::bind_executor(
        test_executor(test, ioc), test_cb{}),
        ec, n));
}

TEST_CASE("testBindFrontHandler legacy hooks 1", "bind_handler") {
    boost::beast::legacy_handler::test(
        [](boost::beast::legacy_handler h)
        {
            return boost::beast::bind_front_handler(h);
        });
}

TEST_CASE("testBindFrontHandler legacy hooks 2", "bind_handler") {
    boost::beast::legacy_handler::test(
        [](boost::beast::legacy_handler h)
        {
            return boost::beast::bind_front_handler(
                h, std::error_code{}, std::size_t{});
        });
}
