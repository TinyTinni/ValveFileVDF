#include <algorithm>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>

#define TYTI_NO_L_UNDEF
#include <vdf_parser.hpp>
#define T_L(x) TYTI_L(charT, x)
using namespace tyti;

#include "doctest.h"

template <typename charT>
void check_DST_AST(const vdf::basic_object<charT> &obj)
{
    CHECK(obj.name == T_L("AppState"));
    REQUIRE(obj.attribs.size() == 25);
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
    CHECK(obj.attribs.at(T_L("quad_escape")) == T_L("\\\\"));
#endif

    CHECK(obj.childs.at(T_L("UserConfig"))->name == T_L("UserConfig"));
    CHECK(obj.childs.at(T_L("UserConfig"))->childs.empty());

    CHECK(obj.childs.at(T_L("MountedDepots"))->attribs.size() == 1);

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
    REQUIRE(obj.attribs.size() == 26);
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

TEST_CASE_TEMPLATE("Read File", charT, char, wchar_t)
{
    SUBCASE("bool return")
    {
        std::basic_ifstream<charT> file("DST_Manifest.acf");
        bool ok;
        auto objects = vdf::read(file, &ok);

        REQUIRE(ok);
        auto it = objects.childs.find(T_L("AppState"));
        CHECK(it != objects.childs.end());
        check_DST_AST(*(it->second));
    }

    SUBCASE("ec return")
    {
        std::basic_ifstream<charT> file("DST_Manifest.acf");
        std::error_code ec;
        auto objects = vdf::read(file, ec);

        REQUIRE(!ec);
        auto it = objects.childs.find(T_L("AppState"));
        CHECK(it != objects.childs.end());
        check_DST_AST(*(it->second));
    }

    SUBCASE("exception")
    {
        std::basic_ifstream<charT> file("DST_Manifest.acf");
        auto objects = vdf::read(file);
        auto it = objects.childs.find(T_L("AppState"));
        CHECK(it != objects.childs.end());
        check_DST_AST(*(it->second));
    }
}

TEST_CASE_TEMPLATE("Read String", charT, char, wchar_t)
{
    std::basic_string<charT> attribs(
        T_L("\"firstNode\"{\"SecondNode\"{\"Key\" \"Value\" //myComment\n}}"));
    bool ok;
    vdf::read(attribs.begin(), attribs.end(), &ok);

    REQUIRE(ok);
}

TEST_CASE_TEMPLATE("Read String with conditional, assuming PC platform", charT,
                   char, wchar_t)
{
    std::basic_stringstream<charT> input;
    input << T_L("\"firstNode\"{");
    input << T_L("\"Key\" \"InvalidValue\"[!$WIN32]\n");
    input << T_L("\"Key\" \"Value\"[$WIN32]\n");
    input << T_L("}");
    auto debug = input.str();

    auto obj = vdf::read(input);

    REQUIRE(obj.attribs.size() == 1);
    REQUIRE(obj.attribs.find(T_L("Key"))->second == T_L("Value"));
}

// todo: error checking
TEST_CASE_TEMPLATE("Find Error", charT, char, wchar_t)
{
    bool ok;
    std::basic_string<charT> attribs(
        T_L("\"firstNode\"{\"SecondNode\"{\"Key\" //myComment\n}}"));
    vdf::read(attribs.begin(), attribs.end(), &ok);

    REQUIRE(!ok);
}

TEST_CASE_TEMPLATE("Write and Read", charT, char, wchar_t)
{
    std::basic_string<charT> attribs(
        T_L("\"firstNode\"{\"SecondNode\"{\"Key\" \"Value\" //myComment\n}}"));
    bool ok;
    auto obj = vdf::read(attribs.begin(), attribs.end(), &ok);

    REQUIRE(ok);

    std::basic_stringstream<charT> output;
    vdf::write(output, obj);
    obj = vdf::read(output);

    CHECK(obj.name == T_L("firstNode"));

    CHECK(obj.attribs.empty() == true);
    REQUIRE(obj.childs.size() == 1);
    const auto &secondNode = obj.childs.at(T_L("SecondNode"));

    CHECK(secondNode->name == T_L("SecondNode"));
    REQUIRE(secondNode->attribs.size() == 1);
    CHECK(secondNode->childs.empty() == true);
    CHECK(secondNode->attribs.at(T_L("Key")) == T_L("Value"));
}

TEST_CASE_TEMPLATE("read multikey", charT, char, wchar_t)
{
    std::basic_ifstream<charT> file("DST_Manifest.acf");
    auto objects = vdf::read<vdf::basic_multikey_object<charT>>(file);
    auto it = objects.childs.find(T_L("AppState"));
    CHECK(it != objects.childs.end());
    check_DST_AST_multikey(*(it->second));
}

TEST_CASE_TEMPLATE("read broken file", charT, char, wchar_t)
{
#ifndef WIN32
    if constexpr (std::is_same_v<charT, wchar_t>)
        return;
#endif
    std::basic_ifstream<charT> file("broken_file.acf");
    std::error_code ec;
    auto objects = vdf::read(file, ec);
    REQUIRE(ec);
    REQUIRE(objects.name.empty());
    REQUIRE(objects.attribs.empty());
    REQUIRE(objects.childs.empty());
}

TEST_CASE_TEMPLATE("read broken file throw", charT, char, wchar_t)
{
#ifndef WIN32
    if constexpr (std::is_same_v<charT, wchar_t>)
        return;
#endif
    std::basic_ifstream<charT> file("broken_file.acf");
    CHECK_THROWS(vdf::read(file));
}

TEST_CASE("issue14")
{
    std::ifstream input_file("issue14.vdf", std::ios::in);
    CHECK_THROWS(tyti::vdf::read(input_file));
}

/////////////////////////////////////////////////////////////
// write test
/////////////////////////////////////////////////////////////

TEST_CASE_TEMPLATE("Write escaped", charT, char, wchar_t)
{
    std::vector<std::basic_string<charT>> data = {
        TYTI_L(charT, "\""),     TYTI_L(charT, "\\"),
        TYTI_L(charT, "\\\\"),   TYTI_L(charT, "\"\""),
        TYTI_L(charT, "\\\\\\"), TYTI_L(charT, "\"\\\""),
        TYTI_L(charT, "\\\""),   TYTI_L(charT, "\\\\\"\\\\")};
    for (const auto &datapoint : data)
    {
        CAPTURE(datapoint);

        vdf::basic_object<charT> obj;
        obj.name = datapoint;

        std::basic_stringstream<charT> output;
        vdf::write(output, obj);
        auto test_obj = vdf::read(output);

        CAPTURE(output.str());
        CHECK(test_obj.name == obj.name);
    }
}

TEST_CASE_TEMPLATE("write not-escaped", charT, char, wchar_t)
{

    vdf::WriteOptions writeOpts;
    writeOpts.escape_symbols = false;

    vdf::Options readOpts;
    readOpts.strip_escape_symbols = false;
    std::vector<std::basic_string<charT>> data = {
        TYTI_L(charT, "\\"),
        TYTI_L(charT, "\\\\"),
        TYTI_L(charT, "\\\\\\"),

    };
    for (const auto &datapoint : data)
    {
        CAPTURE(datapoint);

        vdf::basic_object<charT> obj;
        obj.name = datapoint;

        std::basic_stringstream<charT> output;
        vdf::write(output, obj, writeOpts);
        auto test_obj = vdf::read(output, readOpts);

        CAPTURE(output.str());
        CHECK(test_obj.name == obj.name);
    }
}

/////////////////////////////////////////////////////////////
// readme test
/////////////////////////////////////////////////////////////

TEST_CASE("counter test")
{
    struct counter
    {
        size_t num_attributes;
        counter() : num_attributes(0) {}
        void add_attribute(std::string, std::string) { ++num_attributes; }
        void add_child(std::unique_ptr<counter> child)
        {
            num_attributes += child->num_attributes;
        }
        void set_name(std::string) {}
    };

    std::ifstream file("DST_Manifest.acf");
    counter num = tyti::vdf::read<counter>(file);
    CHECK(num.num_attributes == 30);
}

/////////////////////////////////////////////////////////////
// fuzzer findings
/////////////////////////////////////////////////////////////
TEST_CASE("fuzzing_files")
{

    for (auto const &dir_entry :
         std::filesystem::directory_iterator{"fuzzing_data"})
    {
        SUBCASE(dir_entry.path().filename().string().c_str())
        {
            std::ifstream f(dir_entry.path().string());
            CHECK_THROWS(tyti::vdf::read(f));
        }
    }
}