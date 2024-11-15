#pragma once

#include <rapidcheck.h>
#include <vdf_parser.hpp>

namespace tyti::vdf
{
bool operator==(const tyti::vdf::multikey_object &rhs,
                const tyti::vdf::multikey_object &lhs);
} // namespace tyti::vdf

namespace rc
{

template <> struct Arbitrary<tyti::vdf::multikey_object>
{
    static Gen<tyti::vdf::multikey_object> arbitrary()
    {
        using tyti::vdf::multikey_object;
        return gen::build<multikey_object>(gen::set(&multikey_object::name),
                                           gen::set(&multikey_object::attribs));
    }
};
} // namespace rc

namespace rc::detail
{

void showValue(tyti::vdf::multikey_object obj, std::ostream &os);

} // namespace rc::detail