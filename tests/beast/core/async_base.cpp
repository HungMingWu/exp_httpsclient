#include "catch.hpp"
#include "test_handler.hpp"
#include <boost/asio/io_context.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/beast/core/async_base.hpp>
#include "handler.hpp"
#include "stream.hpp"

namespace boost {
    namespace beast {

        namespace {

#if defined(BOOST_ASIO_NO_TS_EXECUTORS)

            static struct ex1_context : net::execution_context
            {

            } ex1ctx;

            struct ex1_type
            {

                net::execution_context&
                    query(net::execution::context_t c) const noexcept
                {
                    return *reinterpret_cast<net::execution_context*>(&ex1ctx);
                }

                net::execution::blocking_t
                    query(net::execution::blocking_t) const noexcept
                {
                    return net::execution::blocking;
                };

                net::execution::outstanding_work_t
                    query(net::execution::outstanding_work_t w) const noexcept
                {
                    return net::execution::outstanding_work;
                }

                ex1_type
                    require(net::execution::blocking_t::possibly_t b) const
                {
                    return *this;
                }

                ex1_type
                    require(net::execution::blocking_t::never_t b) const
                {
                    return *this;
                };

                ex1_type
                    prefer(net::execution::outstanding_work_t::untracked_t w) const
                {
                    return *this;
                };

                ex1_type
                    prefer(net::execution::outstanding_work_t::tracked_t w) const
                {
                    return *this;
                };

                template<class F>
                void
                    execute(F&&) const
                {}

                bool
                    operator==(ex1_type const&) const noexcept
                {
                    return true;
                }
                bool
                    operator!=(ex1_type const&) const noexcept
                {
                    return false;
                }
            };
            BOOST_STATIC_ASSERT(net::execution::is_executor<ex1_type>::value);
#else
            struct ex1_type
            {
                void* context() { return nullptr; }
                void on_work_started() {}
                void on_work_finished() {}
                template<class F> void dispatch(F&&) {}
                template<class F> void post(F&&) {}
                template<class F> void defer(F&&) {}
            };
            BOOST_STATIC_ASSERT(net::is_executor<ex1_type>::value);
#endif


            struct no_alloc
            {
            };

            struct nested_alloc
            {
                struct allocator_type
                {
                };
            };

            struct intrusive_alloc
            {
                struct allocator_type
                {
                };
            };

            struct no_ex
            {
                using executor_type = net::system_executor;
            };

            struct nested_ex
            {
                struct executor_type
                {
                };
            };

            struct intrusive_ex
            {
                struct executor_type
                {
                };
            };

            template<class E, class A>
            struct handler;

            template<>
            struct handler<no_ex, no_alloc>
            {
            };

            template<>
            struct handler<no_ex, nested_alloc>
                : nested_alloc
            {
            };

            template<>
            struct handler<no_ex, intrusive_alloc>
            {
            };

            template<>
            struct handler<nested_ex, no_alloc>
                : nested_ex
            {
            };

            template<>
            struct handler<intrusive_ex, no_alloc>
            {
            };

        } // (anon)

    } // beast
} // boost

template <typename alloc>
using no_ex_handler = boost::beast::handler<boost::beast::no_ex, alloc>;

using no_ex_alloc_handler = no_ex_handler<boost::beast::no_alloc>;
using no_ex_nestalloc_handler = no_ex_handler<boost::beast::nested_alloc>;
using no_ex_intrusivealloc_handler = no_ex_handler<boost::beast::intrusive_alloc>;
using nested_ex_noalloc_handler = boost::beast::handler<boost::beast::nested_ex, boost::beast::no_alloc>;
using intrusive_ex_noalloc_handler = boost::beast::handler<boost::beast::intrusive_ex, boost::beast::no_alloc>;

namespace boost {
    namespace asio {

        template<class Allocator>
        struct associated_allocator<
            boost::beast::handler<
            boost::beast::no_ex,
            boost::beast::intrusive_alloc>,
            Allocator>
        {
            using type =
                boost::beast::intrusive_alloc::allocator_type;

            static type get(
                boost::beast::handler<
                boost::beast::no_ex,
                boost::beast::intrusive_alloc> const&,
                Allocator const& = Allocator()) noexcept
            {
                return type{};
            }
        };

