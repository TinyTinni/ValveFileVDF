#include <iostream>
#include <string>
#include <algorithm>
#include <sstream>

#define TYTI_NO_L_UNDEF
#include "../vdf_parser.hpp"
#define T_L(x) TYTI_L(charT,x)
using namespace tyti;

#include <catch.hpp>

const std::string testdata_dir = SOURCE_DIR "/testdata/";

#ifdef _WIN32
#include <direct.h>
#define cwd _getcwd
#define cd _chdir
#else
#include "unistd.h"
#define cwd getcwd
#define cd chdir
#endif

template<typename charT>
void check_DST_AST(const vdf::basic_object<charT>& obj)
{
    CHECK(obj.name == T_L("AppState"));
    REQUIRE(obj.attribs.size() == 22);
    REQUIRE(obj.childs.size() == 4);

    CHECK(obj.attribs.at(T_L("appid")) == T_L("343050"));

    CHECK(obj.attribs.at(T_L("buildid")) == T_L("1101428"));
    CHECK(obj.attribs.at(T_L("#1_attrib")) == T_L("1"));
    CHECK(obj.attribs.at(T_L("emptyAttrib")) == T_L(""));
    CHECK(obj.attribs.at(T_L("escape_quote")) == T_L(R"("quote")"));
    CHECK(obj.attribs.at(T_L("escape_quote_backslash")) == T_L("quote_with_other_escapes\\\"\\"));
    CHECK(obj.attribs.at(T_L("tab_escape")) == T_L("new\\ttab"));
    CHECK(obj.attribs.at(T_L("new_line_escape")) == T_L("new\\nline"));

    CHECK(obj.childs.at(T_L("UserConfig"))->name == T_L("UserConfig"));
    CHECK(obj.childs.at(T_L("UserConfig"))->childs.empty());

    const auto& inc = obj.childs.at(T_L("IncludedStuff"));
    CHECK(inc->name == T_L("IncludedStuff"));
    const auto& base = obj.childs.at(T_L("BaseInclude"));
    REQUIRE(base->attribs.size() == 1);
    CHECK(base->attribs.at(T_L("BaseAttrib")) == T_L("Yes"));
    CHECK(obj.attribs.at(T_L("another attribute with fancy space")) == T_L("yay"));
}


template<typename charT>
void check_DST_AST_multikey(const vdf::basic_multikey_object<charT>& obj)
{
    CHECK(obj.name == T_L("AppState"));
    REQUIRE(obj.attribs.size() == 23);
    REQUIRE(obj.childs.size() == 4);

    CHECK(obj.attribs.find(T_L("appid"))->second == T_L("343050"));

    CHECK(obj.attribs.find(T_L("buildid"))->second == T_L("1101428"));
    CHECK(obj.attribs.find(T_L("#1_attrib"))->second == T_L("1"));
    CHECK(obj.attribs.find(T_L("emptyAttrib"))->second == T_L(""));

    CHECK(obj.attribs.count(T_L("UpdateResult")) == 2);

    CHECK(obj.childs.find(T_L("UserConfig"))->second->name == T_L("UserConfig"));
    CHECK(obj.childs.find(T_L("UserConfig"))->second->childs.empty());

    const auto& inc = obj.childs.find(T_L("IncludedStuff"))->second;
    CHECK(inc->name == T_L("IncludedStuff"));
    const auto& base = obj.childs.find(T_L("BaseInclude"))->second;
    REQUIRE(base->attribs.size() == 1);
    CHECK(base->attribs.find(T_L("BaseAttrib"))->second == T_L("Yes"));
    CHECK(obj.attribs.find(T_L("another attribute with fancy space"))->second == T_L("yay"));
}

template<typename charT>
void read_check_DST_file_ok()
{
    std::basic_ifstream<charT> file("DST_Manifest.acf");
    bool ok;
    auto object = vdf::read(file, &ok);

    REQUIRE(ok);

    check_DST_AST(object);
}

template<typename charT>
void read_check_DST_file_ec()
{
    std::basic_ifstream<charT> file("DST_Manifest.acf");
    std::error_code ec;
    auto object = vdf::read(file, ec);

    REQUIRE(!ec);

    check_DST_AST(object);
}

template<typename charT>
void read_check_DST_file_throw()
{
    std::basic_ifstream<charT> file("DST_Manifest.acf");
    auto object = vdf::read(file);

    check_DST_AST(object);
}

template<typename charT>
void read_check_DST_file_multikey_throw()
{
    std::basic_ifstream<charT> file("DST_Manifest.acf");
    auto object = vdf::read<vdf::basic_multikey_object<charT>>(file);

    check_DST_AST_multikey(object);
}

TEST_CASE("Read File", "[read]")
{
    REQUIRE(cd(testdata_dir.c_str()) == 0);
    read_check_DST_file_ok<char>();
    read_check_DST_file_ok<wchar_t>();
    read_check_DST_file_ec<char>();
    read_check_DST_file_ec<wchar_t>();
    read_check_DST_file_throw<char>();
    read_check_DST_file_throw<wchar_t>();
}

template<typename charT>
void read_string()
{
    std::basic_string<charT> attribs( T_L("\"firstNode\"{\"SecondNode\"{\"Key\" \"Value\" //myComment\n}}") );
    bool ok;
    auto obj = vdf::read(attribs.begin(), attribs.end(), &ok);

    REQUIRE(ok);
}

template<typename charT>
void check_string(const vdf::basic_object<charT>& obj)
{
    CHECK(obj.name == T_L("firstNode"));

    CHECK(obj.attribs.empty() == true);
    REQUIRE(obj.childs.size() == 1);
    const auto& secondNode = obj.childs.at(T_L("SecondNode"));

    CHECK(secondNode->name == T_L("SecondNode"));
    REQUIRE(secondNode->attribs.size() == 1);
    CHECK(secondNode->childs.empty() == true);
    CHECK(secondNode->attribs.at(T_L("Key")) == T_L("Value"));
}

TEST_CASE("Read String","[read]")
{
    read_string<char>();
    read_string<wchar_t>();
}

template<typename charT>
void check_fail()
{
    bool ok;
    std::basic_string<charT> attribs( T_L("\"firstNode\"{\"SecondNode\"{\"Key\" //myComment\n}}") );
    auto obj = vdf::read(attribs.begin(), attribs.end(), &ok);

    REQUIRE(!ok);
}
//todo: error checking
TEST_CASE("Find Error","[read_error]")
{
    check_fail<char>();
    check_fail<wchar_t>();
}

template<typename charT>
void write_and_read()
{
    std::basic_string<charT> attribs( T_L("\"firstNode\"{\"SecondNode\"{\"Key\" \"Value\" //myComment\n}}") );
    bool ok;
    auto obj = vdf::read(attribs.begin(), attribs.end(), &ok);

    REQUIRE(ok);

    std::basic_stringstream<charT> output;
    vdf::write(output, obj);
    obj = vdf::read(output);

    check_string(obj);
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


/////////////////////////////////////////////////////////////
// readme test
/////////////////////////////////////////////////////////////

struct counter
{
    size_t num_attributes;
    counter():num_attributes(0){}
    void add_attribute(std::string key, std::string value)
    {
        ++num_attributes;
    }
    void add_child(std::unique_ptr< counter > child)
    {
        num_attributes += child->num_attributes;
    }
    void set_name(std::string n)
    {}
};

TEST_CASE("counter test", "[counter]")
{
    std::ifstream file("DST_Manifest.acf");
    counter num = tyti::vdf::read<counter>(file);
    CHECK(num.num_attributes == 25);
}
