#include <algorithm>
#include <iostream>
#include <sstream>
#include <string>

#define TYTI_NO_L_UNDEF
#include "../vdf_parser.hpp"
#define T_L(x) TYTI_L(charT, x)
using namespace tyti;

#include "catch.hpp"
template <typename charT>
void check_DST_AST(const vdf::basic_object<charT> &obj)
{
    CHECK(obj.name == T_L("AppState"));
    REQUIRE(obj.attribs.size() == 24);
    REQUIRE(obj.childs.size() == 4);

    CHECK(obj.attribs.at(T_L("appid")) == T_L("343050"));

    CHECK(obj.attribs.at(T_L("buildid")) == T_L("1101428"));
    CHECK(obj.attribs.at(T_L("#1_attrib")) == T_L("1"));
    CHECK(obj.attribs.at(T_L("emptyAttrib")) == T_L(""));
    CHECK(obj.attribs.at(T_L("escape_quote")) == T_L(R"("quote")"));
    CHECK(obj.attribs.at(T_L("no_quoted_attrib_support")) == T_L("yes"));
    // "C2017 can occur when the stringize operator is used with strings that
    // include escape sequences."
    // https://docs.microsoft.com/en-us/previous-versions/visualstudio/visual-studio-2013/29t70y03(v=vs.120)
#if !defined(_MSC_VER) || (_MSC_VER > 1800)
    CHECK(obj.attribs.at(T_L("escape_quote_backslash")) ==
          T_L("quote_with_other_escapes\\\"\\"));
    CHECK(obj.attribs.at(T_L("tab_escape")) == T_L("new\\ttab"));
    CHECK(obj.attribs.at(T_L("new_line_escape")) == T_L("new\\nline"));
#endif

    CHECK(obj.childs.at(T_L("UserConfig"))->name == T_L("UserConfig"));
    CHECK(obj.childs.at(T_L("UserConfig"))->childs.empty());

    const auto &inc = obj.childs.at(T_L("IncludedStuff"));
    CHECK(inc->name == T_L("IncludedStuff"));
    const auto &base = obj.childs.at(T_L("BaseInclude"));
    REQUIRE(base->attribs.size() == 1);
    CHECK(base->attribs.at(T_L("BaseAttrib")) == T_L("Yes"));
    CHECK(obj.attribs.at(T_L("another attribute with fancy space")) ==
          T_L("yay"));
}

template <typename charT>
void check_DST_AST_multikey(const vdf::basic_multikey_object<charT> &obj)
{
    CHECK(obj.name == T_L("AppState"));
    REQUIRE(obj.attribs.size() == 25);
    REQUIRE(obj.childs.size() == 4);

    CHECK(obj.attribs.find(T_L("appid"))->second == T_L("343050"));

    CHECK(obj.attribs.find(T_L("buildid"))->second == T_L("1101428"));
    CHECK(obj.attribs.find(T_L("#1_attrib"))->second == T_L("1"));
    CHECK(obj.attribs.find(T_L("emptyAttrib"))->second == T_L(""));
    CHECK(obj.attribs.find(T_L("no_quoted_attrib_support"))->second ==
          T_L("yes"));

    CHECK(obj.attribs.count(T_L("UpdateResult")) == 2);

    CHECK(obj.childs.find(T_L("UserConfig"))->second->name ==
          T_L("UserConfig"));
    CHECK(obj.childs.find(T_L("UserConfig"))->second->childs.empty());

    const auto &inc = obj.childs.find(T_L("IncludedStuff"))->second;
    CHECK(inc->name == T_L("IncludedStuff"));
    const auto &base = obj.childs.find(T_L("BaseInclude"))->second;
    REQUIRE(base->attribs.size() == 1);
    CHECK(base->attribs.find(T_L("BaseAttrib"))->second == T_L("Yes"));
    CHECK(obj.attribs.find(T_L("another attribute with fancy space"))->second ==
          T_L("yay"));
}

template <typename charT> void read_check_DST_file_ok()
{
    std::basic_ifstream<charT> file("DST_Manifest.acf");
    bool ok;
    auto objects = vdf::read(file, &ok);

    REQUIRE(ok);
    auto it = objects.childs.find(T_L("AppState"));
    CHECK(it != objects.childs.end());
    check_DST_AST(*(it->second));
}

template <typename charT> void read_check_DST_file_ec()
{
    std::basic_ifstream<charT> file("DST_Manifest.acf");
    std::error_code ec;
    auto objects = vdf::read(file, ec);

    REQUIRE(!ec);
    auto it = objects.childs.find(T_L("AppState"));
    CHECK(it != objects.childs.end());
    check_DST_AST(*(it->second));
}

template <typename charT> void read_check_DST_file_throw()
{
    std::basic_ifstream<charT> file("DST_Manifest.acf");
    auto objects = vdf::read(file);
    auto it = objects.childs.find(T_L("AppState"));
    CHECK(it != objects.childs.end());
    check_DST_AST(*(it->second));
}

