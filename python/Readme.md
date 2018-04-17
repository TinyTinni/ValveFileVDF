# Python Interface

Adds a simple interface for Python.
Use CMake (>=3.12) to build the .pyd file.

Interface may change in the future.

Module Example:
```python
import vdf

mydict = vdf.read_file("test_file.vdf")
#mydict is a standard dictionary

value = mydict[key]

mydict2 = vdf.read("vdf_file{"key":"value"}")
```