        template<class Executor>
        struct associated_executor<
            boost::beast::handler<
            boost::beast::intrusive_ex,
            boost::beast::no_alloc>,
            Executor>
        {
            using type =
                boost::beast::intrusive_ex::executor_type;

            static type get(
                boost::beast::handler<
                boost::beast::intrusive_ex,
                boost::beast::no_alloc> const&,
                Executor const& = Executor()) noexcept
            {
                return type{};
            }
        };

    } // asio
} // boost

static_assert(
    std::is_same_v<
        std::allocator<void>,
        boost::asio::associated_allocator_t<
            boost::beast::async_base<
                no_ex_alloc_handler,
                boost::asio::io_context::executor_type
            >
        >
    >);


static_assert(
    std::is_same_v<
        std::allocator<int>,
        boost::asio::associated_allocator_t<
            boost::beast::async_base<
                no_ex_alloc_handler,
                boost::asio::io_context::executor_type,
                std::allocator<int>
            >
        >
    >);

static_assert(
    std::is_same_v<
        std::allocator<void>,
        boost::asio::associated_allocator_t<
            boost::beast::async_base<
                no_ex_alloc_handler,
                boost::asio::io_context::executor_type
            >,
            std::allocator<int> // ignored
        >
    >);

static_assert(
    std::is_same_v<
        std::allocator<int>,
        boost::asio::associated_allocator_t<
            boost::beast::async_base<
                no_ex_alloc_handler,
                boost::asio::io_context::executor_type,
                std::allocator<int>
            >,
            std::allocator<double> // ignored
        >
    >);

// nested associated allocator

static_assert(
    std::is_same_v<
        boost::beast::nested_alloc::allocator_type,
        boost::asio::associated_allocator_t<
            boost::beast::async_base<
                no_ex_nestalloc_handler,
                boost::asio::io_context::executor_type
            >
        >
    >);

static_assert(
    std::is_same_v<
        boost::beast::nested_alloc::allocator_type,
        boost::asio::associated_allocator_t<
            boost::beast::async_base<
                no_ex_nestalloc_handler,
                boost::asio::io_context::executor_type,
                std::allocator<int> // ignored
            >
        >
    >);

static_assert(
    std::is_same_v<
        boost::beast::nested_alloc::allocator_type,
        boost::asio::associated_allocator_t<
            boost::beast::async_base<
                no_ex_nestalloc_handler,
                boost::asio::io_context::executor_type
            >,
            std::allocator<int> // ignored
        >
    >);

static_assert(
    std::is_same_v<
        boost::beast::nested_alloc::allocator_type,
        boost::asio::associated_allocator_t<
            boost::beast::async_base<
                no_ex_nestalloc_handler,
                boost::asio::io_context::executor_type,
                std::allocator<int> // ignored
            >,
            std::allocator<int> // ignored
        >
    >);

// intrusive associated allocator

static_assert(
    std::is_same_v<
        boost::beast::intrusive_alloc::allocator_type,
        boost::asio::associated_allocator_t<
            boost::beast::async_base<
                no_ex_intrusivealloc_handler,
                boost::asio::io_context::executor_type
            >
        >
    >);

static_assert(
    std::is_same_v<
        boost::beast::intrusive_alloc::allocator_type,
        boost::asio::associated_allocator_t<
            boost::beast::async_base<
                no_ex_intrusivealloc_handler,
                boost::asio::io_context::executor_type,
                std::allocator<int> // ignored
            >
        >
    >);

static_assert(
    std::is_same_v<
        boost::beast::intrusive_alloc::allocator_type,
        boost::asio::associated_allocator_t<
            boost::beast::async_base<
                no_ex_intrusivealloc_handler,
                boost::asio::io_context::executor_type
            >,
            std::allocator<int> // ignored
        >
    >);

static_assert(
    std::is_same_v<
        boost::beast::intrusive_alloc::allocator_type,
        boost::asio::associated_allocator_t<
            boost::beast::async_base<
                no_ex_intrusivealloc_handler,
                boost::asio::io_context::executor_type,
                std::allocator<int> // ignored
            >,
            std::allocator<int> // ignored
        >
    >);

// no associated executor

static_assert(
    std::is_same_v<
        boost::beast::ex1_type,
        boost::asio::associated_executor_t<
            boost::beast::async_base<
                no_ex_alloc_handler,
                boost::beast::ex1_type
            >
        >
    >);

static_assert(
    std::is_same_v<
        boost::beast::ex1_type,
        boost::asio::associated_executor_t<
            boost::beast::async_base<
                no_ex_alloc_handler,
                boost::beast::ex1_type
            >,
            boost::asio::system_executor // ignored
        >
    >);

