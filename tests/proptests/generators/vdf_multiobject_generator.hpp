#pragma once

#include <rapidcheck.h>
#include <vdf_parser.hpp>

namespace tyti::vdf
{
bool operator==(const tyti::vdf::multikey_object &rhs,
                const tyti::vdf::multikey_object &lhs);
bool operator==(const tyti::vdf::wmultikey_object &rhs,
                const tyti::vdf::wmultikey_object &lhs);
} // namespace tyti::vdf

namespace rc
{

template <typename charT>
struct Arbitrary<tyti::vdf::basic_multikey_object<charT>>
{
    static Gen<tyti::vdf::basic_multikey_object<charT>> arbitrary()
    {
        using obj = tyti::vdf::basic_multikey_object<charT>;
        return gen::build<obj>(gen::set(&obj::name), gen::set(&obj::attribs));
    }
};
} // namespace rc

namespace rc::detail
{

void showValue(tyti::vdf::multikey_object obj, std::ostream &os);

} // namespace rc::detail