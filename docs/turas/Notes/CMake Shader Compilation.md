[[CMake]] [[Shaders]] compilation

https://thatonegamedev.com/cpp/cmake/how-to-compile-shaders-with-cmake/

We have a TurasUtils cmake file that handles auto shader compilation for the project
the shaders are compiled as part of the turas engine project. Each project that depends on the shaders (editor, runtime, tests etc.) then simply need to have this somewhere in the CMakeLists.txt:
```cmake
add_shaders_dependency(YourTargetName)
```