// nested associated executor

static_assert(
    std::is_same_v<
        boost::beast::nested_ex::executor_type,
        boost::asio::associated_executor_t<
            boost::beast::async_base<
                nested_ex_noalloc_handler,
                boost::beast::ex1_type
            >
        >
    >);

static_assert(
    std::is_same_v<
        boost::beast::nested_ex::executor_type,
        boost::asio::associated_executor_t<
            boost::beast::async_base<
                nested_ex_noalloc_handler,
                boost::beast::ex1_type
            >,
            boost::asio::system_executor // ignored
        >
    >);

// intrusive associated executor

static_assert(
    std::is_same_v<
        boost::beast::intrusive_ex::executor_type,
        boost::asio::associated_executor_t<
            boost::beast::async_base<
                intrusive_ex_noalloc_handler,
                boost::beast::ex1_type
            >
        >
    >);

static_assert(
    std::is_same_v<
        boost::beast::intrusive_ex::executor_type,
        boost::asio::associated_executor_t<
            boost::beast::async_base<
                intrusive_ex_noalloc_handler,
                boost::beast::ex1_type
            >,
            boost::asio::system_executor // ignored
        >
    >);

TEST_CASE("testBase get_allocator", "async_base") {
	boost::beast::simple_allocator alloc;
	boost::beast::simple_allocator alloc2;
	boost::beast::async_base<
		boost::beast::move_only_handler,
		boost::beast::simple_executor,
		boost::beast::simple_allocator> op(
			boost::beast::move_only_handler{}, {}, alloc);
	REQUIRE(op.get_allocator() == alloc);
	REQUIRE(op.get_allocator() != alloc2);
}

TEST_CASE("testBase get_executor", "async_base") {
    boost::beast::simple_executor ex;
    boost::beast::simple_executor ex2;
    boost::beast::async_base<
        boost::beast::move_only_handler,
        boost::beast::simple_executor> op(
            boost::beast::move_only_handler{}, ex);
    REQUIRE(op.get_executor() == ex);
    REQUIRE(op.get_executor() != ex2);
}

TEST_CASE("testBase move construction", "async_base") {
    boost::beast::async_base<
        boost::beast::move_only_handler,
        boost::beast::simple_executor> op(
            boost::beast::move_only_handler{}, {});
    auto op2 = std::move(op);
}

TEST_CASE("testBase observers", "async_base") {
    bool b = false;
    boost::beast::async_base<
        boost::beast::legacy_handler,
        boost::beast::simple_executor> op(
            boost::beast::legacy_handler{ b }, {});
    REQUIRE(!op.handler().hook_invoked);
    b = true;
    REQUIRE(op.handler().hook_invoked);
    b = false;
    REQUIRE(!op.release_handler().hook_invoked);
}

TEST_CASE("testBase invocation 1", "async_base") {
    boost::asio::io_context ioc;
    boost::beast::async_base<
        boost::beast::test::handler,
        boost::asio::io_context::executor_type> op(
            boost::beast::test::any_handler(), ioc.get_executor());
    op.complete(true);
}

TEST_CASE("testBase invocation 2", "async_base") {
    boost::asio::io_context ioc;
    auto op = new
        boost::beast::async_base<
        boost::beast::test::handler,
        boost::asio::io_context::executor_type>(
            boost::beast::test::any_handler(), ioc.get_executor());
    op->complete(false);
    delete op;
    ioc.run();
}

TEST_CASE("testBase invocation 3", "async_base") {
    boost::beast::async_base<
        boost::beast::test::handler,
        boost::beast::simple_executor> op(
            boost::beast::test::any_handler(), {});
    op.complete_now();
}

TEST_CASE("testBase legacy hooks", "async_base") {
    boost::beast::legacy_handler::test(
        [](boost::beast::legacy_handler h)
        {
            return boost::beast::async_base<
                boost::beast::legacy_handler,
                boost::beast::simple_executor>(
                    std::move(h), {});
        });
}

TEST_CASE("StableBase get_allocator", "async_base") {
    boost::beast::simple_allocator alloc;
    boost::beast::simple_allocator alloc2;
    boost::beast::stable_async_base<
        boost::beast::move_only_handler,
        boost::beast::simple_executor,
        boost::beast::simple_allocator> op(
            boost::beast::move_only_handler{}, {}, alloc);
    REQUIRE(op.get_allocator() == alloc);
    REQUIRE(op.get_allocator() != alloc2);
}

