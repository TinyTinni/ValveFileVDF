#pragma once

#include <rapidcheck.h>
#include <string>

inline bool containsSurrogate(const std::wstring &str)
{
    return str.find(wchar_t(-1)) != str.npos;
}

////////////////////////////////////////////////////////////////
template <typename charT>
rc::Gen<std::basic_string<charT>> genValidNameString();

template <> inline rc::Gen<std::basic_string<char>> genValidNameString<char>()
{
    return rc::gen::string<std::string>();
}

template <>
inline rc::Gen<std::basic_string<wchar_t>> genValidNameString<wchar_t>()
{
    return rc::gen::suchThat(rc::gen::string<std::wstring>(),
                             [](const auto &str)
                             { return !containsSurrogate(str); });
}

////////////////////////////////////////////////////////////////
template <typename charT>
rc::Gen<std::basic_string<charT>> genValidUnescapedNameString();

template <> inline rc::Gen<std::string> genValidUnescapedNameString<char>()
{
    return rc::gen::suchThat(rc::gen::string<std::string>(),
                             [](const std::string &str)
                             { return str.find("\"") == str.npos; });
}

template <> inline rc::Gen<std::wstring> genValidUnescapedNameString<wchar_t>()
{
    return rc::gen::suchThat(
        rc::gen::string<std::wstring>(), [](const std::wstring &str)
        { return str.find(L"\"") == str.npos && !containsSurrogate(str); });
}