template <typename charT> void read_check_DST_file_multikey_throw()
{
    std::basic_ifstream<charT> file("DST_Manifest.acf");
    auto objects = vdf::read<vdf::basic_multikey_object<charT>>(file);
    auto it = objects.childs.find(T_L("AppState"));
    CHECK(it != objects.childs.end());
    check_DST_AST_multikey(*(it->second));
}

TEST_CASE("Read File", "[read]")
{
    read_check_DST_file_ok<char>();
    read_check_DST_file_ok<wchar_t>();
    read_check_DST_file_ec<char>();
    read_check_DST_file_ec<wchar_t>();
    read_check_DST_file_throw<char>();
    read_check_DST_file_throw<wchar_t>();
}

template <typename charT> void read_string()
{
    std::basic_string<charT> attribs(
        T_L("\"firstNode\"{\"SecondNode\"{\"Key\" \"Value\" //myComment\n}}"));
    bool ok;
    vdf::read(attribs.begin(), attribs.end(), &ok);

    REQUIRE(ok);
}

template <typename charT> void check_string(const vdf::basic_object<charT> &obj)
{
    CHECK(obj.name == T_L("firstNode"));

    CHECK(obj.attribs.empty() == true);
    REQUIRE(obj.childs.size() == 1);
    const auto &secondNode = obj.childs.at(T_L("SecondNode"));

    CHECK(secondNode->name == T_L("SecondNode"));
    REQUIRE(secondNode->attribs.size() == 1);
    CHECK(secondNode->childs.empty() == true);
    CHECK(secondNode->attribs.at(T_L("Key")) == T_L("Value"));
}

TEST_CASE("Read String", "[read]")
{
    read_string<char>();
    read_string<wchar_t>();
}

template <typename charT> void check_fail()
{
    bool ok;
    std::basic_string<charT> attribs(
        T_L("\"firstNode\"{\"SecondNode\"{\"Key\" //myComment\n}}"));
    vdf::read(attribs.begin(), attribs.end(), &ok);

    REQUIRE(!ok);
}
// todo: error checking
TEST_CASE("Find Error", "[read_error]")
{
    check_fail<char>();
    check_fail<wchar_t>();
}

template <typename charT> void write_and_read()
{
    std::basic_string<charT> attribs(
        T_L("\"firstNode\"{\"SecondNode\"{\"Key\" \"Value\" //myComment\n}}"));
    bool ok;
    auto objs = vdf::read(attribs.begin(), attribs.end(), &ok);

    REQUIRE(ok);

    std::basic_stringstream<charT> output;
    vdf::write(output, objs);
    objs = vdf::read(output);

    check_string(objs);
}

TEST_CASE("Write and Read", "[read_write]")
{
    write_and_read<char>();
    write_and_read<wchar_t>();
}

TEST_CASE("read multikey", "[read]")
{
    read_check_DST_file_multikey_throw<char>();
    read_check_DST_file_multikey_throw<wchar_t>();
}

template <typename charT> void read_broken_file()
{
    std::basic_ifstream<charT> file("broken_file.acf");
    std::error_code ec;
    auto objects = vdf::read(file, ec);
    REQUIRE(ec);
    REQUIRE(objects.name.empty());
    REQUIRE(objects.attribs.empty());
    REQUIRE(objects.childs.empty());
}

template <typename charT> void read_broken_file_throw()
{
    std::basic_ifstream<charT> file("broken_file.acf");
    auto objects = vdf::read(file);
}

TEST_CASE("read broken file", "[read]")
{
    read_broken_file<char>();
#ifdef WIN32
    read_broken_file<wchar_t>();
#endif
}

TEST_CASE("read broken file throw", "[read]")
{
    CHECK_THROWS(read_broken_file_throw<char>());
#ifdef WIN32
    CHECK_THROWS(read_broken_file_throw<wchar_t>());
#endif
}

TEST_CASE("issue14", "[read]")
{
    std::ifstream input_file("issue14.vdf", std::ios::in);
    CHECK_THROWS(tyti::vdf::read(input_file));
}

/////////////////////////////////////////////////////////////
// readme test
/////////////////////////////////////////////////////////////

struct counter
{
    size_t num_attributes;
    counter() : num_attributes(0) {}
    void add_attribute(std::string key, std::string value) { ++num_attributes; }
    void add_child(std::unique_ptr<counter> child)
    {
        num_attributes += child->num_attributes;
    }
    void set_name(std::string n) {}
};

TEST_CASE("counter test", "[counter]")
{
    std::ifstream file("DST_Manifest.acf");
    counter num = tyti::vdf::read<counter>(file);
    CHECK(num.num_attributes == 29);
}

/////////////////////////////////////////////////////////////
// fuzzer findings
/////////////////////////////////////////////////////////////

TEST_CASE("fuzzing_endless_loop", "[fuzzing]")
{
    std::string test_corpus{u8R"(-�
/
{)"};
    bool ok;
    tyti::vdf::Options opt;
    opt.ignore_includes = true;
    auto result =
        tyti::vdf::read(test_corpus.begin(), test_corpus.end(), &ok, opt);
    CHECK_FALSE(ok);
}
