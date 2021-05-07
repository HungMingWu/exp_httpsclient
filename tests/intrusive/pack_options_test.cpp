#include <boost/intrusive/pack_options.hpp>
#include <boost/intrusive/detail/mpl.hpp>

struct empty_default {};

using namespace boost::intrusive;

//Test BOOST_INTRUSIVE_OPTION_CONSTANT
BOOST_INTRUSIVE_OPTION_CONSTANT(incremental, bool, Enabled, is_incremental)
constexpr bool is_incremental_value = pack_options<empty_default, incremental<true>>::type::is_incremental;
static_assert(is_incremental_value == true);

//Test BOOST_INTRUSIVE_OPTION_TYPE
BOOST_INTRUSIVE_OPTION_TYPE(my_pointer, VoidPointer, std::remove_pointer_t<VoidPointer>, my_pointer_type)
typedef pack_options< empty_default, my_pointer<void*> >::type::my_pointer_type my_pointer_type;
static_assert(std::is_same_v<my_pointer_type, void>);

//test combination of BOOST_INTRUSIVE_OPTION_CONSTANT and BOOST_INTRUSIVE_OPTION_TYPE
//    First add new options
struct default_options
{
    static const long long_constant = -3;
    typedef double* double_typedef;
};

BOOST_INTRUSIVE_OPTION_CONSTANT(incremental2, bool, Enabled, is_incremental2)
BOOST_INTRUSIVE_OPTION_TYPE(my_pointer2, VoidPointer, typename std::add_pointer<VoidPointer>::type, my_pointer_type2)

typedef pack_options < default_options
    , incremental<false>
    , my_pointer<float*>
    , incremental2<true>
    , my_pointer2<const char*>
>::type combined_type;
static_assert(combined_type::is_incremental == false);
static_assert(combined_type::is_incremental2 == true);
static_assert(std::is_same_v<combined_type::my_pointer_type, float>);
static_assert(std::is_same_v<combined_type::my_pointer_type2, const char**>);

//test packing the default options leads to a default options type
static_assert(std::is_same_v<pack_options<default_options>::type, default_options>);

