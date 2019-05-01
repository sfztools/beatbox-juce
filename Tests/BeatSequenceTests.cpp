#include "../JuceLibraryCode/JuceHeader.h"
#include "catch2/catch.hpp"
#include "../Source/BeatSequence.h"
#include <functional>
#include <numeric>
#include <algorithm>
#include <random>
using namespace Catch::literals;

/**
 * Compare 2 containers, with the size of the LHS
 */
template<class T>
constexpr bool equalElements (const T& lhs, const T& rhs)
{
    return std::equal(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

struct SequenceTimestamps
{
    std::vector<double> noteOnTimeStamps;
    std::vector<double> noteOffTimeStamps;   
};

bool operator==(const SequenceTimestamps& lhs, const SequenceTimestamps& rhs)
{
    return equalElements(lhs.noteOnTimeStamps, rhs.noteOnTimeStamps) && equalElements(lhs.noteOffTimeStamps, rhs.noteOffTimeStamps);
}

SequenceTimestamps extractTimestamps(BeatSequence& sequence)
{
    std::vector<double> noteOns;
    std::vector<double> noteOffs;
    noteOns.reserve(sequence.getNumEvents());
    noteOffs.reserve(sequence.getNumEvents());
    for (const auto & note: sequence)
    {
        noteOns.push_back(note.getTimeStamp());
        noteOffs.push_back(note.getNoteOffTimeStamp());
    }
    return {noteOns, noteOffs};
};

std::vector<int> extractNotes(BeatSequence& sequence)
{
    std::vector<int> noteNumbers;
    noteNumbers.reserve(sequence.getNumEvents());
    for (const auto & note: sequence)
    {
        noteNumbers.push_back(note.getNoteNumber());
    }
    return noteNumbers;
};

TEST_CASE("BeatSequence constructors", "[BeatSequence]")
{
    SECTION("Default constructor")
    {
        auto sequence = BeatSequence();
        REQUIRE( sequence.getNumEvents() == 0 );
    }

    SECTION("Quarter per bars = 4")
    {
        BeatSequence sequence(4);
        REQUIRE( sequence.getQuartersPerBar() == 4 );
    }
    SECTION("Quarter per bars = 8")
    {
        BeatSequence sequence(8);
        REQUIRE( sequence.getQuartersPerBar() == 8 );
    }
}

TEST_CASE("BeatSequence quarter per bar invariants", "[BeatSequence]")
{
    SECTION("Valid value on constructor")
    {
        auto sequence = BeatSequence(4);
        REQUIRE( sequence.getQuartersPerBar() == 4 );
    }

    SECTION("Negative value on constructor")
    {
        auto sequence = BeatSequence(-1);
        REQUIRE( sequence.getQuartersPerBar() == 1 );
    }

    SECTION("Zero value on constructor")
    {
        auto sequence = BeatSequence(0);
        REQUIRE( sequence.getQuartersPerBar() == 1 );
    }

    auto sequence = BeatSequence(4);

    SECTION("Changing to a negative value")
    {
        sequence.setQuartersPerBar(-1);
        REQUIRE( sequence.getQuartersPerBar() == 1 );
    }

    SECTION("Changing to a zero value")
    {
        sequence.setQuartersPerBar(0);
        REQUIRE( sequence.getQuartersPerBar() == 1 );
    }
}

TEST_CASE("BeatSequence sorting", "[BeatSequence]")
{
    const double noteDuration = 0.1;
    const int noteNumber = 1;
    const int channel = 1;
    const uint8 velocity = 63;

    auto refSequence = BeatSequence();
    for (auto timestamp: {0.0, 1.0, 1.5, 4.0})
    {
        refSequence.addEvent({noteNumber, channel, velocity, timestamp, noteDuration});    
    }
    auto [refNoteOnTimestamps, refNoteOffTimestamps] = extractTimestamps(refSequence);

    SECTION("Shuffled sorting")
    {
        auto shuffledSequence = BeatSequence();
        for (auto timestamp: {4.0, 1.0, 1.5, 0.0})
            shuffledSequence.addEvent({noteNumber, channel, velocity, timestamp, noteDuration});    
        
        auto [noteOnTimestamps, noteOffTimestamps] = extractTimestamps(shuffledSequence);
        REQUIRE( equalElements(noteOnTimestamps, refNoteOnTimestamps) );
        REQUIRE( equalElements(noteOffTimestamps, refNoteOffTimestamps) );
    }
}

TEST_CASE("BeatSequence getLastBarTime", "[BeatSequence]")
{
    const double noteDuration = 0.1;
    const int noteNumber = 1;
    const int channel = 1;
    const uint8 velocity = 63;
    

    SECTION("Get last bar time (4 beats per bar, less than 2 bar long)")
    {
        auto sequence = BeatSequence(4);
        for (auto timestamp: {0.0, 1.0, 1.5, 2.6, 3.2, 4.0, 5.3, 6.1, 6.9})
            sequence.addEvent({noteNumber, channel, velocity, timestamp, noteDuration});    
        
        REQUIRE( sequence.getLastBarTime() == 8.0 );
    }

    SECTION("Get last bar time (4 beats per bar, slightly less than 2 bars)")
    {
        auto sequence = BeatSequence(4);
        for (auto timestamp: {0.0, 1.0, 1.5, 2.6, 3.2, 4.0, 5.3, 6.1, 7.9})
            sequence.addEvent({noteNumber, channel, velocity, timestamp, noteDuration});    
        
        REQUIRE( sequence.getLastBarTime() == 8.0 );
    }

    SECTION("Get last bar time (4 beats per bar, slightly more than 2 bars)")
    {
        auto sequence = BeatSequence(4);
        for (auto timestamp: {0.0, 1.0, 1.5, 2.6, 3.2, 4.0, 5.3, 6.1, 8.01})
            sequence.addEvent({noteNumber, channel, velocity, timestamp, noteDuration});    
        
        REQUIRE( sequence.getLastBarTime() == 12.0 );
    }

    SECTION("Get last bar time (4 beats per bar, exactly 1 bar)")
    {
        auto sequence = BeatSequence(4);
        for (auto timestamp: {0.0, 1.0, 1.5, 2.6, 3.2, 4.0})
            sequence.addEvent({noteNumber, channel, velocity, timestamp, noteDuration});    
        
        REQUIRE( sequence.getLastBarTime() == 4.0 );
    }

    SECTION("Get last bar time (3 beats per bar, less than 2 bar long)")
    {
        auto sequence = BeatSequence(3);
        for (auto timestamp: {0.0, 1.0, 1.5, 2.6, 3.2, 4.0, 5.3 })
            sequence.addEvent({noteNumber, channel, velocity, timestamp, noteDuration});    
        
        REQUIRE( sequence.getLastBarTime() == 6.0 );
    }

    SECTION("Get last bar time (3 beats per bar, slightly less than 3 bars)")
    {
        auto sequence = BeatSequence(3);
        for (auto timestamp: {0.0, 1.0, 1.5, 2.6, 3.2, 4.0, 5.3, 6.1, 8.99})
            sequence.addEvent({noteNumber, channel, velocity, timestamp, noteDuration});    
        
        REQUIRE( sequence.getLastBarTime() == 9.0 );
    }

    SECTION("Get last bar time (3 beats per bar, slightly more than 4 bars)")
    {
        auto sequence = BeatSequence(3);
        for (auto timestamp: {0.0, 1.0, 1.5, 2.6, 3.2, 4.0, 5.3, 6.1, 9.01})
            sequence.addEvent({noteNumber, channel, velocity, timestamp, noteDuration});    
        
        REQUIRE( sequence.getLastBarTime() == 12.0 );
    }
}

TEST_CASE("getBarAtTimes and getEventsWithin", "[BeatSequence]")
{
    const double noteDuration = 0.1;
    const int noteNumber = 1;
    const int channel = 1;
    const uint8 velocity = 63;
    auto sequence = BeatSequence(1);
    for (auto timestamp: {0.0, 0.1, 0.5, 0.99, 1.0, 1.01,  1.5, 1.999, 2.0, 2.5, 3.1, 3.7})
        sequence.addEvent({noteNumber, channel, velocity, timestamp, noteDuration});    
    
    SECTION("getEventsWithin 1")
    {
        auto refSequence = BeatSequence(1);
        for (auto timestamp: {1.0, 1.01})
            refSequence.addEvent({noteNumber, channel, velocity, timestamp, noteDuration});
        
        auto [refNoteOnTimestamps, refNoteOffTimestamps] = extractTimestamps(refSequence);
        
        auto testSequence = sequence.getEventsWithin(1.0, 1.5);
        auto [noteOnTimestamps, noteOffTimestamps] = extractTimestamps(*testSequence);
        REQUIRE( equalElements(noteOnTimestamps, refNoteOnTimestamps) );
        REQUIRE( equalElements(noteOffTimestamps, refNoteOffTimestamps) ); 
    }

    SECTION("getEventsWithin 2")
    {
        auto refSequence = BeatSequence(1);
        for (auto timestamp: {1.0, 1.01})
            refSequence.addEvent({noteNumber, channel, velocity, timestamp, noteDuration});
        
        auto [refNoteOnTimestamps, refNoteOffTimestamps] = extractTimestamps(refSequence);
        
        auto testSequence = sequence.getEventsWithin(0.999, 1.5);
        auto [noteOnTimestamps, noteOffTimestamps] = extractTimestamps(*testSequence);
        REQUIRE( equalElements(noteOnTimestamps, refNoteOnTimestamps) );
        REQUIRE( equalElements(noteOffTimestamps, refNoteOffTimestamps) ); 
    }

    SECTION("getEventsWithin 3")
    {
        auto refSequence = BeatSequence(1);
        for (auto timestamp: {1.0, 1.01, 1.5})
            refSequence.addEvent({noteNumber, channel, velocity, timestamp, noteDuration});
        
        auto [refNoteOnTimestamps, refNoteOffTimestamps] = extractTimestamps(refSequence);
        
        auto testSequence = sequence.getEventsWithin(1.0, 1.6);
        auto [noteOnTimestamps, noteOffTimestamps] = extractTimestamps(*testSequence);
        REQUIRE( equalElements(noteOnTimestamps, refNoteOnTimestamps) );
        REQUIRE( equalElements(noteOffTimestamps, refNoteOffTimestamps) ); 
    }

    SECTION("Extract 1st bar with 0.0")
    {
        auto refSequence = BeatSequence(1);
        for (auto timestamp: {0.0, 0.1, 0.5, 0.99})
            refSequence.addEvent({noteNumber, channel, velocity, timestamp, noteDuration});
        
        auto [refNoteOnTimestamps, refNoteOffTimestamps] = extractTimestamps(refSequence);
        
        auto testBar = sequence.getBarAtTime(0.0);
        auto [noteOnTimestamps, noteOffTimestamps] = extractTimestamps(*testBar);
        REQUIRE( equalElements(noteOnTimestamps, refNoteOnTimestamps) );
        REQUIRE( equalElements(noteOffTimestamps, refNoteOffTimestamps) ); 
    }

    SECTION("Extract 1st bar with 0.999")
    {
        auto refSequence = BeatSequence(1);
        for (auto timestamp: {0.0, 0.1, 0.5, 0.99})
            refSequence.addEvent({noteNumber, channel, velocity, timestamp, noteDuration});
        
        auto [refNoteOnTimestamps, refNoteOffTimestamps] = extractTimestamps(refSequence);        
        auto testBar = sequence.getBarAtTime(0.999);
        auto [noteOnTimestamps, noteOffTimestamps] = extractTimestamps(*testBar);
        REQUIRE( equalElements(noteOnTimestamps, refNoteOnTimestamps) );
        REQUIRE( equalElements(noteOffTimestamps, refNoteOffTimestamps) ); 
    }

    SECTION("Extract 2nd bar with 1.0")
    {
        auto refSequence = BeatSequence(1);
        for (auto timestamp: {1.0, 1.01,  1.5, 1.999})
            refSequence.addEvent({noteNumber, channel, velocity, timestamp, noteDuration});
        
        auto [refNoteOnTimestamps, refNoteOffTimestamps] = extractTimestamps(refSequence);        
        auto testBar = sequence.getBarAtTime(1.2);
        auto [noteOnTimestamps, noteOffTimestamps] = extractTimestamps(*testBar);
        REQUIRE( equalElements(noteOnTimestamps, refNoteOnTimestamps) );
        REQUIRE( equalElements(noteOffTimestamps, refNoteOffTimestamps) ); 
    }

    SECTION("Extract all when there are 4 quarters per bar")
    {
        sequence.setQuartersPerBar(4);
        auto [refNoteOnTimestamps, refNoteOffTimestamps] = extractTimestamps(sequence);        
        auto testBar = sequence.getBarAtTime(1.0);
        auto [noteOnTimestamps, noteOffTimestamps] = extractTimestamps(*testBar);
        REQUIRE( equalElements(noteOnTimestamps, refNoteOnTimestamps) );
        REQUIRE( equalElements(noteOffTimestamps, refNoteOffTimestamps) ); 
    }
}

TEST_CASE("Build from Midi file", "[BeatSequence]")
{
    const double noteDuration = 0.1;
    const int noteNumber = 1;
    const int channel = 1;
    const uint8 velocity = 63;
    const int ticksPerQuarterNote = 1000;

    SECTION("Empty midi file")
    {
        MidiFile midiFile;
        midiFile.setTicksPerQuarterNote(ticksPerQuarterNote);
        BeatSequence sequence(midiFile);
        REQUIRE( sequence.getQuartersPerBar() == 4 );
        REQUIRE( sequence.getNumEvents() == 0 ); 
    }


    SECTION("Basic file")
    {
        MidiFile midiFile;
        midiFile.setTicksPerQuarterNote(ticksPerQuarterNote);
        MidiMessageSequence track;
        std::vector<double> refNoteOnTimestamps = {0.0, 0.1, 0.5, 0.99, 1.0, 1.2,  1.5, 1.999, 2.0, 2.5, 3.1, 3.7};
        std::vector<double> refNoteOffTimestamps = refNoteOnTimestamps;
        std::for_each(refNoteOffTimestamps.begin(), refNoteOffTimestamps.end(), [noteDuration](double& x){ x += noteDuration; });

        for (auto timestamp: refNoteOnTimestamps)
        {
            const double noteOnTicks = std::round(timestamp * ticksPerQuarterNote);
            const double noteOffTicks = std::round((timestamp + noteDuration) * ticksPerQuarterNote);
            track.addEvent(MidiMessage::noteOn(channel, noteNumber, velocity), noteOnTicks);
            track.addEvent(MidiMessage::noteOff(channel, noteNumber), noteOffTicks);
        }
        midiFile.addTrack(track);
        BeatSequence sequence { midiFile };

        auto [noteOnTimestamps, noteOffTimestamps] = extractTimestamps(sequence);
        REQUIRE( equalElements(noteOnTimestamps, refNoteOnTimestamps) );
        REQUIRE( equalElements(noteOffTimestamps, refNoteOffTimestamps) );
    }

    SECTION("numberOfBars and ignoreBars")
    {
        MidiFile midiFile;
        midiFile.setTicksPerQuarterNote(ticksPerQuarterNote);
        MidiMessageSequence track;
        std::vector<double> insertedNoteOnTimestamps = {0.0, 0.1, 0.5, 0.99, 1.0, 1.2,  1.5, 1.999, 2.0, 2.5, 3.1, 3.7};
        std::vector<double> insertedNoteOffTimestamps = insertedNoteOnTimestamps;

        std::for_each(insertedNoteOffTimestamps.begin(), insertedNoteOffTimestamps.end(), [noteDuration](double& x){ x += noteDuration; });

        for (auto timestamp: insertedNoteOnTimestamps)
        {
            const double noteOnTicks = std::round(timestamp * ticksPerQuarterNote);
            const double noteOffTicks = std::round((timestamp + noteDuration) * ticksPerQuarterNote);
            track.addEvent(MidiMessage::noteOn(channel, noteNumber, velocity), noteOnTicks);
            track.addEvent(MidiMessage::noteOff(channel, noteNumber), noteOffTicks);
        }
        midiFile.addTrack(track);

        std::vector<double> refNoteOnTimestamps = {0.99, 1.0, 1.2,  1.5}; // keeps the 0.99 but ignores the 1.999
        std::vector<double> refNoteOffTimestamps = refNoteOnTimestamps;
        std::for_each(refNoteOffTimestamps.begin(), refNoteOffTimestamps.end(), [noteDuration](double& x){ x += noteDuration; });

        BeatSequence sequence(midiFile, 1, 1);

        auto [noteOnTimestamps, noteOffTimestamps] = extractTimestamps(sequence);
        REQUIRE( equalElements(noteOnTimestamps, refNoteOnTimestamps) );
        REQUIRE( equalElements(noteOffTimestamps, refNoteOffTimestamps) );
    }
}

TEST_CASE("Use note replacements", "[BeatSequence]")
{
    const double noteDuration = 0.1;
    const int channel = 1;
    const uint8 velocity = 63;
    const int ticksPerQuarterNote = 240;
    std::vector<double> refNoteOnTimestamps {0.0, 0.1, 0.5, 0.99, 1.0, 1.2, 1.5, 1.999, 2.0, 2.5, 3.1, 3.7};
    std::vector<double> refNoteOffTimestamps = refNoteOnTimestamps;
    std::for_each(refNoteOffTimestamps.begin(), refNoteOffTimestamps.end(), [noteDuration](double& x){ x += noteDuration; });
    std::vector<int> refNoteNumbers {30, 31, 32, 33, 34, 35, 30, 31, 32, 33, 34, 35};

    SECTION("Replace nothing (check that notes are set correctly)")
    {
        BeatSequence sequence;
        for (int noteIdx = 0; noteIdx < refNoteOnTimestamps.size() && noteIdx < refNoteNumbers.size(); ++noteIdx)
        {
            sequence.addEvent({ refNoteNumbers[noteIdx], channel, velocity, refNoteOnTimestamps[noteIdx], noteDuration });
        }
        auto [noteOnTimestamps, noteOffTimestamps] = extractTimestamps(sequence);
        auto noteNumbers = extractNotes(sequence);
        REQUIRE( equalElements(noteOnTimestamps, refNoteOnTimestamps) );
        REQUIRE( equalElements(noteOffTimestamps, refNoteOffTimestamps) );
        REQUIRE( equalElements(noteNumbers, refNoteNumbers) );
    }

    SECTION("Replace an inexistant note")
    {
        BeatSequence sequence;
        for (int noteIdx = 0; noteIdx < refNoteOnTimestamps.size() && noteIdx < refNoteNumbers.size(); ++noteIdx)
        {
            sequence.addEvent({ refNoteNumbers[noteIdx], channel, velocity, refNoteOnTimestamps[noteIdx], noteDuration });
        }
        std::vector<std::pair<int, int>> noteReplacements { {20, 40} };
        for (auto& replacement: noteReplacements)
        {
            sequence.replaceNote(replacement.first, replacement.second);
        }
        auto [noteOnTimestamps, noteOffTimestamps] = extractTimestamps(sequence);
        auto noteNumbers = extractNotes(sequence);
        REQUIRE( equalElements(noteOnTimestamps, refNoteOnTimestamps) );
        REQUIRE( equalElements(noteOffTimestamps, refNoteOffTimestamps) );
        REQUIRE( equalElements(noteNumbers, refNoteNumbers) );
    }

    SECTION("Replace 1 note")
    {
        BeatSequence sequence;
        for (int noteIdx = 0; noteIdx < refNoteOnTimestamps.size() && noteIdx < refNoteNumbers.size(); ++noteIdx)
        {
            sequence.addEvent({ refNoteNumbers[noteIdx], channel, velocity, refNoteOnTimestamps[noteIdx], noteDuration });
        }
        std::vector<std::pair<int, int>> noteReplacements { {30, 40} };
        for (auto& replacement: noteReplacements)
        {
            sequence.replaceNote(replacement.first, replacement.second);
        }
        std::vector<int> refReplacedNoteNumbers {40, 31, 32, 33, 34, 35, 40, 31, 32, 33, 34, 35};
        auto [noteOnTimestamps, noteOffTimestamps] = extractTimestamps(sequence);
        auto noteNumbers = extractNotes(sequence);
        REQUIRE( equalElements(noteOnTimestamps, refNoteOnTimestamps) );
        REQUIRE( equalElements(noteOffTimestamps, refNoteOffTimestamps) );
        REQUIRE( equalElements(noteNumbers, refReplacedNoteNumbers) );
    }

    SECTION("Replace 2 notes")
    {
        BeatSequence sequence;
        for (int noteIdx = 0; noteIdx < refNoteOnTimestamps.size() && noteIdx < refNoteNumbers.size(); ++noteIdx)
        {
            sequence.addEvent({ refNoteNumbers[noteIdx], channel, velocity, refNoteOnTimestamps[noteIdx], noteDuration });
        }
        std::vector<std::pair<int, int>> noteReplacements { {30, 40}, {33, 43} };
        for (auto& replacement: noteReplacements)
        {
            sequence.replaceNote(replacement.first, replacement.second);
        }
        std::vector<int> refReplacedNoteNumbers {40, 31, 32, 43, 34, 35, 40, 31, 32, 43, 34, 35};
        auto [noteOnTimestamps, noteOffTimestamps] = extractTimestamps(sequence);
        auto noteNumbers = extractNotes(sequence);
        REQUIRE( equalElements(noteOnTimestamps, refNoteOnTimestamps) );
        REQUIRE( equalElements(noteOffTimestamps, refNoteOffTimestamps) );
        REQUIRE( equalElements(noteNumbers, refReplacedNoteNumbers) );
    }
}

TEST_CASE("12-8 Blues bug", "[BeatSequence]")
{
    const auto file = File("TestFiles/12-8 Slow Hat F1.mid");
    REQUIRE( file.exists() );

    auto inputStream = FileInputStream(file);
    REQUIRE( inputStream.openedOk() );

    MidiFile midiFile;
    midiFile.readFrom(inputStream);
    const auto sequence = BeatSequence(midiFile, 1, 11);

    REQUIRE( sequence.getQuartersPerBar() == 6 );
    REQUIRE( sequence.getLastBarTime() == 6.0 );
}