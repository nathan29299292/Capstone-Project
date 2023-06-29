# Jelly Bean Dropper
## Build Instructions
### Pattern Generator and G-code Generator
**DOES NOT WORK ON WINDOWS YET.**

We are using CMake for both the Pattern Generator program which is able to
generate both the pattern and the G-code for the Jelly Bean Dropper itself.

#### Required Dependencies
- CMake
- glfw3
- glew

#### Build Instructions

First create a build folder within the Server folder and change the current
directory into that build folder,

```console
mkdir Server/build; cd Server/build
```

Next, generate the Makefiles using CMake,

```console
cmake ..
``` 

Lastly, run `make`,

```console
make
``` 

This should create `PatternGen` and also copy the needed files into the same
directory in order to run it.
