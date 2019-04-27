# RhythmBox

This application is a live drummer that you can control using buttons or MIDI CC with at least 1 controller, as an arranger or a [BeatBuddy](https://singularsound.com/) pedal would.
The drum loops are described in simple JSON files.
The application either outputs MIDI notes, and can also generate sounds internally using SFZ instruments.
For now the sound generation uses [SFZero](http://stevefolta.github.io/SFZero/), which has many limitations.
It is possible to use it within a VST host with any drum machine, or with an external sound generator.
Since it uses [JUCE](https://juce.com/), it should be almost cross platform, although I only tested Windows and Linux, and a tiny bit of Android.
Contributions and comments are welcome, as well as beat description files.
I'll put up some forum or sharing platform at some point.

## Installation

Windows binaries are available [here](https://github.com/paulfd/rhythmbox/releases), along with Raspberry Pi binaries.

### Compiling using the Projucer

If you have JUCE, you probably know how to use the Projucer.
In this case, just open the `.jucer` file and go at it.
If you don't know JUCE, you can download it from [here](https://shop.juce.com/get-juce) or clone the repository [here](https://github.com/WeAreROLI/JUCE).
From there, unpack/unzip/untar/... and go to `extras/Projucer/Builds`.
Select the proper build system for your platform; for Visual Studio you have `.sln` files that you can open and build, for Linux you have Makefiles.
Once compiled, run the application and open the project.
You can then target your preferred platform or development environment.
Note that the source code requires C++17.

### Compiling using CMake

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

## Beat descriptions

You need at least beat description files for this application to be useful.
Sadly, I couldn't find much free to use MIDI files (and I am quite sure royalty free does not mean that I can freely redistribute files).
I mainly use files from Groove Monkee, which are pretty nice, but you may use anything from your library.
Although the MIDI files are not free to redistribute, the beat description files are.
These beat descriptions are either free `.json` files targeting MIDI files in relative or absolute fashion, or `.zip` files with MIDI files included in the archive.
When constructed as a `.zip` file the beat description file should be named `description.json` and placed in the archive along with the MIDI files.

### Using the free Groove Monkee MIDI files

I built some beat descriptions that uses the [free midi pack from Groove Monkey](https://groovemonkee.com/pages/free-midi-loops).
You can register and download the pack, and put the `.json` files at the root of the directory (the one with all the directories "Ballad, Big Easy, ...).
You should then be able to add these in the application.

### Using paid Groove Monkee files

I put the beat descriptions I made for some of the other Groove Monkee packs in here.
I'll update them if I make more (or receive them from someone!).

### Other files

You can use any midi file for this to work, either from VST drum libraries (many come with MIDI files included) or even BeatBuddy files.
I don't have much on hand but if you do create such files, feel free to contact me and I'll put them up here.

### Beat description format

The format for these files looks like the following:
```json
{
    "name": "Slow Blues A",
    "group": "Blues",
    "bpm" : 78,
    "quarters_per_bar": 4,
    "intro": { "filename" : "Blues GM/Intros/Snare 01.mid", "bars": 1 },
    "ending": { "filename" : "Blues GM/Endings/080 BPM/080 Endings 01.mid", "bars": 2 },
    "parts": [
        {
            "name": "Hat loop",
            "midi_file" : { "filename" : "Blues GM/8-Bar Blues/078 Slow Blues A/Hat Grooves/Slow Blues A Hats F1 S.mid", "bars": 4, "ignore_bars": 1 },
            "fills": [
                { "filename" : "Blues GM/8-Bar Blues/078 Slow Blues A/Hat Grooves/Slow Blues A Hats F2 S.mid", "bars": 1, "ignore_bars": 7 },
                { "filename" : "Blues GM/8-Bar Blues/078 Slow Blues A/Hat Grooves/Slow Blues A Hats F3 M.mid", "bars": 1, "ignore_bars": 7 },
                { "filename" : "Blues GM/8-Bar Blues/078 Slow Blues A/Hat Grooves/Slow Blues A Hats F4 M.mid", "bars": 1, "ignore_bars": 7 },
                { "filename" : "Blues GM/8-Bar Blues/078 Slow Blues A/Hat Grooves/Slow Blues A Hats F5 M.mid", "bars": 1, "ignore_bars": 7 },
                { "filename" : "Blues GM/8-Bar Blues/078 Slow Blues A/Hat Grooves/Slow Blues A Hats F6 L.mid", "bars": 1, "ignore_bars": 7 }
            ],
            "transition": { "filename" : "Blues GM/8-Bar Blues/078 Slow Blues A/Hat Grooves/Slow Blues A Hats F1 S.mid", "bars": 1, "ignore_bars": 7 }
        },
        {
            "name": "Ride loop",
            "midi_file" : { "filename" : "Blues GM/8-Bar Blues/078 Slow Blues A/Ride Grooves/Slow Blues A Ride F1 S.mid", "bars": 4, "ignore_bars": 1},
            "fills": [
                { "filename" : "Blues GM/8-Bar Blues/078 Slow Blues A/Ride Grooves/Slow Blues A Ride F2 S.mid", "bars": 1, "ignore_bars": 7 },
                { "filename" : "Blues GM/8-Bar Blues/078 Slow Blues A/Ride Grooves/Slow Blues A Ride F3 M.mid", "bars": 1, "ignore_bars": 7 },
                { "filename" : "Blues GM/8-Bar Blues/078 Slow Blues A/Ride Grooves/Slow Blues A Ride F4 M.mid", "bars": 1, "ignore_bars": 7 },
                { "filename" : "Blues GM/8-Bar Blues/078 Slow Blues A/Ride Grooves/Slow Blues A Ride F5 M.mid", "bars": 1, "ignore_bars": 7 },
                { "filename" : "Blues GM/8-Bar Blues/078 Slow Blues A/Ride Grooves/Slow Blues A Ride F6 L.mid", "bars": 1, "ignore_bars": 7 }
            ],
            "transition": { "filename" : "Blues GM/8-Bar Blues/078 Slow Blues A/Ride Grooves/Slow Blues A Ride F1 S.mid", "bars": 1, "ignore_bars": 7 }
        }
    ]
}
```
You have a header with the general information about the file, among which:
- `name`: the beat name
- `group`: the style, or any group you may wish this file to be in
- `bpm`: the basic BPM for the beat. This will override the BPM registered in the MIDI files.
- `quarters_per_bar`: the number of quarter notes per bar. Note that 6/8 is 3 quarters. I may make this more precise in the future.
- `intro` and `ending`: both of these are optional. If they are present, they should contain a sub-dictionary with a mandatory `filename` field, which is the MIDI file name in relative or absolute path, and optional fields `bars` and `ignore_bars` that indicate the loop length within the MIDI file and how many bars to ignore at the beginning.

Following the header, there's an array named `parts`.
Each part contains:
- `name`: the part name
- `midi_file`: the main loop file, described as with `intro` and `ending`
- `fills`: an array of MIDI file descriptions which contains the fills for the part.
- `transition`: the MIDI file description for the transition fill to the next part.

Parts are always changed in sequence within the application.
Fills can be randomly or sequentially launched.

## Drum SFZ files

Sadly SFZero is not quite up to par and lacks many advanced features for proper drum sounds reproduction.
You can more or less use the drumkits from the good people over at AVLinux directly (see [here](http://www.bandshed.net/avldrumkits/)) and they sound OK.
You can also host the RhythmBox plugin and use any drum library out there; there are very good free ones, most notably [this one](https://www.powerdrumkit.com/).
Note that hosting the plugin means that the tempo is synced to the host tempo though, so it's sometimes impractical; working on adding an option to lift this.
There are other SFZ libraries out there, and you can quite easily create your own since SFZ files are just description files for samples.
Improving SFZero or rewriting an SFZ plugin is in my pipeline, so stay tuned.

## Basic usage

The usage is quite straightforward.
Upon launching the application you have to select at least a beat description by clicking on Rhythms.
You will be presented with a list of the current beat descriptions you have previously entered, as well as a button to add more.
To load a rhythm, just double click it.
You may also load an SFZ file in the same way, or use the MIDI output.

Once the rhythm is loaded, you can use the buttons to launch the rhythm, fills and parts.
You can also use a midi controller by setting the proper input channel and input CC in the options.
After this, the behavior is:
- a tap starts the live rhythm, and if already started it makes a fill.
- a long tap transitions into the next part.
- 2 taps trigger the ending.

## Contact and contributions

Feel free to post an issue, or contact me by email if you need.
Any contribution is welcome.
There are many things to improve in the code and most notably in the GUI which is terrible but I'm very bad at this.
However the best is probably to build beat descriptions/sfz files and share the love! :)

## Acknowledgments

The original SFZero code is from Steve Folta [http://stevefolta.github.io/SFZero/].
There are many updates to the original codebase.
The one I'm using has been improved by Leo Olivers and cleaned up by MatkatMusic [https://github.com/matkatmusic].

JUCE is owned by ROLI [https://roli.com/].
