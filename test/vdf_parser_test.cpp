#include <iostream>
#include <string>
#include <algorithm>

#define TYTI_NO_L_UNDEF
#include <vdf_parser.hpp>
#define T_L(x) TYTI_L(charT,x)
using namespace tyti;

#define BOOST_TEST_MODULE example
#include <boost/test/included/unit_test.hpp>

#include <experimental/filesystem>

std::ostream& operator<<(std::ostream& o, const std::basic_string<wchar_t>& w)
{
	for (const auto& i : w)
		o << reinterpret_cast<const char*>(i); //hard cast to simple char, maybe not best idea
	return o;
}

namespace boost {
	namespace test_tools {
		namespace tt_detail{
		template<>
		struct print_log_value< std::basic_string<wchar_t> > {
			void operator()(std::ostream& os, const std::basic_string<wchar_t>& ts) const
			{
				::operator<<(os, ts);
			}
		};
		}
	}
}

template<typename charT>
void read_check_DST_file()
{
    auto path = std::basic_string<charT>(T_L("DST_Manifest.acf"));
    std::basic_ifstream<charT> file(path);
	bool ok;
    auto object = vdf::read(file, &ok);
    
	BOOST_REQUIRE(ok);

    check_DST_AST(object);
}

template<typename charT>
void check_DST_AST(const vdf::basic_object<charT>& obj)
{
    BOOST_CHECK_EQUAL(obj.name, T_L("AppState"));
    BOOST_REQUIRE_EQUAL(obj.attribs.size() , 16);
    BOOST_REQUIRE_EQUAL(obj.childs.size() , 4);

    BOOST_CHECK_EQUAL(obj.attribs.at(T_L("appid")), T_L("343050"));

    BOOST_CHECK_EQUAL(obj.attribs.at(T_L("buildid")), T_L("1101428"));
	BOOST_CHECK_EQUAL(obj.attribs.at(T_L("#1_attrib")), T_L("1"));
	BOOST_CHECK_EQUAL(obj.attribs.at(T_L("emptyAttrib")), T_L(""));

    BOOST_CHECK_EQUAL(obj.childs.at(T_L("UserConfig"))->name, T_L("UserConfig"));
    BOOST_CHECK(obj.childs.at(T_L("UserConfig"))->childs.empty());

    const auto inc = obj.childs.at(T_L("IncludedStuff"));
    BOOST_CHECK_EQUAL(inc->name, T_L("IncludedStuff"));
    const auto base = obj.childs.at(T_L("BaseInclude"));
    BOOST_REQUIRE_EQUAL(base->attribs.size(), 1);
    BOOST_CHECK_EQUAL(base->attribs.at(T_L("BaseAttrib")), T_L("Yes"));
}

BOOST_AUTO_TEST_CASE(Read_File)
{
    std::experimental::filesystem::current_path(std::string(SOURCE_DIR)+"/testdata/");
    read_check_DST_file<char>();
    read_check_DST_file<wchar_t>();
}

template<typename charT>
void read_string()
{
    std::basic_string<charT> attribs{ T_L("\"firstNode\"{\"SecondNode\"{\"Key\" \"Value\" //myComment\n}}") };
	bool ok;
    auto obj = vdf::read(attribs.begin(), attribs.end(), &ok);

	BOOST_REQUIRE(ok);

    BOOST_CHECK_EQUAL(obj.name, T_L("firstNode"));

    BOOST_CHECK(obj.attribs.empty());
    BOOST_REQUIRE_EQUAL(obj.childs.size(), 1);
    auto secondNode = obj.childs.at(T_L("SecondNode"));

    BOOST_CHECK_EQUAL(secondNode->name, T_L("SecondNode"));
    BOOST_REQUIRE_EQUAL(secondNode->attribs.size(), 1);
    BOOST_CHECK(secondNode->childs.empty());
    BOOST_CHECK_EQUAL(secondNode->attribs.at(T_L("Key")), T_L("Value"));
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

    BOOST_REQUIRE(!ok);
}

BOOST_AUTO_TEST_CASE(Error_Test)
{
    check_fail<char>();
    check_fail<wchar_t>();
}
