#pragma once

#include <rapidcheck.h>
#include <string>

inline bool contains_surrogate(const std::wstring &str)
{
    return str.find(wchar_t(-1)) != str.npos;
}

////////////////////////////////////////////////////////////////
template <typename charT> rc::Gen<std::basic_string<charT>> gen_name_string();

template <> inline rc::Gen<std::basic_string<char>> gen_name_string<char>()
{
    return rc::gen::string<std::string>();
}

template <>
inline rc::Gen<std::basic_string<wchar_t>> gen_name_string<wchar_t>()
{
    return rc::gen::suchThat(rc::gen::string<std::wstring>(),
                             [](const auto &str)
                             { return !contains_surrogate(str); });
}

////////////////////////////////////////////////////////////////
template <typename charT>
rc::Gen<std::basic_string<charT>> gen_unescaped_name_string();

template <> inline rc::Gen<std::string> gen_unescaped_name_string<char>()
{
    return rc::gen::suchThat(gen_name_string<char>(), [](const std::string &str)
                             { return str.find("\"") == str.npos; });
}

template <> inline rc::Gen<std::wstring> gen_unescaped_name_string<wchar_t>()
{
    return rc::gen::suchThat(gen_name_string<wchar_t>(),
                             [](const std::wstring &str)
                             { return str.find(L"\"") == str.npos; });
}