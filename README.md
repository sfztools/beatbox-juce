# RhythmBox

This application is a live drummer that you can control using buttons or MIDI CC with at least 1 controller, as an arranger or a [BeatBuddy](https://singularsound.com/) pedal would.
The drum loops are described in simple JSON files.
The application either outputs MIDI notes, and can also generate sounds internally using SFZ instruments.
For now the sound generation uses [SFZero](http://stevefolta.github.io/SFZero/), which has many limitations, which I may lift in the future given the time.
It is possible to use it within a VST host with any drum machine, or with an external sound generator.
Since it uses [JUCE](https://juce.com/), it should be almost cross platform, although I only tested Windows and Linux, and a tiny bit of Android.
Contributions and comments are welcome, as well as beat description files.
I'll put up some forum or sharing platform as soon as I can.

# Installation

Windows binaries are available [here](https://github.com/paulfd/rhythmbox/releases), along with Raspberry Pi binaries.

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

# Beat descriptions

You need at least beat description files for this application to be useful.
Sadly, I couldn't find much free to use MIDI files (and I am quite sure royalty free does not mean that I can freely redistribute files).
I mainly use files from Groove Monkee, which are pretty nice, but you may use anything from your library.
Although the MIDI files are not free to redistribute, the beat description files are.

## Using the free Groove Monkee MIDI files

I built 

## Using paid Groove Monkee files

## Other files

You can use any midi file for this to work, either from VST drum libraries (many come with MIDI files included) or even BeatBuddy files.
I don't have much on hand but if you do create such files, feel free to contact me and I'll put them up here.

# Drum SFZ files

Sadly SFZero is not quite up to par and lacks many advanced features for proper drum sounds reproduction.
You can more or less use the drumkits from the good people over at AVLinux directly (see [here](http://www.bandshed.net/avldrumkits/)) and they sound OK.
You can also host the RhythmBox plugin and use any drum library out there; there are very good free ones, most notably [this one](https://www.powerdrumkit.com/).
Note that hosting the plugin means that the tempo is synced to the host tempo though, so it's sometimes impractical; working on adding an option to lift this.
There are other SFZ libraries out there, and you can quite easily create your own since SFZ files are just description files for samples.
Improving SFZero or rewriting an SFZ plugin is in my pipeline, so stay tuned.

# Contact and contributions

Feel free to post an issue, or contact me by email if you need.
Any contribution is welcome.
There are many things to improve in the code and most notably in the GUI which is terrible but I'm very bad at this.
However the best is probably to build beat descriptions/sfz files and share the love! :)
