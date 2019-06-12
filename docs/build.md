---
title: Build From Source
lang: en
---
## Compiling using the Projucer

If you have JUCE, you probably know how to use the Projucer.
In this case, just open the `.jucer` file and go at it.
If you don't know JUCE, you can download it from [here][]
or clone the [repository][].
From there, unpack/unzip/untar/... and go to `extras/Projucer/Builds`.
Select the proper build system for your platform;
for Visual Studio you have `.sln` files that you can open and build,
for Linux you have Makefiles.
Once compiled, run the application and open the project.
You can then target your preferred platform or development environment.
Note that the source code requires C++17.

[here]: https://shop.juce.com/get-juce
[repository]: https://github.com/WeAreROLI/JUCE

## Compiling using CMake

I provide my personal CMake hack, which I use because it allows me to

1. use something other than Visual Studio if I want to
2. have multiple targets (e.g. tests...).

You will need to edit the file with you JUCE path for it to work,
but it's straightforward otherwise.
Create a `build` directory within the repository, `cd` into it, and run
```
cmake ..
cmake --build . -j 8 --config Release
```
There are 3 targets:
- `rhythmbox_Standalone` for the standalone application
- `rhythmbox_VST` for the VST3 plugin
- `rhythmbox_Test` for the test suite.

Replace 8 by your number of cores, and Release with Debug if you wish.
