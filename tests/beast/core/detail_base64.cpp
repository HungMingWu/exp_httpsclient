#include "catch.hpp"
#include <string>
#include <string_view>
#include <boost/beast/core/detail/base64.hpp>
namespace {
    using namespace boost::beast::detail;

    std::string
        base64_encode(std::string_view s)
    {
        std::string dest;
        dest.resize(base64::encoded_size(s.size()));
        dest.resize(base64::encode(&dest[0], s.data(), s.size()));
        return dest;
    }

    std::string
        base64_decode(std::string_view data)
    {
        std::string dest;
        dest.resize(base64::decoded_size(data.size()));
        auto const result = base64::decode(
            &dest[0], data.data(), data.size());
        dest.resize(result.first);
        return dest;
    }
    void
    check(std::string const& in, std::string const& out)
    {
        auto const encoded = base64_encode(in);
        REQUIRE(encoded == out);
        REQUIRE(base64_decode(encoded) == in);
    }
}

TEST_CASE("detail base64 test", "detail_base64")  {
    check("", "");
    check("f", "Zg==");
    check("fo", "Zm8=");
    check("foo", "Zm9v");
    check("foob", "Zm9vYg==");
    check("fooba", "Zm9vYmE=");
    check("foobar", "Zm9vYmFy");

    check(
        "Man is distinguished, not only by his reason, but by this singular passion from "
        "other animals, which is a lust of the mind, that by a perseverance of delight "
        "in the continued and indefatigable generation of knowledge, exceeds the short "
        "vehemence of any carnal pleasure."
        ,
        "TWFuIGlzIGRpc3Rpbmd1aXNoZWQsIG5vdCBvbmx5IGJ5IGhpcyByZWFzb24sIGJ1dCBieSB0aGlz"
        "IHNpbmd1bGFyIHBhc3Npb24gZnJvbSBvdGhlciBhbmltYWxzLCB3aGljaCBpcyBhIGx1c3Qgb2Yg"
        "dGhlIG1pbmQsIHRoYXQgYnkgYSBwZXJzZXZlcmFuY2Ugb2YgZGVsaWdodCBpbiB0aGUgY29udGlu"
        "dWVkIGFuZCBpbmRlZmF0aWdhYmxlIGdlbmVyYXRpb24gb2Yga25vd2xlZGdlLCBleGNlZWRzIHRo"
        "ZSBzaG9ydCB2ZWhlbWVuY2Ugb2YgYW55IGNhcm5hbCBwbGVhc3VyZS4="
    );
}