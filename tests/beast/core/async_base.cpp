#include "catch.hpp"
#include "test_handler.hpp"
#include <boost/asio/io_context.hpp>
#include <boost/beast/core/async_base.hpp>

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

#if 0
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
#endif

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

#if 0
static_assert(
    !std::is_same_v<
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
#endif

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