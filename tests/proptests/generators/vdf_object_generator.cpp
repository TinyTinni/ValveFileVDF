#include "vdf_object_generator.hpp"

template <typename charT>
bool equal_impl(const tyti::vdf::basic_object<charT> &rhs,
                const tyti::vdf::basic_object<charT> &lhs)
{
    if (rhs.name != lhs.name)
        return false;
    if (rhs.attribs != lhs.attribs)
        return false;
    for (const auto &[k, v] : rhs.childs)
    {
        auto itr = lhs.childs.find(k);
        if (itr == lhs.childs.end())
        {
            return false;
        }

#ifdef _MSC_VER
// suppress warning about recursive call of operator==. This is here
// by intention
#pragma warning(disable : 5232)
#endif
        if (itr->second != v)
        {
            if (itr->second == nullptr || v == nullptr)
                return *(itr->second) != *v;
        }
#ifdef _MSC_VER
#pragma warning(default : 5232)
#endif
    }

    return true;
}

namespace tyti::vdf
{

bool operator==(const tyti::vdf::wobject &rhs, const tyti::vdf::wobject &lhs)
{
    return equal_impl(rhs, lhs);
}

bool operator==(const tyti::vdf::object &rhs, const tyti::vdf::object &lhs)
{
    return equal_impl(rhs, lhs);
}

} // namespace tyti::vdf

namespace rc::details
{
void showValue(tyti::vdf::object obj, std::ostream &os)
{
    os << "name: " << obj.name << "\n";
    os << "attribs (size:" << obj.attribs.size() << "): \n";
    for (const auto &[k, v] : obj.attribs)
        os << k << "\t" << v << "\n";
    os << "childs: (size:" << obj.childs.size() << "): \n";
    for (const auto &[k, v] : obj.childs)
    {
        os << k << "\t";
        if (v)
            showValue(*v, os);
        else
            os << "nullptr!";
        os << "\n";
    }
}

void showValue(const std::tuple<tyti::vdf::object, tyti::vdf::object> &objs,
               std::ostream &os)
{
    os << "[LHS]: \n";
    showValue(std::get<0>(objs), os);
    os << "[RHS]: \n";
    showValue(std::get<1>(objs), os);
}
} // namespace rc::details