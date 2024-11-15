#pragma once

#include <rapidcheck.h>
#include <vdf_parser.hpp>

namespace tyti::vdf
{
bool operator==(const tyti::vdf::object &rhs, const tyti::vdf::object &lhs);

} // namespace tyti::vdf

namespace rc
{
template <> struct Arbitrary<tyti::vdf::object>
{
    static Gen<tyti::vdf::object> arbitrary()
    {
        using tyti::vdf::object;
        return gen::build<object>(gen::set(&object::name),
                                  gen::set(&object::attribs));
    }
};
} // namespace rc

namespace rc::details
{

void showValue(tyti::vdf::object obj, std::ostream &os);

void showValue(const std::tuple<tyti::vdf::object, tyti::vdf::object> &objs,
               std::ostream &os);
} // namespace rc::details