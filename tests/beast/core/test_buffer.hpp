//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/boostorg/beast
//

#ifndef BOOST_BEAST_TEST_BUFFER_HPP
#define BOOST_BEAST_TEST_BUFFER_HPP

#include <boost/beast/core/detail/config.hpp>
#include <boost/beast/core/buffer_traits.hpp>
#include <boost/beast/core/buffers_to_string.hpp>
#include <boost/beast/core/string.hpp>
#include <boost/beast/core/detail/type_traits.hpp>
#include <asio/buffer.hpp>
#include <boost/assert.hpp>
#include <algorithm>
#include <string>
#include <type_traits>

namespace boost {
    namespace beast {

        /** A MutableBufferSequence for tests, where length is always 3.
        */
        class buffers_triple
        {
            net::mutable_buffer b_[3];

        public:
            using value_type = net::mutable_buffer;
            using const_iterator = net::mutable_buffer const*;

            buffers_triple(
                buffers_triple const&) = default;

            buffers_triple& operator=(
                buffers_triple const&) = default;

            buffers_triple(char* data, std::size_t size)
            {
                b_[0] = { data, size / 6 };
                data += b_[0].size();
                size -= b_[0].size();

                b_[1] = { data, 2 * size / 5 };
                data += b_[1].size();
                size -= b_[1].size();

                b_[2] = { data, size };

                BOOST_ASSERT(b_[0].size() > 0);
                BOOST_ASSERT(b_[1].size() > 0);
                BOOST_ASSERT(b_[2].size() > 0);
            }

            bool
                operator==(buffers_triple const& rhs) const noexcept
            {
                return
                    b_[0].data() == rhs.b_[0].data() &&
                    b_[0].size() == rhs.b_[0].size() &&
                    b_[1].data() == rhs.b_[1].data() &&
                    b_[1].size() == rhs.b_[1].size() &&
                    b_[2].data() == rhs.b_[2].data() &&
                    b_[2].size() == rhs.b_[2].size();
            }

            bool
                operator!=(buffers_triple const& rhs) const noexcept
            {
                return !(*this == rhs);
            }

            const_iterator
                begin() const noexcept
            {
                return &b_[0];
            }

            const_iterator
                end() const noexcept
            {
                return &b_[3];
            }
        };

        template<class ConstBufferSequence>
        std::size_t
            buffers_length(
                ConstBufferSequence const& buffers)
        {
            return std::distance(
                net::buffer_sequence_begin(buffers),
                net::buffer_sequence_end(buffers));
        }

        //------------------------------------------------------------------------------

        namespace detail {

            template<class MutableBufferSequence>
            void test_mutable_buffers(
                MutableBufferSequence const&,
                net::const_buffer)
            {
            }

            template<class MutableBufferSequence>
            void test_mutable_buffers(
                MutableBufferSequence const& b,
                net::mutable_buffer)
            {
                string_view src = "Hello, world!";
                BOOST_ASSERT(buffer_bytes(b) <= src.size());
                if (src.size() > buffer_bytes(b))
                    src = { src.data(), buffer_bytes(b) };
                net::buffer_copy(b, net::const_buffer(
                    src.data(), src.size()));
                REQUIRE(beast::buffers_to_string(b) == src);
            }

        } // detail

