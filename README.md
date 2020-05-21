# NinjaMasters
Masters Compiled with Ninja

# Requirements
CMake Version 3.0 or greater
Ninja version 1.10 or greater
Mingw is recommended for compiling on windows

# Compiling with Ninja
mkdir build 
cd build
cmake .. -G Ninja
ninja

bc and bz will be generated in the build directory 

#Style Guide
snake case for variable names: my_variable
trailing underscore for class member variables: my_member_
pascal case for classes and type definitions: MyClass
pascal case for functions: MyFunction

indentation is 4 spaces
always indent when going into a new scope and outdent when leaving scope
maximum line length is 120 characters




