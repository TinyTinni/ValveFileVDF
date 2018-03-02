# Valve Data Format (.vdf) Reader and Writer in C++
[![Build Status](https://travis-ci.org/TinyTinni/ValveFileVDF.svg?branch=master)](https://travis-ci.org/TinyTinni/ValveFileVDF)
[![Build status](https://ci.appveyor.com/api/projects/status/380441mkwkfvh4tj?svg=true)](https://ci.appveyor.com/project/TinyTinni/valvefilevdf)

Vavle uses its own JSON-like data format: [KeyValue, also known as vdf.](https://developer.valvesoftware.com/wiki/KeyValues)
e.g. in game manifest files or as SteamCMD output.
This header-only file provides a parser and writer to load and save the given data.

## Features:
- read and write vdf data in C++
- build-in encodings: `char`  and `wchar_t`
- supports custom character sets
- support for C++ (//) and C (/**/) comments
- `#include`/`#base` keyword (note: searches for files in the current working directoy)
- platform independent
- header-only

## Requirements
- C++11
 
## How-To Use
First, you have to include the main file `vdf-Parser.h`.
This file provides several functions and data-structures which are
in the namespace `tyti::vdf`.

All functions and data structures suppoers wide characters.
The wide character data structure is indicated by the commonly known `w`-prefix.
Functions are templates and don't need a prefix.

To read an file, create a stream e.g. `std::ifsteam` or `std::wifstream`
and call the `tyti::vdf::read` function.
```c++
std::ifstream file("PathToMyFile");
tyti::vdf::object root = tyti::vdf::read(file);
```
You can also define a sequence of character defined by a range.
```c++
std::string blob;
...
tyti::vdf::object root = tyti::vdf::read(std::cbegin(blob), std::cend(blob));

//given .vdf below, following holds
assert(root.name == "name");
const std::shared_ptr<tyti::vdf::object> child = root.childs["child0"];
assert(child->name == "child0");
const std::string& k = root.attribs["attrib0"];
assert(k == "value");
```

The `tyti::vdf::object` is a tree like data structure.
It has its name, some attributes as a pair of `key` and `value`
and its object childs. Below you can see a vdf data structure and how it is stored by naming:
```javascript
"name"
{
    "attrib0" "value" // saved as a pair, first -> key, second -> value
    "#base" "includeFile.vdf" // appends object defined in the file to childs
    "child0"
    {
    ...
    }
    ...
}
```

Given such an object, you can also write it into vdf files via:
```c++
tyti::vdf::write(file, object);
```

## Multi-Key and Custom Output Format

It is also possible to customize your output dataformat.
Per default, the parser stores all items in a std::unordered_map, which, per definition,
doesn't allow different entries with the same key.

However, the Valve vdf format supports multiple keys. Therefore, the output data format
has to store all items in e.g. a std::unordered_multimap.

You can change the output format by passing the output type via template argument to
the read function
```c++
namespace tyti;
vdf::basic_object no_multi_key  = vdf::read(std::cbegin(blob), std::cend(blob));
vdf::multikey_object multi_key = vdf::read<vdf::multikey_object>(std::cbegin(blob), std::cend(blob));
```

__Note__: The interface of [std::unordered_map](http://en.cppreference.com/w/cpp/container/unordered_map) and [std::unordered_multimap](http://en.cppreference.com/w/cpp/container/unordered_multimap)
are different when you access the elements.

It is also possible to create your own data structure which is used by the parser.
Your output class needs to define 3 functions with the following signature:

```c++
void add_attribute(std::basic_string<CHAR> key, std::basic_string<CHAR> value)
void add_child(std::unique_ptr< MYCLASS > child)
void set_name(std::basic_string<CHAR> n)
```
where ```MYCLASS``` is the tpe of your class and ```CHAR``` the type of your character set.

This also allows you, to inspect the file without storing it in a datastructure.
Lets say, for example, you want to count all attributes of a file without storing it.
You can do this by using this class

```c++
struct counter
{
    size_t num_attributes = 0;
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
```

and then call the read function
```c++
counter num = tyti::vdf::read<counter>(file);
```

## Reference
```c++
  // classes

  // default output object
  template<typename T>
  basic_object<T>
  {
    std::basic_string<char_type> name;
    std::unordered_map<std::basic_string<char_type>, std::basic_string<char_type> > attribs;
    std::unordered_map<std::basic_string<char_type>, std::shared_ptr< basic_object<char_type> > > childs;
  };
  typedef basic_object<char> object;
  typedef basic_object<wchar_t> wobject

  // output object with multikey support
  template<typename T>
  basic_multikey_object<T>
  {
    std::basic_string<char_type> name;
    std::unordered_multimap<std::basic_string<char_type>, std::basic_string<char_type> > attribs;
    std::unordered_multimap<std::basic_string<char_type>, std::shared_ptr< basic_object<char_type> > > childs;
  };

/*
  Possible error codes:
    std::errc::protocol_error: file is mailformatted
    std::errc::not_enough_memory: not enough space
    std::errc::invalid_argument: iterators throws e.g. out of range
*/

/** \brief Read VDF formatted sequences defined by the range [first, last).
  If the file is mailformatted, parser will try to read it until it can.
  @param first begin iterator
  @param end end iterator
  @param ec output bool. 0 if ok, otherwise, holds an system error code
  */
  template<typename IterT>
  basic_object<typename IterT::value_type> read(IterT first, IterT last, std::error_code& ec) noexcept;
  
  /** \brief Read VDF formatted sequences defined by the range [first, last).
  If the file is mailformatted, parser will try to read it until it can.
  @param first begin iterator
  @param end end iterator
  @param ok output bool. true, if parser successed, false, if parser failed
  */
  template<typename IterT>
  basic_object<typename IterT::value_type> read(IterT first, IterT last, bool* ok) noexcept;
  
  /** \brief Read VDF formatted sequences defined by the range [first, last).
  If the file is mailformatted, parser will try to read it until it can.
  @param first begin iterator
  @param end end iterator
  
  throws a "std::system_error" if a parsing error occured
  */
  template<typename IterT>
  basic_object<typename IterT::value_type> read(IterT first, IterT last);
  
  /** \brief Loads a stream (e.g. filestream) into the memory and parses the vdf formatted data.
      throws "std::bad_alloc" if file buffer could not be allocated
  */
  template<typename iStreamT>
  basic_object<iStreamT::char_type> read(iStreamT& inStream, std::error_code& ec);
  
  /** \brief Loads a stream (e.g. filestream) into the memory and parses the vdf formatted data.
      throws "std::bad_alloc" if file buffer could not be allocated
      ok == false, if a parsing error occured
  */
  template<typename iStreamT>
  basic_object<typename iStreamT::char_type> read(iStreamT& inStream, bool* ok);
  
  /** \brief Loads a stream (e.g. filestream) into the memory and parses the vdf formatted data.
      throws "std::bad_alloc" if file buffer could not be allocated
      throws "std::system_error" if a parsing error occured
  */
  template<typename iStreamT>
  basic_object<typename iStreamT::char_type> read(iStreamT& inStream);


/////////////////////////////////////////////////////////////////////////////
  // Writer functions
  /// writes given obj into out in vdf style 
  /// Output is prettyfied, using tabs
  template<typename oStreamT>
  void write(oStreamT& out, const basic_object<typename oStreamT::char_type>& obj);
  
```

## Remarks for Errors
The current version is a greedy implementation and jumps over unrecognized fields.
Therefore, the error detection is very imprecise an does not give the line, where the error occurs.

## License

[MIT License](./LICENSE) © Matthias Möller. Made with ♥ in Germany.