        /** Test an instance of a constant or mutable buffer sequence.
        */
        template<class ConstBufferSequence>
        void
            test_buffer_sequence(
                ConstBufferSequence const& buffers)
        {
            static_assert(
                net::is_const_buffer_sequence<
                ConstBufferSequence>::value);

            using iterator = decltype(
                net::buffer_sequence_begin(buffers));
            REQUIRE(sizeof(iterator) > 0);

            auto const size = buffer_bytes(buffers);
            REQUIRE(size > 0);

            // begin, end
            auto const length = std::distance(
                net::buffer_sequence_begin(buffers),
                net::buffer_sequence_end(buffers));
            REQUIRE(length > 0);
            REQUIRE(
                net::buffer_sequence_begin(buffers) !=
                net::buffer_sequence_end(buffers));

            // copy construction
            ConstBufferSequence b1(buffers);
            REQUIRE(buffer_bytes(b1) == size);

            // copy assignment
            ConstBufferSequence b2(buffers);
            b2 = b1;
            REQUIRE(buffer_bytes(b2) == size);

            // iterators
            {
                iterator it1{};
                iterator it2{};
                iterator it3 =
                    net::buffer_sequence_begin(buffers);
                iterator it4 =
                    net::buffer_sequence_end(buffers);
                REQUIRE(it1 == it2);
                REQUIRE(it1 != it3);
                REQUIRE(it3 != it1);
                REQUIRE(it1 != it4);
                REQUIRE(it4 != it1);
            }

            // bidirectional
            {
                auto const first =
                    net::buffer_sequence_begin(buffers);
                auto const last =
                    net::buffer_sequence_end(buffers);
                std::size_t n, m;
                iterator it;

                // pre-increment
                m = 0;
                n = length;
                for (it = first; n--; ++it)
                    m += buffer_bytes(*it);
                REQUIRE(it == last);
                REQUIRE(m == size);

                // post-increment
                m = 0;
                n = length;
                for (it = first; n--;)
                    m += buffer_bytes(*it++);
                REQUIRE(it == last);
                REQUIRE(m == size);

                // pre-decrement
                m = 0;
                n = length;
                for (it = last; n--;)
                    m += buffer_bytes(*--it);
                REQUIRE(it == first);
                REQUIRE(m == size);

                // post-decrement
                m = 0;
                n = length;
                for (it = last; n--;)
                {
                    it--;
                    m += buffer_bytes(*it);
                }
                REQUIRE(it == first);
                REQUIRE(m == size);
            }

            detail::test_mutable_buffers(buffers,
                buffers_type<ConstBufferSequence>{});
        }

        //------------------------------------------------------------------------------

        /** Metafunction to determine if a type meets the requirements of MutableDynamicBuffer_v0 */
        template <typename T>
        concept MutableDynamicBuffer = requires (T t) {
            { t.data() } -> std::convertible_to<typename T::const_buffers_type>;
            { t.data() } -> std::convertible_to<typename T::mutable_buffers_type>;
            { t.cdata() } -> std::convertible_to<typename T::const_buffers_type>;
        } && net::is_dynamic_buffer_v1<T>::value;

        /** @} */

        namespace detail {

            template<class MutableBufferSequence>
            void
                buffers_fill(
                    MutableBufferSequence const& buffers,
                    char c)
            {
                auto const end =
                    net::buffer_sequence_end(buffers);
                for (auto it = net::buffer_sequence_begin(buffers);
                    it != end; ++it)
                {
                    net::mutable_buffer b(*it);
                    std::fill(
                        static_cast<char*>(b.data()),
                        static_cast<char*>(b.data()) + b.size(), c);
                }
            }

