#pragma once

#include <rapidcheck.h>
#include <vdf_parser.hpp>

#include "string_generator.hpp"

namespace tyti::vdf
{
bool operator==(const tyti::vdf::object &rhs, const tyti::vdf::object &lhs);
bool operator==(const tyti::vdf::wobject &rhs, const tyti::vdf::wobject &lhs);

} // namespace tyti::vdf

namespace rc
{
template <> struct Arbitrary<tyti::vdf::object>
{
    static Gen<tyti::vdf::object> arbitrary()
    {
        using obj = tyti::vdf::object;
        return gen::build<obj>(gen::set(&obj::name), gen::set(&obj::attribs));
    }
};

template <> struct Arbitrary<tyti::vdf::wobject>
{
    static Gen<tyti::vdf::wobject> arbitrary()
    {
        using obj = tyti::vdf::wobject;
        return gen::build<obj>(
            gen::set(&obj::name, gen_name_string<wchar_t>()),
            gen::set(
                &obj::attribs,

                rc::gen::container<
                    std::unordered_map<std::wstring, std::wstring>>(
                    gen_name_string<wchar_t>(), gen_name_string<wchar_t>())));
    }
};
} // namespace rc

namespace rc::details
{

void showValue(tyti::vdf::object obj, std::ostream &os);

void showValue(const std::tuple<tyti::vdf::object, tyti::vdf::object> &objs,
               std::ostream &os);
} // namespace rc::details