TEST_CASE("StableBase get_executor", "async_base") {
    boost::beast::simple_executor ex;
    boost::beast::simple_executor ex2;
    boost::beast::stable_async_base<
        boost::beast::move_only_handler,
        boost::beast::simple_executor> op(
            boost::beast::move_only_handler{}, ex);
    REQUIRE(op.get_executor() == ex);
    REQUIRE(op.get_executor() != ex2);
}

TEST_CASE("StableBase move construction", "async_base") {
    boost::beast::stable_async_base<
        boost::beast::move_only_handler,
        boost::beast::simple_executor> op(
            boost::beast::move_only_handler{}, {});
    auto op2 = std::move(op);
}

TEST_CASE("StableBase invocation 1", "async_base") {
    boost::asio::io_context ioc;
    boost::beast::stable_async_base<
        boost::beast::test::handler,
        boost::asio::io_context::executor_type> op(
            boost::beast::test::any_handler(), ioc.get_executor());
    op.complete(true);
}

TEST_CASE("StableBase invocation 2", "async_base") {
    boost::asio::io_context ioc1;
    boost::asio::io_context ioc2;
    auto h = boost::asio::bind_executor(ioc2, boost::beast::test::any_handler());
    auto op = new boost::beast::stable_async_base<
        decltype(h),
        boost::asio::io_context::executor_type>(
            std::move(h),
            ioc1.get_executor());
    op->complete(false);
    delete op;
    REQUIRE(ioc1.run() == 0);
    REQUIRE(ioc2.run() == 1);
}

TEST_CASE("StableBase invocation 3", "async_base") {
    boost::beast::stable_async_base<
        boost::beast::test::handler,
        boost::beast::simple_executor> op(
            boost::beast::test::any_handler(), {});
    op.complete_now();
}

TEST_CASE("StableBase legacy hooks", "async_base") {
    boost::beast::legacy_handler::test(
        [](boost::beast::legacy_handler h)
        {
            return boost::beast::stable_async_base<
                boost::beast::legacy_handler,
                boost::beast::simple_executor>(
                    std::move(h), {});
        });
}

TEST_CASE("StableBase allocate_stable 1", "async_base") {
    bool destroyed = false;
    {
        struct data
        {
            bool& destroyed;

            ~data()
            {
                destroyed = true;
            }
        };
        boost::beast::stable_async_base<
            boost::beast::move_only_handler,
            boost::beast::simple_executor> op(
                boost::beast::move_only_handler{}, {});
        REQUIRE(!destroyed);
        auto& d = boost::beast::allocate_stable<data>(op, destroyed);
        REQUIRE(!d.destroyed);
    }
    REQUIRE(destroyed);
}

TEST_CASE("StableBase allocate_stable 2", "async_base") {
    struct throwing_data
    {
        throwing_data()
        {
            BOOST_THROW_EXCEPTION(
                std::exception{});
        }
    };
    boost::beast::stable_async_base<
        boost::beast::move_only_handler,
        boost::beast::simple_executor> op(
            boost::beast::move_only_handler{}, {});
    bool checked = false;
    try
    {
        boost::beast::allocate_stable<throwing_data>(op);
    }
    catch (std::exception const&)
    {
        checked = true;
    }
    REQUIRE(checked);
}

namespace {
    // Asynchronously read into a buffer until the buffer is full, or an error occurs
    template<class AsyncReadStream, BOOST_BEAST_ASYNC_TPARAM2 ReadHandler>
    typename boost::asio::async_result<ReadHandler, void(boost::system::error_code, std::size_t)>::return_type
        async_read(AsyncReadStream& stream, boost::asio::mutable_buffer buffer, ReadHandler&& handler)
    {
        using handler_type = BOOST_ASIO_HANDLER_TYPE(ReadHandler, void(error_code, std::size_t));
        using base_type = async_base<handler_type, typename AsyncReadStream::executor_type>;

        struct op : base_type
        {
            AsyncReadStream& stream_;
            boost::asio::mutable_buffer buffer_;
            std::size_t total_bytes_transferred_;

            op(
                AsyncReadStream& stream,
                boost::asio::mutable_buffer buffer,
                handler_type& handler)
                : base_type(std::move(handler), stream.get_executor())
                , stream_(stream)
                , buffer_(buffer)
                , total_bytes_transferred_(0)
            {
                (*this)({}, 0, false); // start the operation
            }

            void operator()(error_code ec, std::size_t bytes_transferred, bool is_continuation = true)
            {
                // Adjust the count of bytes and advance our buffer
                total_bytes_transferred_ += bytes_transferred;
                buffer_ = buffer_ + bytes_transferred;

                // Keep reading until buffer is full or an error occurs
                if (!ec && buffer_.size() > 0)
                    return stream_.async_read_some(buffer_, std::move(*this));

                // Call the completion handler with the result. If `is_continuation` is
                // false, which happens on the first time through this function, then
                // `net::post` will be used to call the completion handler, otherwise
                // the completion handler will be invoked directly.

                this->complete(is_continuation, ec, total_bytes_transferred_);
            }
        };

        boost::asio::async_completion<ReadHandler, void(boost::system::error_code, std::size_t)> init{ handler };
        op(stream, buffer, init.completion_handler);
        return init.result.get();
    }

