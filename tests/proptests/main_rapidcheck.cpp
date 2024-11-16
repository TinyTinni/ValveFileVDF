#define TYTI_NO_L_UNDEF
#include <vdf_parser.hpp>
#define T_L(x) TYTI_L(charT, x)

#include "generators/vdf_multiobject_generator.hpp"
#include "generators/vdf_object_generator.hpp"
#include <algorithm>
#include <string>

bool containsSurrogate(const std::wstring &str)
{
    return str.find(wchar_t(-1)) != str.npos;
}

////////////////////////////////////////////////////////////////
template <typename charT> std::basic_string<charT> genValidNameString();

template <> std::string genValidNameString<char>()
{
    return *rc::gen::string<std::string>();
}

template <> std::wstring genValidNameString<wchar_t>()
{
    return *rc::gen::suchThat(rc::gen::string<std::wstring>(),
                              [](const auto &str)
                              { return !containsSurrogate(str); });
}

////////////////////////////////////////////////////////////////
template <typename charT>
std::basic_string<charT> genValidUnescapedNameString();

template <> std::string genValidUnescapedNameString<char>()
{
    return *rc::gen::suchThat(rc::gen::string<std::string>(),
                              [](const std::string &str)
                              { return str.find("\"") == str.npos; });
}

template <> std::wstring genValidUnescapedNameString<wchar_t>()
{
    return *rc::gen::suchThat(
        rc::gen::string<std::wstring>(), [](const std::wstring &str)
        { return str.find(L"\"") == str.npos && !containsSurrogate(str); });
}

////////////////////////////////////////////////////////////////
template <typename T> constexpr std::string_view getName();

template <> constexpr std::string_view getName<tyti::vdf::multikey_object>()
{
    return "multikey_object";
}
template <> constexpr std::string_view getName<tyti::vdf::wmultikey_object>()
{
    return "wmultikey_object";
}

template <> constexpr std::string_view getName<tyti::vdf::object>()
{
    return "object";
}
template <> constexpr std::string_view getName<tyti::vdf::wobject>()
{
    return "wobject";
}

template <> constexpr std::string_view getName<char>() { return "char"; }

template <> constexpr std::string_view getName<wchar_t>() { return "wchar_t"; }
////////////////////////////////////////////////////////////////

template <typename charT, template <typename T> typename basic_obj>
bool executeTest(std::string_view test_name, auto test_f)
{
    using obj = basic_obj<charT>;
    auto f = [test_f = std::move(test_f)]()
    { test_f.template operator()<charT, obj>(); };

    return rc::check(std::string{test_name} + " - " +
                         std::string{getName<charT>()} + " - " +
                         std::string{getName<obj>()},
                     std::move(f));
}

bool forAllObjectPermutations(std::string_view test_name, auto test_f)
{
    using namespace tyti;
    bool ret = true;
    ret &= executeTest<char, vdf::basic_object>(test_name, std::move(test_f));

    ret &= executeTest<char, vdf::basic_multikey_object>(test_name,
                                                         std::move(test_f));
    ret &=
        executeTest<wchar_t, vdf::basic_object>(test_name, std::move(test_f));

    ret &= executeTest<wchar_t, vdf::basic_multikey_object>(test_name,
                                                            std::move(test_f));
    return ret;
}

bool forLimitedObjectPermutation(std::string_view test_name, auto test_f)
{
    using namespace tyti;
    bool ret = true;
    ret &= executeTest<char, vdf::basic_object>(test_name, std::move(test_f));

    ret &= executeTest<char, vdf::basic_multikey_object>(test_name,
                                                         std::move(test_f));
    // todo, remove surrogate to the wobject generator and it's
    // attributes for linux support
    // afterwards, use "forAllObjectPermutations" and delete this function
#if defined(WIN32)
    ret &=
        executeTest<wchar_t, vdf::basic_object>(test_name, std::move(test_f));

    ret &= executeTest<wchar_t, vdf::basic_multikey_object>(test_name,
                                                            std::move(test_f));
#endif
    return ret;
}

