#include <algorithm>
#include <string>
#include <vdf_parser.hpp>

namespace rc::detail
{
bool operator==(const tyti::vdf::object &rhs, const tyti::vdf::object &lhs)
{
    if (rhs.name != lhs.name)
        return false;
    return true;
    for (const auto &[k, v] : rhs.attribs)
    {
        auto itr = lhs.attribs.find(k);
        if (itr == lhs.attribs.end())
            return false;
        if (itr->second != v)
            return false;
    }
    for (const auto &[k, v] : rhs.childs)
    {
        auto itr = lhs.childs.find(k);
        if (itr == lhs.childs.end())
            return false;

#ifdef _MSC_VER
// suppress warning about recursive call of operator==. This is here
// by intention
#pragma warning(disable : 5232)
#endif
        if (itr->second != v)
        {
            if (itr->second == nullptr || v == nullptr || *(itr->second) != *v)
                return false;
        }
#ifdef _MSC_VER
#pragma warning(default : 5232)
#endif
    }

    return true;
}

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
} // namespace rc::detail

#include <rapidcheck.h>

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

int main()
{
    using namespace tyti;
    rc::check("serializing and then parsing just the name with default options "
              "should return the original name",
              [](std::string n)
              {
                  vdf::object obj;
                  obj.name = n;

                  std::stringstream sstr;
                  vdf::write(sstr, obj);

                  auto to_test = vdf::read(sstr);
                  RC_ASSERT(obj.name == to_test.name);
              });

    rc::check("check if the attributes are also written and parsed correctly",
              [](const vdf::object &in)
              {
                  std::stringstream sstr;
                  vdf::write(sstr, in);
                  auto to_test = tyti::vdf::read(sstr);
                  RC_ASSERT(in == to_test);
              });

    rc::check("single line comment should not cause any errors",
              [](const std::string &comment)
              {
                  RC_PRE(comment.find("\n") == comment.npos);

                  std::stringstream input;
                  input << "\"test\""
                        << "{"
                        << "//" << comment << "\n"
                        << "\"key\" \"value\"\n"
                        << "}";
                  auto to_test = vdf::read(input);
                  RC_ASSERT("test" == to_test.name);
                  RC_ASSERT(1 == to_test.attribs.size());
                  RC_ASSERT("value" == to_test.attribs["key"]);
              });

    rc::check("multi line comment should not cause any errors",
              [](const std::string &comment)
              {
                  RC_PRE(comment.find("*/") == comment.npos);

                  std::stringstream input;
                  input << "\"test\""
                        << "{"
                        << "/*" << comment << "*/"
                        << "\"key\" \"value\"\n"
                        << "}";
                  auto to_test = vdf::read(input);
                  RC_ASSERT("test" == to_test.name);
                  RC_ASSERT(1 == to_test.attribs.size());
                  RC_ASSERT("value" == to_test.attribs["key"]);
              });

    return 0;
}
