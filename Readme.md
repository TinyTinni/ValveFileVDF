#Valve Data Format (vdf) Reader and Writer in C++
Vavle has its own JSON-like data format: (KeyValue or also known as vdf.)[https://developer.valvesoftware.com/wiki/KeyValues]
It is widely used e.g. as a games manifest or SteamCMD output.
This Header-only file provides a parser and writer, loading and saving
data.

The parser is based on [Boost Spirit](www.boost.org).

#Features:
- read and write vdf data in C++
- buildt-in encodings: char and wide chars
- additional character sets can be configured
- supports C++ one line comments ('//')
- platform independent (tested only on windows yet)
- header-only
- No C++11 is required (tests requires C++11)
- Does not support the '#include' keyword

#Requirements
- [Boost Spirit](www.boost.org)

#How-To Use
First, you have to include the main file 'vdf-Parser.h'.
This file provides several functions and data-structures which are
in the namespace 'tyti::vdf'.

All functions and data structures suppoers wide characters.
Functions are templates and the wide character data structure has a 'w' prefix.

To read an file, create a stream e.g. 'std::ifsteam' or 'std::wifstream'
and call the 'tyti::vdf::read' function.
```
std::ifstream file("PathToMyFile");
tyti::vdf::object root = tyti::vdf::read(file);
```
You can also define a sequence of character defined by a range.
```
std::string blob;
...
tyti::vdf::object root = tyti::vdf::read(blob.cbegin(), blob.cend());
```

The 'tyti::vdf::object' is a tree like data structure.
It has its name, some attributes as a pair of 'key' and 'value'
and its object childs. Below you csn find an illustration on how the vdf data
is stored into an object:
```
"name"
{
    "attrib0" "Value"
    "child0"
    {
    ...
    }
    ...
}
```

Given such an object, you can also write it into vdf files via 'tyti::vdf::write':
```
tyti::vdf::write(file, object);
```

#Reference
```
  // Reader functions
  /// reads vdf data from the given stream
  template<typename iStreamT, typename charT = typename iStreamT::char_type>
  basic_object<charT> read(iStreamT& inStream, bool *ok = 0) 
  
  /// reads vdf data within the given range
  template<typename IterT, typename charT = typename IterT::value_type>
  basic_object<charT> read(IterT first, IterT last, bool* ok = 0)

  // Writer functions
  /// writes given obj into out in vdf style 
  template<typename oStreamT, typename charT = typename oStreamT::char_type>
  void write(oStreamT& out, const basic_object<charT>& obj)
  
  template<typename T>
  basic_object<T>;
  typedef basic_object<char> object;
  typedef basic_object<wchar_t> wobject
  
  template<typename T>
  basic_key_value<T>;
  typedef basic_key_value<char> key_value;
  typedef basic_key_value<whcar_t> wkey_value;
  
```