int main()
{

    ////////////////////////////////////////////////////////////////
    // object parsing tests
    using namespace tyti;
    bool success = true;

    success &= forAllObjectPermutations(
        "serializing and then parsing just the name with default options "
        "should return the original name",
        []<typename charT, typename objType>()
        {
            objType obj;
            obj.name = genValidNameString<charT>();

            std::basic_stringstream<charT> sstr;
            vdf::write(sstr, obj);

            auto to_test = vdf::read<objType>(sstr);
            RC_ASSERT(obj.name == to_test.name);
        });

    success &= forAllObjectPermutations(
        "serializing and then parsing just the name with default options "
        "should return the original name - not escaped",
        []<typename charT, typename objType>()
        {
            objType obj;
            obj.name = genValidUnescapedNameString<charT>();

            vdf::WriteOptions writeOpts;
            writeOpts.escape_symbols = false;

            vdf::Options readOpts;
            readOpts.strip_escape_symbols = false;

            std::basic_stringstream<charT> sstr;
            vdf::write(sstr, obj, writeOpts);

            auto to_test = vdf::read<objType>(sstr, readOpts);
            RC_ASSERT(obj.name == to_test.name);
        });

    success &= forLimitedObjectPermutation(
        "check if the attributes are also written and parsed correctly",
        []<typename charT, typename objType>()
        {
            objType in = *rc::gen::arbitrary<objType>();
            std::basic_stringstream<charT> sstr;
            vdf::write(sstr, in);
            auto to_test = tyti::vdf::read<objType>(sstr);
            RC_ASSERT(in == to_test);
        });

    success &= forLimitedObjectPermutation(
        "check if the childs are also written and parsed correctly",
        []<typename charT, typename objType>()
        {
            objType in = *rc::gen::arbitrary<objType>();
            // todo this just tests childs with depth 1
            using child_vec = std::vector<std::shared_ptr<objType>>;
            child_vec childs = *rc::gen::container<child_vec>(
                rc::gen::makeShared<objType>(rc::gen::arbitrary<objType>()));

            for (const auto &c : childs)
            {
                in.childs.emplace(c->name, c);
            }

            std::basic_stringstream<charT> sstr;
            vdf::write(sstr, in);
            auto to_test = tyti::vdf::read<objType>(sstr);
            RC_ASSERT(in == to_test);
        });

    ////////////////////////////////////////////////////////////////
    // comments parsing tests

    success &= forAllObjectPermutations(
        "single line comment should not cause any errors",
        []<typename charT, typename objType>()
        {
            using string = std::basic_string<charT>;

            auto comment = *rc::gen::suchThat(
                rc::gen::string<string>(), [](const string &str)
                { return str.find(T_L("\n")) == str.npos; });

            std::basic_stringstream<charT> input;
            input << T_L("\"test\"") << T_L("{") << "//" << comment << T_L("\n")
                  << T_L("\"key\" \"value\"\n") << T_L("}");

            auto to_test = vdf::read<objType>(input);

            RC_ASSERT(string{T_L("test")} == to_test.name);
            RC_ASSERT(1 == to_test.attribs.size());
            auto finder = to_test.attribs.find(T_L("key"));
            RC_ASSERT(finder != to_test.attribs.end());
            RC_ASSERT(string{T_L("value")} == finder->second);
        });

    success &= forAllObjectPermutations(
        "multi line comment should not cause any errors",
        []<typename charT, typename objType>()
        {
            using string = std::basic_string<charT>;

            auto comment = *rc::gen::suchThat(
                rc::gen::string<string>(), [](const string &str)
                { return str.find(T_L("*/")) == str.npos; });

            std::basic_stringstream<charT> input;
            input << T_L("\"test\"") << T_L("{") << T_L("/*") << comment
                  << T_L("*/") << T_L("\"key\" \"value\"\n") << T_L("}");

            auto to_test = vdf::read<objType>(input);

            RC_ASSERT(string{T_L("test")} == to_test.name);
            RC_ASSERT(1 == to_test.attribs.size());
            auto finder = to_test.attribs.find(T_L("key"));
            RC_ASSERT(finder != to_test.attribs.end());
            RC_ASSERT(string{T_L("value")} == finder->second);
        });

    return (success) ? 0 : 1;
}
