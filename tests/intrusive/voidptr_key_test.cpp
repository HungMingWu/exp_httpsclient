#include "catch.hpp"
#include <boost/intrusive/options.hpp>
#include <boost/intrusive/set.hpp>
#include <boost/intrusive/set_hook.hpp>
#include <boost/config.hpp>
#include <functional> // std::less

// The test verifies that the set implementation does not use void* as auxiliary arguments for SFINAE
// in internal functions, which would make overload resolution ambiguous if user's key type is also void*.

using set_element_hook_t = boost::intrusive::set_base_hook<
    boost::intrusive::link_mode< boost::intrusive::link_mode_type::safe_link >,
    boost::intrusive::tag< struct for_set_element_lookup_by_key >,
    boost::intrusive::optimize_size< true >
>;

struct set_element :
    public set_element_hook_t
{
    struct order_by_key
    {
        bool operator() (set_element const& left, set_element const& right) const
        {
            return std::less< void* >()(left.m_key, right.m_key);
        }
        bool operator() (void* left, set_element const& right) const
        {
            return std::less< void* >()(left, right.m_key);
        }
        bool operator() (set_element const& left, void* right) const
        {
            return std::less< void* >()(left.m_key, right);
        }
    };

    void* m_key;

    explicit set_element(void* key) : m_key(key) {}

    set_element(set_element const&) = delete;
    set_element& operator=(set_element const&) = delete;
};

typedef boost::intrusive::set<
    set_element,
    boost::intrusive::base_hook< set_element_hook_t >,
    boost::intrusive::compare< set_element::order_by_key >,
    boost::intrusive::constant_time_size< true >
> set_t;

TEST_CASE("voidptr_key_test", "voidptr_key_test") {
    int v1 = 0, v2 = 1, v3 = 2;
    set_element e1(&v1), e2(&v2), e3(&v3);

    set_t s;
    s.insert(e1);
    s.insert(e2);

    set_t::iterator it = s.find(e1);
    REQUIRE(it != s.end());
    REQUIRE(&* it == &e1);

    it = s.find((void*)&v2, set_element::order_by_key());
    REQUIRE(it != s.end());
    REQUIRE(&* it == &e2);

    it = s.find(e3);
    REQUIRE(it == s.end());

    it = s.find((void*)&v3, set_element::order_by_key());
    REQUIRE(it == s.end());

    s.clear();
}