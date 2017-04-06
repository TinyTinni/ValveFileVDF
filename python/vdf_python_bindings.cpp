#include "../vdf_parser.hpp"
#include <fstream>

#include <pybind11/pybind11.h>
#include <pybind11/stl_bind.h>

#include <boost/variant.hpp>

namespace py = pybind11;

struct py_vdf_visitor : public boost::static_visitor<>
{
	typename py::object result_type;
	template<typename T>
	py::object operator()(T& a) const
	{
		return py::cast(a);
	}
};

template<typename charT>
struct py_vdf_praser_ast_visitor : public boost::static_visitor<>
{
	typedef py::dict vis_object;
	typedef std::pair<std::basic_string<charT>, std::basic_string<charT> > vis_key_value;
	vis_object& m_currentObj;
public:
	py_vdf_praser_ast_visitor(vis_object& t) : m_currentObj(t) {}
	void operator()(std::pair<std::basic_string<charT>, std::basic_string<charT> > in) const
	{
		m_currentObj[py::cast(in.first)] = py::cast(in.second);
	}
	void operator()(std::basic_string<charT> in) const
	{
		auto obj = my_read(in.c_str());
		m_currentObj[py::cast(std::move(in))] = py::cast(std::make_shared<vis_object>(std::move(obj)));
	}
	void operator()(const tyti::vdf::detail::parser_ast<charT>& x) const
	{
		vis_object t;
		for (auto& i : x.children)
			boost::apply_visitor(py_vdf_praser_ast_visitor<charT>(t), i);
		m_currentObj[py::cast(x.name)] = t;
	}
};

py::dict py_read(const char* filename)
{
	using charT = char;
	using IterT = std::string::iterator;
	tyti::vdf::detail::parser_ast<charT> ast;
	{
		py::gil_scoped_release r;

		std::ifstream inStream(filename);
		std::basic_string<charT> str;
		inStream.seekg(0, std::ios::end);
		str.resize(inStream.tellg());
		//if (str.empty())
		//	return py::dict{};

		inStream.seekg(0, std::ios::beg);
		inStream.read(&str[0], str.size());
		inStream.close();

		using namespace tyti::vdf::detail;
		vdf_grammar<charT, IterT> parser;
		vdf_skipper<charT, IterT> skipper;
		boost::spirit::qi::phrase_parse(str.begin(), str.end(), parser, skipper, ast);

	}
	py::dict root;
	for (auto& i : ast.children)
		boost::apply_visitor(py_vdf_praser_ast_visitor<charT>(root), i);
	
	return root;
}

size_t tabsize = 0;
void py_write(const py::str& f, const py::dict& d)
{
	//std::ifstream(py::cast<std::string>(f));
	
	std::string tab = "";
	for (int i = 0; i < tabsize; ++i)
		tab += "\t";
	py::print(tab+"{");

	for (auto& it : d)
	{
		std::string key = py::cast<std::string>(it.first);
		if (py::isinstance<py::str>(it.second))
		{
			std::string value = py::cast<std::string>(it.second);
			py::print(tab+"\t"+ key + " " + value);
		}			
		else if (py::isinstance<py::dict>(it.second))
		{
			++tabsize;
			py::print(tab+"\t"+key);
			py_write(f, py::cast<py::dict>(it.second));
			--tabsize;
		}
	}
	py::print(tab+"}");

}

PYBIND11_PLUGIN(vdf)
{
	py::module m("vdf", "Reads and Write Valve vdf files.");

	m.def("read", &py_read, "Read vdf file");
	//m.def("write", &py_write);


	return m.ptr();
}