    // Asynchronously send a message multiple times, once per second
    template <class AsyncWriteStream, class T, class WriteHandler>
    auto async_write_messages(
        AsyncWriteStream& stream,
        T const& message,
        std::size_t repeat_count,
        WriteHandler&& handler) ->
        typename boost::asio::async_result<
        typename std::decay<WriteHandler>::type,
        void(boost::system::error_code)>::return_type
    {
        using handler_type = typename net::async_completion<WriteHandler, void(error_code)>::completion_handler_type;
        using base_type = stable_async_base<handler_type, typename AsyncWriteStream::executor_type>;

        struct op : base_type, boost::asio::coroutine
        {
            // This object must have a stable address
            struct temporary_data
            {
                // Although std::string is in theory movable, most implementations
                // use a "small buffer optimization" which means that we might
                // be submitting a buffer to the write operation and then
                // moving the string, invalidating the buffer. To prevent
                // undefined behavior we store the string object itself at
                // a stable location.
                std::string const message;

                net::steady_timer timer;

                temporary_data(std::string message_, net::any_io_executor ex)
                    : message(std::move(message_))
                    , timer(std::move(ex))
                {
                }
            };

            AsyncWriteStream& stream_;
            std::size_t repeats_;
            temporary_data& data_;

            op(AsyncWriteStream& stream, std::size_t repeats, std::string message, handler_type& handler)
                : base_type(std::move(handler), stream.get_executor())
                , stream_(stream)
                , repeats_(repeats)
                , data_(allocate_stable<temporary_data>(*this,
                    std::move(message),
                    stream.get_executor()))
            {
                (*this)(); // start the operation
            }

            // Including this file provides the keywords for macro-based coroutines
            #include <boost/asio/yield.hpp>

            void operator()(error_code ec = {}, std::size_t = 0)
            {
                reenter(*this)
                {
                    // If repeats starts at 0 then we must complete immediately. But
                    // we can't call the final handler from inside the initiating
                    // function, so we post our intermediate handler first. We use
                    // net::async_write with an empty buffer instead of calling
                    // net::post to avoid an extra function template instantiation, to
                    // keep compile times lower and make the resulting executable smaller.
                    yield net::async_write(stream_, net::const_buffer{}, std::move(*this));
                    while (!ec && repeats_-- > 0)
                    {
                        // Send the string. We construct a `const_buffer` here to guarantee
                        // that we do not create an additional function template instantation
                        // of net::async_write, since we already instantiated it above for
                        // net::const_buffer.

                        yield net::async_write(stream_,
                            net::const_buffer(net::buffer(data_.message)), std::move(*this));
                        if (ec)
                            break;

                        // Set the timer and wait
                        data_.timer.expires_after(std::chrono::seconds(1));
                        yield data_.timer.async_wait(std::move(*this));
                    }
                }

                // The base class destroys the temporary data automatically,
                // before invoking the final completion handler
                this->complete_now(ec);
            }

            // Including this file undefines the macros for the coroutines
            #include <boost/asio/unyield.hpp>
        };

        net::async_completion<WriteHandler, void(error_code)> completion(handler);
        std::ostringstream os;
        os << message;
        op(stream, repeat_count, os.str(), completion.completion_handler);
        return completion.result.get();
    }
}

TEST_CASE("testJavadocs", "async_base") {
    struct handler
    {
        void operator()(boost::system::error_code = {}, std::size_t = 0)
        {
        }
    };

    //REQUIRE(&async_read<boost::beast::test::stream, handler>);
    //REQUIRE(&async_write_messages<boost::beast::test::stream, std::string, handler>);
}