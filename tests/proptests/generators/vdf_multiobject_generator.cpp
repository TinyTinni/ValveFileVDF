#include "vdf_multiobject_generator.hpp"

template <typename charT>
bool equal_impl(const tyti::vdf::basic_multikey_object<charT> &rhs,
                const tyti::vdf::basic_multikey_object<charT> &lhs)
{
    if (rhs.name != lhs.name)
        return false;
    if (rhs.attribs != lhs.attribs)
        return false;
    for (const auto &[k, v] : rhs.childs)
    {
        auto [begin, end] = lhs.childs.equal_range(k);

#ifdef _MSC_VER
// suppress warning about recursive call of operator==. This is here
// by intention
#pragma warning(disable : 5232)
#endif
        while (begin != end)
        {
            if (begin->second == nullptr && v == nullptr)
                return true;
            if (*(begin->second) == *v)
                return true;
            ++begin;
        }
        return false;
#ifdef _MSC_VER
#pragma warning(default : 5232)
#endif
    }

    return true;
}
namespace tyti::vdf
{

bool operator==(const tyti::vdf::multikey_object &rhs,
                const tyti::vdf::multikey_object &lhs)
{
    return equal_impl(rhs, lhs);
}
bool operator==(const tyti::vdf::wmultikey_object &rhs,
                const tyti::vdf::wmultikey_object &lhs)
{
    return equal_impl(rhs, lhs);
}

} // namespace tyti::vdf

namespace rc::detail
{

void showValue(tyti::vdf::multikey_object obj, std::ostream &os)
{
    os << "name: " << obj.name << "\n";
    os << "attribs (size:" << obj.attribs.size() << "): \n";
    for (const auto &[k, v] : obj.attribs)
        os << k << "\t" << v << "\n";
    os << "childs: (size:" << obj.childs.size() << "): \n";
    for (const auto &[k, v] : obj.childs)
    {
        os << "'" << k << "'\t'";
        if (v)
            showValue(*v, os);
        else
            os << "nullptr!";

        os << "'\n";
    }

    os << "'\n";
}

} // namespace rc::detail