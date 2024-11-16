#pragma once

#include <rapidcheck.h>
#include <vdf_parser.hpp>

#include "string_generator.hpp"

namespace tyti::vdf
{
bool operator==(const tyti::vdf::multikey_object &rhs,
                const tyti::vdf::multikey_object &lhs);
bool operator==(const tyti::vdf::wmultikey_object &rhs,
                const tyti::vdf::wmultikey_object &lhs);
} // namespace tyti::vdf

namespace rc
{

template <> struct Arbitrary<tyti::vdf::multikey_object>
{
    static Gen<tyti::vdf::multikey_object> arbitrary()
    {
        using obj = tyti::vdf::multikey_object;
        return gen::build<obj>(gen::set(&obj::name), gen::set(&obj::attribs));
    }
};

template <> struct Arbitrary<tyti::vdf::wmultikey_object>
{
    static Gen<tyti::vdf::wmultikey_object> arbitrary()
    {
        using obj = tyti::vdf::wmultikey_object;
        return gen::build<obj>(
            gen::set(&obj::name, gen_name_string<wchar_t>()),
            gen::set(
                &obj::attribs,

                rc::gen::container<
                    std::unordered_multimap<std::wstring, std::wstring>>(
                    gen_name_string<wchar_t>(), gen_name_string<wchar_t>())));
    }
};
} // namespace rc

namespace rc::detail
{

void showValue(tyti::vdf::multikey_object obj, std::ostream &os);

} // namespace rc::detail