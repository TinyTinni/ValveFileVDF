#include <iostream>
#include <string>
#include <algorithm>

#define TYTI_NO_L_UNDEF
#include <vdf_parser.hpp>
#define T_L(x) TYTI_L(charT,x)

#define BOOST_TEST_MODULE example
#include <boost/test/included/unit_test.hpp>

template<typename charT>
void read_check_DST_file()
{
    auto path = std::basic_string<charT>(T_L(SOURCE_DIR))+T_L("/testdata/DST_Manifest.acf");
    std::basic_ifstream<charT> file(path);
    auto object = vdf::read(file);
    
    check_DST_AST(object);
}

template<typename charT>
void check_DST_AST(const vdf::basic_object<charT>& obj)
{
    BOOST_CHECK(obj.name == T_L("AppState"));
    BOOST_REQUIRE(obj.attribs.size() == 14);
    BOOST_REQUIRE(obj.childs.size() == 2);

    BOOST_CHECK(obj.attribs[0].first == T_L("appid"));
    BOOST_CHECK(obj.attribs[0].second == T_L("343050"));

    BOOST_CHECK(obj.attribs[8].first == T_L("buildid"));
    BOOST_CHECK(obj.attribs[8].second == T_L("1101428"));

    BOOST_CHECK(obj.childs[0].name == T_L("UserConfig"));
    BOOST_CHECK(obj.childs[0].childs.empty());
}

BOOST_AUTO_TEST_CASE(Read_File)
{
    
    read_check_DST_file<char>();
    read_check_DST_file<wchar_t>();
}

template<typename charT>
void read_string()
{
    std::basic_string<charT> attribs{ T_L("\"firstNode\"{\"SecondNode\"{\"Key\" \"Value\" //myComment\n}}") };
    auto obj = vdf::read(attribs.begin(), attribs.end());

    BOOST_CHECK(obj.name == T_L("firstNode"));

    BOOST_CHECK(obj.attribs.empty());
    BOOST_REQUIRE(obj.childs.size() == 1);
    auto secondNode = obj.childs[0];

    BOOST_CHECK(secondNode.name == T_L("SecondNode"));
    BOOST_REQUIRE(secondNode.attribs.size() == 1);
    BOOST_CHECK(secondNode.childs.empty());
    BOOST_CHECK(secondNode.attribs[0].first == T_L("Key"));
    BOOST_CHECK(secondNode.attribs[0].second == T_L("Value"));
}

BOOST_AUTO_TEST_CASE(Read_String)
{
    read_string<char>();
    read_string<wchar_t>();
}

template<typename charT>
void check_fail()
{
    bool ok;
    std::basic_string<charT> attribs{ T_L("\"firstNode\"\"SecondNode\"{\"Key\" \"Value\" //myComment\n}}") };
    auto obj = vdf::read(attribs.begin(), attribs.end(), &ok);

    BOOST_CHECK(!ok);
}

BOOST_AUTO_TEST_CASE(Error_Test)
{
    check_fail<char>();
    check_fail<wchar_t>();
}
