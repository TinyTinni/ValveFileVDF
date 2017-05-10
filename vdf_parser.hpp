//MIT License
//
//Copyright(c) 2016 Matthias Moeller
//
//Permission is hereby granted, free of charge, to any person obtaining a copy
//of this software and associated documentation files(the "Software"), to deal
//in the Software without restriction, including without limitation the rights
//to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
//copies of the Software, and to permit persons to whom the Software is
//furnished to do so, subject to the following conditions :
//
//The above copyright notice and this permission notice shall be included in all
//copies or substantial portions of the Software.
//
//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
//AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
//SOFTWARE.

#ifndef __TYTI_STEAM_VDF_PARSER_H__
#define __TYTI_STEAM_VDF_PARSER_H__

#include <vector>
#include <unordered_map>
#include <utility>
#include <fstream>

#include <memory>
#include <stack>

namespace tyti
{
    namespace vdf
    {
        namespace detail
        {
            ///////////////////////////////////////////////////////////////////////////
            //  Helper functions selecting the right encoding (char/wchar_T)
            ///////////////////////////////////////////////////////////////////////////

            template<typename T>
            struct literal_macro_help
            {
                static const char* result(const char* c, const wchar_t* wc)
                {
                    return c;
                }
                static const char result(char c, wchar_t wc)
                {
                    return c;
                }
            };

            template<>
            struct literal_macro_help<wchar_t>
            {
                static const wchar_t* result(const char* c, const wchar_t* wc)
                {
                    return wc;
                }
                static const wchar_t result(char c, wchar_t wc)
                {
                    return wc;
                }
            };
#define TYTI_L(type, text) vdf::detail::literal_macro_help<type>::result(text, L##text)

            ///////////////////////////////////////////////////////////////////////////
            //  Writer helper functions
            ///////////////////////////////////////////////////////////////////////////

            struct tabs
            {
                size_t t;
                tabs(size_t i) :t(i) {}
            };

            template<typename oStreamT>
            oStreamT& operator<<(oStreamT& s, tabs t)
            {
                for (; t.t > 0; --t.t)
                    s << "\t";
                return s;
            }
        } // end namespace detail

        ///////////////////////////////////////////////////////////////////////////
        //  Interface
        ///////////////////////////////////////////////////////////////////////////

        /// basic object node. Every object has a name and can contains attributes saved as key_value pairs or childrens
        template<typename CharT>
        struct basic_object
        {
            typedef CharT char_type;
            std::basic_string<char_type> name;
            std::unordered_map<std::basic_string<char_type>, std::basic_string<char_type> > attribs;
            std::unordered_map<std::basic_string<char_type>, std::shared_ptr< basic_object<char_type> > > childs;
        };

        typedef basic_object<char> object;
        typedef basic_object<wchar_t> wobject;

        /** \brief writes given object tree in vdf format to given stream.
        Uses tabs instead of whitespaces.
        */
        template<typename oStreamT, typename charT = typename oStreamT::char_type>
        void write(oStreamT& s, const basic_object<charT>& r, size_t t = 0)
        {
            using namespace detail;
            s << tabs(t) << TYTI_L(charT, '"') << r.name << TYTI_L(charT, "\"\n") << tabs(t) << TYTI_L(charT, "{\n");
            for (auto& i : r.attribs)
                s << tabs(t + 1) << TYTI_L(charT, '"') << i.first << TYTI_L(charT, "\"\t\t\"") << i.second << TYTI_L(charT, "\"\n");
            for (auto& i : r.childs)
                write(s, i, t + 1);
            s << tabs(t) << TYTI_L(charT, "}\n");
        }


        class parser_error : public std::exception
        {
        };


        /** \brief Read VDF formatted sequences defined by the range [first, last).
        If the file is mailformatted, parser will try to read it until it can.
        @param first begin iterator
        @param end end iterator
        @param ok output bool. true, if parser successed, false, if parser failed
        */

        template<typename IterT, typename charT = typename IterT::value_type>
        basic_object<charT> read(IterT first, IterT last, bool* ok = 0)
        {
            //todo: error handling
            if (ok)
                *ok = true;

            basic_object<charT> root;
            basic_object<charT>* cur = &root;
            std::stack< basic_object<charT>* > lvls;
            //read header
            // first, quoted name
            auto b = std::find(first, last, TYTI_L(charT, '\"'));
            auto bend = std::find(b + 1, last, TYTI_L(charT, '\"'));
            root.name = std::basic_string<charT>(b + 1, bend);
            // second, get {}
            b = std::find(bend, last, TYTI_L(charT, '{'));
            if (b == last)
                if (ok)
                    *ok = false;
                else
                    throw parser_error();
            else
                lvls.push(&root);
            try {
                while (!lvls.empty() && b != last)
                {
                    const std::basic_string<charT> startsym = TYTI_L(charT, "\"}");

                    //find first starting attrib/child, or ending
                    b = std::find_first_of(b, last, std::cbegin(startsym), std::cend(startsym));
                    if (*b == '\"')
                    {
                        bend = std::find(b + 1, last, TYTI_L(charT, '\"'));
                        if (bend == last)
                            throw parser_error(); // could not find end of name

                        std::basic_string<charT> curName(b + 1, bend);
                        b = bend + 1;

                        const std::basic_string<charT> ecspsym = TYTI_L(charT, "\"{");
                        b = std::find_first_of(b, last, std::cbegin(ecspsym), std::cend(ecspsym));
                        if (b == last)
                            throw parser_error(); //could not find 2nd part of pair

                        if (*b == '\"')
                        {
                            bend = std::find(b + 1, last, TYTI_L(charT, '\"'));
                            if (bend == last)
                                throw parser_error(); //could not find end of name

                            auto value = std::basic_string<charT>(b + 1, bend);
                            b = bend + 1;

                            if (curName != TYTI_L(charT, "#include") && curName != TYTI_L(charT, "#base"))
                                cur->attribs[curName] = value;
                            else
                            {
                                std::basic_ifstream<charT> i(value);
                                auto n = std::make_shared<basic_object<charT>>(read(i, ok));
                                cur->childs[n->name] = n;
                            }
                        }
                        else if (*b == '{')
                        {
                            lvls.push(cur);
                            auto n = std::make_shared<basic_object<charT>>();
                            cur->childs[curName] = n;
                            cur = n.get();
                            cur->name = curName;
                            ++b;
                        }
                    }
                    else if (*b == '}')
                    {
                        cur = lvls.top();
                        lvls.pop();
                        ++b;
                    }
                }
            }
            catch (parser_error& p)
            {
                if (ok)
                    *ok = false;
                else
                    throw p;
            }
            return root;
        }

        /** \brief Loads a stream (e.g. filestream) into the memory and parses the vdf formatted data.
        */
        template<typename iStreamT, typename charT = iStreamT::char_type >
        basic_object<charT> read(iStreamT& inStream, bool *ok = 0)
        {
            // cache the file
            std::basic_string<charT> str;
            inStream.seekg(0, std::ios::end);
            str.resize(inStream.tellg());
            if (str.empty())
                return basic_object<charT>();

            inStream.seekg(0, std::ios::beg);
            inStream.read(&str[0], str.size());
            inStream.close();

            // parse it
            return read(str.begin(), str.end(), ok);
        }

    } // end namespace vdf
} // end namespace tyti
#ifndef TYTI_NO_L_UNDEF
#undef TYTI_L
#endif

#endif //__TYTI_STEAM_VDF_PARSER_H__