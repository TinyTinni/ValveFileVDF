#include "generators/vdf_multiobject_generator.hpp"
#include "generators/vdf_object_generator.hpp"
#include <algorithm>
#include <string>
#include <vdf_parser.hpp>

bool containsSurrogate(const std::wstring &str)
{
    return str.find(wchar_t(-1)) != str.npos;
}

int main()
{

    ////////////////////////////////////////////////////////////////
    // object parsing tests

    using namespace tyti;
    bool success = true;
    success &= rc::check(
        "serializing and then parsing just the name with default options "
        "should return the original name",
        []()
        {
            vdf::object obj;
            obj.name = *rc::gen::string<std::string>();

            std::stringstream sstr;
            vdf::write(sstr, obj);

            auto to_test = vdf::read(sstr);
            RC_ASSERT(obj.name == to_test.name);
        });

    success &= rc::check(
        "serializing and then parsing just the name with default options "
        "should return the original name - not escaped",
        []()
        {
            vdf::object obj;
            obj.name = *rc::gen::suchThat(rc::gen::string<std::string>(),
                                          [](const std::string &str) {
                                              return str.find("\"") == str.npos;
                                          });

            vdf::WriteOptions writeOpts;
            writeOpts.escape_symbols = false;

            vdf::Options readOpts;
            readOpts.strip_escape_symbols = false;

            std::stringstream sstr;
            vdf::write(sstr, obj, writeOpts);

            auto to_test = vdf::read(sstr, readOpts);
            RC_ASSERT(obj.name == to_test.name);
        });

    success &= rc::check(
        "serializing and then parsing just the name with default options "
        "should return the original name - wchar_t",
        []()
        {
            vdf::wobject obj;
            obj.name = *rc::gen::suchThat(rc::gen::string<std::wstring>(),
                                          [](const auto &str)
                                          { return !containsSurrogate(str); });

            std::wstringstream sstr;
            vdf::write(sstr, obj);

            auto to_test = vdf::read(sstr);
            RC_ASSERT(obj.name == to_test.name);
        });

    success &= rc::check(
        "serializing and then parsing just the name with default options "
        "should return the original name - not escaped - wchar_t",
        []()
        {
            vdf::wobject obj;
            obj.name =
                *rc::gen::suchThat(rc::gen::string<std::wstring>(),
                                   [](const std::wstring &str) {
                                       return str.find(L"\"") == str.npos &&
                                              !containsSurrogate(str);
                                   });

            vdf::WriteOptions writeOpts;
            writeOpts.escape_symbols = false;

            vdf::Options readOpts;
            readOpts.strip_escape_symbols = false;

            std::wstringstream sstr;
            vdf::write(sstr, obj, writeOpts);

            auto to_test = vdf::read(sstr, readOpts);
            RC_ASSERT(obj.name == to_test.name);
        });

    success &= rc::check(
        "check if the attributes are also written and parsed correctly",
        [](const vdf::object &in)
        {
            std::stringstream sstr;
            vdf::write(sstr, in);
            auto to_test = tyti::vdf::read(sstr);
            RC_ASSERT(in == to_test);
        });

    success &= rc::check(
        "check if the childs are also written and parsed correctly",
        [](vdf::object in)
        {
            // todo this just tests childs with depth 1
            using child_vec = std::vector<std::shared_ptr<vdf::object>>;
            child_vec childs =
                *rc::gen::container<child_vec>(rc::gen::makeShared<vdf::object>(
                    rc::gen::arbitrary<vdf::object>()));

            for (const auto &c : childs)
            {
                in.childs.emplace(c->name, c);
            }

            std::stringstream sstr;
            vdf::write(sstr, in);
            auto to_test = tyti::vdf::read(sstr);
            RC_ASSERT(in == to_test);
        });

    success &= rc::check(
        "check if the childs are also written and parsed correctly - multikey",
        [](vdf::multikey_object in)
        {
            // todo this just tests childs with depth 1
            using child_vec =
                std::vector<std::shared_ptr<vdf::multikey_object>>;
            child_vec childs = *rc::gen::container<child_vec>(
                rc::gen::makeShared<vdf::multikey_object>(
                    rc::gen::arbitrary<vdf::multikey_object>()));

            for (const auto &c : childs)
            {
                in.childs.emplace(c->name, c);
            }

            std::stringstream sstr;
            vdf::write(sstr, in);
            auto to_test = tyti::vdf::read<vdf::multikey_object>(sstr);
            RC_ASSERT(in == to_test);
        });

    ////////////////////////////////////////////////////////////////
    // comments parsing tests

    success &= rc::check("single line comment should not cause any errors",
                         []()
                         {
                             auto comment = *rc::gen::suchThat(
                                 rc::gen::string<std::string>(),
                                 [](const std::string &str)
                                 { return str.find("\n") == str.npos; });

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

    success &= rc::check("multi line comment should not cause any errors",
                         []()
                         {
                             auto comment = *rc::gen::suchThat(
                                 rc::gen::string<std::string>(),
                                 [](const std::string &str)
                                 { return str.find("*/") == str.npos; });

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

    return (success) ? 0 : 1;
}
