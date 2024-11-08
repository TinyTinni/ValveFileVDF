#include "vdf_parser.hpp"
#include <fstream>
#include <string>

#include <pybind11/pybind11.h>
#include <pybind11/stl_bind.h>

namespace py = pybind11;

struct python_object
{
    py::dict dict;
    std::string name;

    void add_attribute(std::string key, std::string value)
    {
        dict[py::cast(std::move(key))] = py::cast(std::move(value));
    }
    void add_child(std::unique_ptr<python_object> child)
    {
        std::string n = std::move(child->name);
        dict[py::cast(n)] = std::move(child->dict);
    }
    void set_name(std::string n) { name = std::move(n); }
};

py::dict py_read_file(const char *filename)
{
    std::ifstream input(filename);
    auto obj = tyti::vdf::read<python_object>(input);
    if (obj.name.empty())
        return obj.dict;
    auto result = py::dict();
    result[py::cast(obj.name)] = std::move(obj.dict);
    return result;
}

py::dict py_read(const std::string &filename)
{
    auto obj = tyti::vdf::read<python_object>(std::begin(filename),
                                              std::end(filename));
    if (obj.name.empty())
        return obj.dict;
    auto result = py::dict();
    result[py::cast(obj.name)] = std::move(obj.dict);
    return result;
}

PYBIND11_MODULE(vdf, m)
{
    m.doc() = "Read and Write Valve's vdf files.";

    m.def("read", &py_read, "Read vdf from memory");
    m.def("read_file", &py_read_file, "Read vdf file");
}