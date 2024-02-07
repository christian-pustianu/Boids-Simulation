# Boids-Simulation

Simulation of natural schooling behaviour in fish. Programmed in C++ with OpenGL, GLFW, and various other libraries (described in `third_party.md`). All `.obj` models were created by me in Blender.

A demo of this simulation can be found [here](https://youtu.be/mlxYGclOZXI) and the report of the dissertation [here](https://github.com/christian-pustianu/Boids-Simulation/blob/main/Dissertation%20Report.pdf).

# User Manual

## Building on Linux (command line)

Make sure your device has a recent version of the C++ compiler by running
```
> g++ --version
```
Make sure you are in the directory containing the premake files (e.g., `premake5`, ...). Then, run the `premake5` command to generate the makefiles:

```
> ./premake5 gmake2
```

Note: On certain devices, running this command results in a `permission denied` error. In that case, run:
```
> chmod +x premake5
```
and then try running the `premake5` command again.

### Building and running *debug* version

Use `make` to build the project:
```
> make -j6
==== Building x-glad (debug_x64) ====
==== Building x-glfw (debug_x64) ====
(...)
Linking main
```
Note: Depending on your CPU, the `-j6` flag can be changed to any number, so that build is run in parallel on that many processes.

To run the build, use:
```
> ./bin/main-debug-x64-gcc.exe
```

### Building and running *release* version

Use `make` to build the project:
```
> make -j6 config=release_x64
==== Building x-glad (release_x64) ====
==== Building x-glfw (release_x64) ====
(...)
Linking main
```
To run the build, use:
```
> ./bin/main-release-x64-gcc.exe
```

***

## Building on Windows (Visual Studio)

To generate the Visual Studio solution and project files, use `premake` from the command line:
```
> .\premake5.exe vs2022
```
Open the solution file `Boids-Simulaion.sln` and press `F5` to run it. For the *release* version, find the dropdown menu with *debug* and select `release` instead. Press `F5` afterwards to run the release version.

***

## Application Controls

### Camera controls

| Option | Key |
|--------|:---:|
| Locked Height Arc-ball Camera | PRESS [1] |
| Top-Down View Camera | PRESS [2] |
| Fly-Through Camera | PRESS [3] |
| Third-Person Camera | PRESS [4] |
| Rotate camera | MOVE MOUSE |
| Move forward/backwards | PRESS [W]/[S] |
| Move left/right | PRESS [A]/[D] |
| Move down/up | PRESS [Q]/[E] |
| Increase Camera Speed | PRESS [SHIFT] |
| Decrease Camera Speed | PRESS [CTRL] |

### Other controls

| Option | Key |
|--------|:---:|
| Pause/play simulation | PRESS [SPACE] |
| Toggle Technical View | PRESS [T] |
| Toggle GUI | PRESS [G] |
| Control target point on Y-axis | HOLD [MOUSE LEFT BUTTON] |
| Control target point on XZ-plane | HOLD [MOUSE RIGHT BUTTON] |


### Modify simulation parameters -> in the GUI
