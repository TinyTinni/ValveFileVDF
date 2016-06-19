#include <boost/python.hpp>

#include "../vdf_parser.hpp"
#include <fstream>

namespace py = boost::python;

py::dict extract(tyti::vdf::object obj)
{
    py::dict result;
    for (auto&i : obj.attribs)
        result[i.first.c_str()] = i.second.c_str();
    for (auto&i : obj.childs)
        result[i.first.c_str()] = extract(i.second);
    return result;
}

py::dict read(py::str filename)
{
    py::dict result;
    std::ifstream f{ py::extract<std::string>(filename) };
    auto obj = tyti::vdf::read(f);
    return extract(obj);
}

void write(py::str filename, py::dict input)
{

}

BOOST_PYTHON_MODULE(vdf)
{
    py::dict(*fn)(py::str) = &read;
    py::def("read", fn);
}