            template<class MutableDynamicBuffer_v0>
            void test_mutable_dynamic_buffer(MutableDynamicBuffer_v0 const& b0)
            {
                if (MutableDynamicBuffer<MutableDynamicBuffer_v0>) {
                    static_assert(
                        net::is_mutable_buffer_sequence<typename
                        MutableDynamicBuffer_v0::mutable_buffers_type>::value);

                    static_assert(
                        std::is_convertible<
                        typename MutableDynamicBuffer_v0::mutable_buffers_type,
                        typename MutableDynamicBuffer_v0::const_buffers_type>::value);

                    string_view src = "Hello, world!";
                    if (src.size() > b0.max_size())
                        src = { src.data(), b0.max_size() };

                    // modify readable bytes
                    {
                        MutableDynamicBuffer_v0 b(b0);
                        auto const mb = b.prepare(src.size());
                        REQUIRE(buffer_bytes(mb) == src.size());
                        buffers_fill(mb, '*');
                        b.commit(src.size());
                        REQUIRE(b.size() == src.size());
                        REQUIRE(
                            beast::buffers_to_string(b.data()) ==
                            std::string(src.size(), '*'));
                        REQUIRE(
                            beast::buffers_to_string(b.cdata()) ==
                            std::string(src.size(), '*'));
                        auto const n = net::buffer_copy(
                            b.data(), net::const_buffer(
                                src.data(), src.size()));
                        REQUIRE(n == src.size());
                        REQUIRE(
                            beast::buffers_to_string(b.data()) == src);
                        REQUIRE(
                            beast::buffers_to_string(b.cdata()) == src);
                    }

                    // mutable to const sequence conversion
                    {
                        MutableDynamicBuffer_v0 b(b0);
                        b.commit(net::buffer_copy(
                            b.prepare(src.size()),
                            net::const_buffer(src.data(), src.size())));
                        auto mb = b.data();
                        auto cb = static_cast<
                            MutableDynamicBuffer_v0 const&>(b).data();
                        auto cbc = b.cdata();
                        REQUIRE(
                            beast::buffers_to_string(b.data()) == src);
                        REQUIRE(
                            beast::buffers_to_string(b.cdata()) == src);
                        beast::test_buffer_sequence(cb);
                        beast::test_buffer_sequence(cbc);
                        beast::test_buffer_sequence(mb);
                        {
                            decltype(mb)  mb2(mb);
                            mb = mb2;
                            decltype(cb)  cb2(cb);
                            cb = cb2;
                            decltype(cbc) cbc2(cbc);
                            cbc = cbc2;
                        }
                        {
                            decltype(cb)  cb2(mb);
                            decltype(cbc) cbc2(mb);
                            cb2 = mb;
                            cbc2 = mb;
                        }
                    }
                }
            }

        } // detail

