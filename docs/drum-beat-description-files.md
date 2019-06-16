---
title: "Beat Description Files"
lang: "en"
---
You need at least beat description files for this application to be useful.
Sadly, I couldn't find much free to use MIDI files (and I am quite sure royalty
free does not mean that I can freely redistribute files).
I mainly use files from Groove Monkee, which are pretty nice,
but you may use anything from your library.
Although the MIDI files are not free to redistribute,
the beat description files are.
These beat descriptions are either free `.json` files targeting MIDI files
in relative or absolute fashion, or `.zip` files with MIDI files
included in the archive.
When constructed as a `.zip` file the beat description file should be named
`description.json` and placed in the archive along with the MIDI files.

## Using the free Groove Monkee MIDI files

I built some beat descriptions that uses the [free midi pack from Groove Monkey][].
You can register and download the pack, and put the `.json` files
at the root of the directory (the one with all the directories "Ballad, Big Easy, ...).
You should then be able to add these in the application.
I also provide complete packages with MIDI files from their ["Beat Farm"][].
These are provided as `.zip` files.
You don't need to extract them, just add them to your Rhythm library :)

["Beat Farm"]: https://groovemonkee.com/pages/beat-farm-free-midi-beats
[free midi pack from Groove Monkey]: https://groovemonkee.com/pages/free-midi-loop

## Using paid Groove Monkee files

I put the beat descriptions I made for some of the other Groove Monkee packs in here.
I'll update them if I make more (or receive them from someone!).

## Other files

You can use any midi file for this to work, either from VST drum libraries
(many come with MIDI files included) or even BeatBuddy files.
I don't have much on hand but if you do create such files,
feel free to contact me and I'll put them up here.

## Beat description format

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
- `bpm`: the basic BPM for the beat. This will override the BPM registered
in the MIDI files.
- `quarters_per_bar`: the number of quarter notes per bar.
Note that 6/8 is 3 quarters. I may make this more precise in the future.
- `intro` and `ending`: both of these are optional. If they are present,
they should contain a sub-dictionary with a mandatory `filename` field, which is
the MIDI file name in relative or absolute path, and optional fields `bars`
and `ignore_bars` that indicate the loop length within the MIDI file and
how many bars to ignore at the beginning.
You can also add an optional `replace_notes` field
that contains an array of pairs (e.g. `[[37, 41]]` or `[[37, 41], [42, 53]]`).

Following the header, there's an array named `parts`.
Each part contains:
- `name`: the part name
- `midi_file`: the main loop file, described as with `intro` and `ending`
- `fills`: an array of MIDI file descriptions which contains the fills for the part.
- `transition`: the MIDI file description for the transition fill to the next part.

Parts are always changed in sequence within the application.
Fills can be randomly or sequentially launched.
