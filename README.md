# RhythmBox

This application is a live drummer that you can control using buttons or MIDI CC with at least 1 controller.
The drum loops are described in simple JSON files.
The application either outputs MIDI notes, and can also generate sounds internally using SFZ instruments.
For now the sound generation uses [SFZero](http://stevefolta.github.io/SFZero/), which has many limitations, which I may lift in the future given the time.
It is possible to use it within a VST host with any drum machine, or with an external sound generator.
Since it uses [JUCE](https://juce.com/), it should be almost cross platform, although I only tested Windows and Linux, and a tiny bit of Android.
Contributions and comments are welcome, as well as beat description files.
I'll put up some forum or sharing platform as soon as I can.

# Installation

Windows binaries are available [here](), along with Raspberry Pi binaries.

## Compiling using the Projucer

If you have JUCE, you probably know how to use the Projucer.
In this case, just open the `.jucer` file and go at it.
If you don't know JUCE, you can download it from [here](https://shop.juce.com/get-juce) or clone the repository [here](https://github.com/WeAreROLI/JUCE).
From there, unpack/unzip/untar/... and go to `extras/Projucer/Builds`.
Select the proper build system for your platform; for Visual Studio you have `.sln` files that you can open and build, for Linux you have Makefiles.
Once compiled, run the application and open the project.
You can then target your preferred platform or development environment.
Note that the source code requires C++17.

## Compiling using CMake

I provide my personal CMake hack, which I use because it allows me to i) use something other than Visual Studio if I want to, and ii) have multiple targets (e.g. tests...).
You will need to edit the file with you JUCE path for it to work, but it's straightforward otherwise.
Create a `build` directory within the repository, `cd` into it, and run
```
cmake ..
cmake --build . -j 8 --config Release
```
There are 3 targets:
- `rhythmbox_Standalone` for the standalone application
- `rhythmbox_VST` for the VST3 plugin
- `rhythmbox_Test` for the test suite
Replace 8 by your number of cores, and Release with Debug if you wish.
