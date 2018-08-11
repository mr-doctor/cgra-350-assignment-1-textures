# CGRA 350 Real-time 3D Computer Graphics T2/2018 OpenGL Assignment 1

# Instructions

All parts of the assignment can be implemented by modifying the file `ex1.cpp` in the `work/src/`
folder. You may have to look at some of the other files to know what functions to
call, `work/src/ex1.hpp` for example.

Documentation for ImGui can be found in `work/vendor/imgui/imgui.h` and
`work/vender/imgui/imgui.cpp`. `imgui.h` is the header file and contains the definitions of the
functions you'll need to use, as well some basic documention. `imgui.cpp` contains more complete
documentation on how to use ImGui.

# External Library Documentation

* The [OpenGL Wiki](https://www.khronos.org/opengl/wiki/) has a wide variery of information on
  OpenGL as well as reference documentation.
* GLM documentation can be found here: [Manual](http://glm.g-truc.net/0.9.8/glm-0.9.8.pdf),
  [API Documentation](http://glm.g-truc.net/0.9.8/api/index.html)
* The [GLSL specification](https://khronos.org/registry/OpenGL/specs/gl/GLSLangSpec.4.50.pdf) may
  also be useful for working with GLM.
  See sections 5.4 and 8 for mathematics-specific documentation.
* GLFW documentation can be found here: http://www.glfw.org/docs/3.1.1/

# Requirements

The project requires [CMake](https://cmake.org/) to build before compiling. The recommended way to build to project is to create a build folder then using CMake to create to project inside it. The following sections are a guides to building common project types.
```sh
$ mkdir build
```

This project also requires OpenGL v3.3 and a suitable C++11 compiler.


## Linux

#### Command Line

The simpliest way to set up a basic project is to run the shell script `f5.sh` which runs the `cmake`, `make` and run commands for you.
```sh
$ ./f5.sh
```

Alternativiely you can run the commands manually.
```sh
$ cd build
$ cmake ../work
$ make
$ cd ..
```

If the project builds without errors the executable should be located in the `build/bin/` directory and can be run with:
```sh
$ ./build/bin/a1 [args...]
```

#### Eclipse
Setting up for [Eclipse](https://eclipse.org/) is a little more complicated. Navigate to the build folder and run `cmake` for Eclipse.
```sh
$ cd build
$ cmake  -G "Eclipse  CDT4 - Unix  Makefiles" ../work
```
Start Eclipse and go to `File > Import > Existing Projects into Workspace`, browse to and select the `build/` directory as the project. Make sure  the  box `Copy  Projects into Workspace` is unchecked. Once you've imported the project, and are unable run it, do the following:
 - Go to `Run > Run  Configurations`.  On the left side, select C/C++  Application, then in the `Main` tab, make sure your C/C++ Application field contains `./bin/a1` and that `Enable auto build` is checked.
 - On your project, `[right click] > Run As > C/C++  Application`.  This should setup the default way to start the program, so you can simply run the project anytime after that.

If  you  need  to  run  with  arguments  (and  you  will  with  some  projects)  go  to `Run > Run Configurations > Arguments` and enter your arguments there. For example: `./work/res/models/bunny.obj `



## Windows

#### Visual Studio

This project may require at least Visual Studio 2013. You can get [Visual Studio Community 2017](https://www.visualstudio.com/downloads/) for free from Microsoft.

| Product |  XX  |
|:-------:|:----:|
| Visual Studio 2013 | 12 |
| Visual Studio 2015 | 14 |
| Visual Studio 2017 | 15 |

Run the `cmake` command for Visual Studio with the appropriate version number (XX).
```cmd
> cmake -G "Visual Studio XX" ..\work
```

Or if you are building for 64-bit systems.
```cmd
> cmake -G "Visual Studio XX Win64" ..\work
```

See runcmake.bat for an example.

After opening the solution (`.sln`) you will need to set some additional variables before running.
 - `Solution Explorer > a1 > [right click] > Set as StartUp Project`
 - `Solution Explorer > a1 > [right click] > Properties > Configuration Properties > Debugging`
 -- Select `All Configurations` from the configuration drop-down
 -- Set `Working Directory` to `(SolutionDir)../work`
 -- Set `Command Arguments` to whatever is required by your program



## OSX

#### XCode

[Xcode](https://developer.apple.com/xcode/) is an IDE that offers a little more than simple text editing. The setup again is very similar to Eclipse.
```sh
$ cd build
$ cmake -G "Xcode" ../work
$ cd ..
```

Once you're setup, you can build your project with Xcode, but have to execute your program with the terminal (making sure you are in the root directory).
```sh
$ ./build/bin/a1 [args..]
```