        /** Test an instance of a dynamic buffer or mutable dynamic buffer.
        */
        void test_dynamic_buffer(const MutableDynamicBuffer auto & b0)
        {
            using type = std::decay_t<decltype(b0)>;
            static_assert(
                net::is_const_buffer_sequence<typename
                type::const_buffers_type>::value);

            static_assert(
                net::is_mutable_buffer_sequence<typename
                type::mutable_buffers_type>::value);

            REQUIRE(b0.size() == 0);
            REQUIRE(buffer_bytes(b0.data()) == 0);

            // members
            {
                string_view src = "Hello, world!";

                auto b1(b0);
                auto const mb = b1.prepare(src.size());
                b1.commit(net::buffer_copy(mb,
                    net::const_buffer(src.data(), src.size())));

                // copy constructor
                {
                    auto b2(b1);
                    REQUIRE(b2.size() == b1.size());
                    REQUIRE(
                        buffers_to_string(b1.data()) ==
                        buffers_to_string(b2.data()));

                    // https://github.com/boostorg/beast/issues/1621
                    b2.consume(1);
                    auto b3(b2);
                    REQUIRE(b3.size() == b2.size());
                    REQUIRE(
                        buffers_to_string(b2.data()) ==
                        buffers_to_string(b3.data()));
                }

                // move constructor
                {
                    auto b2(b1);
                    auto b3(std::move(b2));
                    REQUIRE(b3.size() == b1.size());
                    REQUIRE(
                        buffers_to_string(b3.data()) ==
                        buffers_to_string(b1.data()));
                }

                // copy assignment
                {
                    auto b2(b0);
                    b2 = b1;
                    REQUIRE(b2.size() == b1.size());
                    REQUIRE(
                        buffers_to_string(b1.data()) ==
                        buffers_to_string(b2.data()));

                    // self assignment
                    b2 = *&b2;
                    REQUIRE(b2.size() == b1.size());
                    REQUIRE(
                        buffers_to_string(b2.data()) ==
                        buffers_to_string(b1.data()));

                    // https://github.com/boostorg/beast/issues/1621
                    b2.consume(1);
                    auto b3(b2);
                    REQUIRE(b3.size() == b2.size());
                    REQUIRE(
                        buffers_to_string(b2.data()) ==
                        buffers_to_string(b3.data()));

                }

                // move assignment
                {
                    auto b2(b1);
                    auto b3(b0);
                    b3 = std::move(b2);
                    REQUIRE(b3.size() == b1.size());
                    REQUIRE(
                        buffers_to_string(b3.data()) ==
                        buffers_to_string(b1.data()));

                    // self move
                    b3 = std::move(b3);
                    REQUIRE(b3.size() == b1.size());
                    REQUIRE(
                        buffers_to_string(b3.data()) ==
                        buffers_to_string(b1.data()));
                }

                // swap
                {
                    auto b2(b1);
                    auto b3(b0);
                    REQUIRE(b2.size() == b1.size());
                    REQUIRE(b3.size() == b0.size());
                    using std::swap;
                    swap(b2, b3);
                    REQUIRE(b2.size() == b0.size());
                    REQUIRE(b3.size() == b1.size());
                    REQUIRE(
                        buffers_to_string(b3.data()) ==
                        buffers_to_string(b1.data()));
                }
            }

            // n == 0
            {
                auto b(b0);
                b.commit(1);
                REQUIRE(b.size() == 0);
                REQUIRE(buffer_bytes(b.prepare(0)) == 0);
                b.commit(0);
                REQUIRE(b.size() == 0);
                b.commit(1);
                REQUIRE(b.size() == 0);
                b.commit(b.max_size() + 1);
                REQUIRE(b.size() == 0);
                b.consume(0);
                REQUIRE(b.size() == 0);
                b.consume(1);
                REQUIRE(b.size() == 0);
                b.consume(b.max_size() + 1);
                REQUIRE(b.size() == 0);
            }

            // max_size
            {
                auto b(b0);
                bool checked = false;
                if (b.max_size() + 1 > b.max_size())
                {
                    try
                    {
                        b.prepare(b.max_size() + 1);
                    }
                    catch (std::length_error const&)
                    {
                        checked = true;
                    }
                    catch (...)
                    {
                    }
                }
                REQUIRE(checked);
            }

            // setup source buffer
            char buf[13];
            unsigned char k0 = 0;
            string_view src(buf, sizeof(buf));
            if (src.size() > b0.max_size())
                src = { src.data(), b0.max_size() };
            REQUIRE(b0.max_size() >= src.size());
            REQUIRE(b0.size() == 0);
            REQUIRE(buffer_bytes(b0.data()) == 0);
            auto const make_new_src =
                [&buf, &k0, &src]
            {
                auto k = k0++;
                for (std::size_t i = 0; i < src.size(); ++i)
                    buf[i] = k++;
            };

            // readable / writable buffer sequence tests
            {
                make_new_src();
                auto b(b0);
                auto const& bc(b);
                auto const mb = b.prepare(src.size());
                REQUIRE(buffer_bytes(mb) == src.size());
                beast::test_buffer_sequence(mb);
                b.commit(net::buffer_copy(mb,
                    net::const_buffer(src.data(), src.size())));
                REQUIRE(
                    buffer_bytes(bc.data()) == src.size());
                beast::test_buffer_sequence(bc.data());
            }

            // h = in size
            // i = prepare size
            // j = commit size
            // k = consume size
            for (std::size_t h = 1; h <= src.size(); ++h)
            {
                string_view in(src.data(), h);
                for (std::size_t i = 1; i <= in.size(); ++i) {
                    for (std::size_t j = 1; j <= i + 1; ++j) {
                        for (std::size_t k = 1; k <= in.size(); ++k) {
                            {
                                make_new_src();

                                auto b(b0);
                                auto const& bc(b);
                                net::const_buffer cb(in.data(), in.size());
                                while (cb.size() > 0)
                                {
                                    auto const mb = b.prepare(
                                        std::min<std::size_t>(i,
                                            b.max_size() - b.size()));
                                    auto const n = net::buffer_copy(mb,
                                        net::const_buffer(cb.data(),
                                            std::min<std::size_t>(j, cb.size())));
                                    b.commit(n);
                                    cb += n;
                                }
                                REQUIRE(b.size() == in.size());
                                REQUIRE(
                                    buffer_bytes(bc.data()) == in.size());
                                REQUIRE(beast::buffers_to_string(
                                    bc.data()) == in);
                                while (b.size() > 0)
                                    b.consume(k);
                                REQUIRE(buffer_bytes(bc.data()) == 0);
                            }
                        }
                    }
                }
            }

            // MutableDynamicBuffer_v0 refinement
            detail::test_mutable_dynamic_buffer(b0);
        }

    } // beast
} // boost

#endif
