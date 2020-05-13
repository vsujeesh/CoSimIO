# Tutorial for integrating the _CoSimIO_ using the Python interface

This tutorial helps you through to integrate the _CoSimIO_ into a solver using the Python interface.

## What you need
- Downloading the _CosimIO_ from the repository:

```bash
git clone https://github.com/KratosMultiphysics/CoSimIO.git
```

- A C++11 compatible compiler. This includes most of the major CPP compilers in Windows, Linux, and Mac. You may find a detailed list of C++11 compatible compilers [here](https://en.cppreference.com/w/cpp/compiler_support#cpp11)


## Tutorial 1: Building
The Python interface is located in [co_sim_io/python](https://github.com/KratosMultiphysics/CoSimIO/tree/master/co_sim_io/python) folder of the repository. It depends to the [pybind11 library](https://github.com/pybind/pybind11) which is provided in the [expternal_libraries/pybind11](https://github.com/KratosMultiphysics/CoSimIO/tree/master/external_libraries/pybind11) folder of this repo.

Using cmake the compilation is straightforward. Just calling the following command from the CoSimIO root:

```
sh scripts/build_python.sh
```

This will compile the binaries in the build folder and installs them into the bin subfolder of CoSimIO.

Please don't forget to add the binaries in the bin folder to your `PYTHONPATH`.

## Tutorial 2: Hello CosimIO
After integrating the _CoSimIO_ in your code now it's time to say hello

```python
import CoSimIO
info = CoSimIO.Hello()
```
Please note that this method like other methods in _CoSimIO_ returns an `Info` object. This object is a versatile container holding important information about the operation that has been done. In this case, it contains the version of the _CoSimIO_ library which can be queried:

```python
print(info)
```

Info class also provides get method for retriving a certain value knowing its type:

```python
major_version = info.GetInt("major_version")
minor_version = info.GetInt("minor_version")
patch_version = info.GetString("patch_version")
```
You may find this example in hello.py file in the `solver_integration/python_solver` folder

## Tutorial 3: Connecting and Disconnecting
The first step to establishing a connection to Kratos CoSimulation is to use the `Connect()` method:
```python
# The connect should be called before any CosimIO method called
info = CoSimIO.Connect(settings)
```

First of all, you may notice that `Connect()` method takes an `Info` as its arguments. This contianer can be used to pass additional information about the solver or connection settings to the CoSimIO:

```python
settings = CoSimIO.Info()
settings.SetString("connection_name", "test_connection") # This must be unique for each connection between two solvers
settings.SetString("solver_name", "my_solver") # Not to be confused with the connection name.
settings.SetInt("echo_level", 1)
settings.SetString("solver_version", "1.25")
```
This method returns an `Info` object containing information about the connection which can be queried using Get method:

```python
info.GetInt("connection_status")
```

Now putting together everything:

```python
import CoSimIO

settings = CoSimIO.Info()
settings.SetString("connection_name", "test_connection") # This should be unique for each connection between two solvers
settings.SetString("solver_name", "my_solver") # Not to be confused with the connection name.
settings.SetInt("echo_level", 1)
settings.SetString("solver_version", "1.25")

info = CoSimIO.Connect(settings)
if info.GetInt("connection_status") == CoSimIO.ConnectionStatus.Connected:
    print("Connected!")

info = CoSimIO.Disconnect(settings)
if info.GetInt("connection_status") == CoSimIO.ConnectionStatus.Disconnected:
    print("Disconnected!")
```

You may find this example in connect_disconect.cpp file in the `solver_integration/cpp` folder

## Tutorial 4: Data Exchange
One of the important missions of the _CoSimIO_ is to send and recieve data between processes. The `ExportData()` method can be used to send data to the Kratos or directly to another solver:

```python
info = CoSimIO.Info()
info.SetString("identifier", "vector_of_pi")
info.SetString("connection_name", "test_connection")
return_info = CoSimIO.ExportData(info, data_to_be_send)
```
The `ImportData()` should be used on the other side to recieve data:

```python
info = CoSimIO.Info()
info.SetString("identifier", "vector_of_pi")
info.SetString("connection_name", "test_connection")
return_info, received_data = CoSimIO.ImportData(info)
```
It is important to mention that the `ImportData()` will clear and resize the vector if needed.



## Tutorial 5: Mesh Exchange
After seeing how we transfer raw data between solvers, it is time to see how we can export and import meshes. For exporting the mesh one may use the `ExportMesh()` method:


```Python
info = CoSimIO.Info()
info.SetString("identifier", "fluid_mesh")
info.SetString("connection_name", "test_connection")
return_info = CoSimIO.ExportMesh(info, nodal_coords, element_connectivities, element_types)
```

The arguments are:

* `nodal_coordinates`: A vector of doubles of 3D coordinates of each node in x1,y1,z1,x2,y2,z2,... format:
```python
nodal_coords = [
    0.0, 2.5, 1.0, # 0
    2.0, 0.0, 1.5, # 1
    2.0, 2.5, 1.5, # 2
    4.0, 2.5, 1.7, # 3
    4.0, 0.0, 1.7, # 4
    6.0, 0.0, 1.8  # 5
]
```
* `elements_connectivities`: A vector of int containing the zero based index of each node in e1_1,e1_2,...,e2_1, e2_2,... format:
```python
element_connectivities = [
    0, 1, 2, # 1
    1, 3, 2, # 2
    1, 4, 3, # 3
    3, 4, 5, # 4
]
```

* `elements_types`: A vector of int containing the type of the elements. They are according to the vtk cell types, see [this link](https://vtk.org/wp-content/uploads/2015/04/file-formats.pdf), page 9 & 10.
```python
element_types = [
    5,5,5,5 # VTK_TRIANGLE
]
```
On the other side one can use the ImportMesh() method to get the mesh sent by the export:

```Python
info = CoSimIO.Info()
info.SetString("identifier", "fluid_mesh")
info.SetString("connection_name", "test_connection")
return_info, nodal_coords, element_connectivities, element_types = CoSimIO.ImportMesh(info)
```







