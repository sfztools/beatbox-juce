---
title: "Home"
lang: "en"
layout: "home"
date_fmt: "%B %d, %Y"
---
<div markdown="1" class="jumbotron p-4 mb-3">

This application is a live drummer that you can control using buttons or MIDI CC
with at least 1 controller, as an arranger or a [BeatBuddy] pedal would.
The drum loops are described in simple [JSON files].
The application either outputs MIDI notes, and can also generate sounds
internally using SFZ instruments.
For now the sound generation uses [SFZero], which has many limitations.
It is possible to use it within a VST host with any drum machine,
or with an external sound generator.
Since it uses [JUCE], it should be almost cross platform,
although I only tested Windows and Linux, and a tiny bit of Android.
Contributions and comments are welcome, as well as beat description files.
I'll put up some forum or sharing platform at some point.

[![Travis](https://img.shields.io/travis/com/sfztools/beatbox.svg?label=Linux-macOS&style=popout&logo=travis)](https://travis-ci.com/sfztools/beatbox)
[![AppVeyor](https://img.shields.io/appveyor/ci/sfztools/beatbox.svg?label=Windows&style=popout&logo=appveyor)](https://ci.appveyor.com/project/sfztools/beatbox)

![Screenshot](/assets/img/screenshot.png)

</div>

## Installation

Windows binaries are available on the [releases page],
along with Raspberry Pi binaries.

See the [build documentation page] about how to build the application
from the source code.

## Usage

The usage is quite straightforward.
Upon launching the application you have to select at least a [beat description]
by clicking on Rhythms.
You will be presented with a list of the current beat descriptions you have
previously entered, as well as a button to add more.
To load a rhythm, just double click it.
You may also load an [SFZ file] in the same way, or use the MIDI output.

Once the rhythm is loaded, you can use the buttons to launch the rhythm,
fills and parts.
You can also use a midi controller
by setting the proper input channel and input CC in the options.
After this, the behavior is:
- a tap starts the live rhythm, and if already started it makes a fill.
- a long tap transitions into the next part.
- 2 taps trigger the ending.

## Contact and contributions

Feel free to post an issue, or contact me by email at paulfd@outlook.fr if you need.
Any contribution is welcome.
There are many things to improve in the code and most notably in the GUI
which is terrible but I'm very bad at this.
However the best is probably to build beat descriptions/sfz files
and share the love! :)

## Acknowledgments

The original SFZero code is from [Steve Folta].
There are many updates to the original codebase.
The one I'm using has been improved by [Leo Olivers]
and cleaned up by [MatkatMusic].

JUCE is owned by [ROLI].

[BeatBuddy]: https://singularsound.com/
[beat description]: {{ "/drum-beat-description-files" | relative_url }}
[build documentation page]: {{ "/build" | relative_url }}
[JUCE]: https://juce.com/
[JSON files]: {{ "/beat" | relative_url }}
[Leo Olivers]: https://github.com/altalogix/SFZero
[MatkatMusic]: https://github.com/matkatmusic/SFZero
[releases page]: https://github.com/sfztools/beatbox/releases
[ROLI]: https://roli.com/
[SFZ file]: {{ "drum-sfz-files" | relative_url }}
[SFZero]: http://stevefolta.github.io/SFZero/
[Steve Folta]: http://stevefolta.github.io/SFZero/

<!--
## Latest News

{% include post.html %}
-->
