# Python Interface

Adds a simple interface for Python.
Use CMake to build the .pyd file.  

To enable the python build, set the CMake variable `VALVEFILEVDF_ENABLE_PYTHON=ON`.

**Interface may change in the future.**

Currently, it only supports basic reading with the basic non-multidict dictionary and default parsing options. No write support yet.

`vdf.read_file`/`vdf.read` will return a simple python dict.

Module Example:
```python
import vdf

mydict = vdf.read_file("test_file.vdf")
#mydict is a standard dictionary

value = mydict[key]

mydict2 = vdf.read('"vdf_file"{"key" "value"}')
self.assertEqual(d["vdf_file"]["key"], "value")
```
