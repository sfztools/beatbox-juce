#include "../JuceLibraryCode/JuceHeader.h"
#include "catch2/catch.hpp"
#include "../Source/BeatSequence.h"
using namespace Catch::literals;

TEST_CASE("MidiNote creation", "[MidiNote]")
{
    SECTION("Creating from scratch")
    {
        auto note = MidiNote(23, 1, 123, 1.0, 0.1);
        REQUIRE(note.getTimeStamp() == 1.0); 
        REQUIRE(note.getNoteOffTimeStamp() == 1.1);        
        REQUIRE(note.getNoteNumber() == 23);        
        REQUIRE(note.getChannel() == 1);        
        REQUIRE(note.getVelocity() == 123);        
    }

    SECTION("Creating from a single MidiMessage")
    {
        auto note = MidiNote(MidiMessage::noteOn(1, 127, juce::uint8(127)), 0.1);
        REQUIRE(note.getTimeStamp() == 0.0); 
        REQUIRE(note.getNoteOffTimeStamp() == 0.1);        
        REQUIRE(note.getChannel() == 1);        
        REQUIRE(note.getNoteNumber() == 127);        
        REQUIRE(note.getVelocity() == 127);

        note = MidiNote(MidiMessage::noteOn(1, 127, juce::uint8(127)).withTimeStamp(1.0), 0.1);
        REQUIRE(note.getTimeStamp() == 1.0); 
        REQUIRE(note.getNoteOffTimeStamp() == 1.1_a);
        
        note = MidiNote(MidiMessage::channelPressureChange(1, 127).withTimeStamp(1.0), 0.1);
        REQUIRE(note.getNoteNumber() == 0);        
    }

    SECTION("Creating from a 2 MidiMessages")
    {
        auto note = MidiNote(MidiMessage::noteOn(1, 127, juce::uint8(127)), MidiMessage::noteOff(1, 127));
        REQUIRE(note.getTimeStamp() == 0.0); 
        REQUIRE(note.getNoteOffTimeStamp() == 0.0_a);        
        REQUIRE(note.getChannel() == 1);        
        REQUIRE(note.getNoteNumber() == 127);        
        REQUIRE(note.getVelocity() == 127);

        note = MidiNote(MidiMessage::noteOn(1, 127, juce::uint8(127)).withTimeStamp(1.0), MidiMessage::noteOff(1, 127).withTimeStamp(1.1));
        REQUIRE(note.getTimeStamp() == 1.0); 
        REQUIRE(note.getNoteOffTimeStamp() == 1.1_a);
    }

    SECTION("Invariants")
    {
        auto note = MidiNote(128, 18, 251, 1.0, 0.1);
        REQUIRE(note.getNoteNumber() == 127); 
        REQUIRE(note.getChannel() == 16); 
        REQUIRE(note.getVelocity() == 127); 
        
        note = MidiNote(-1, -1, 127, -1.0, -0.1);
        REQUIRE(note.getNoteNumber() == 0); 
        REQUIRE(note.getChannel() == 1);
        REQUIRE(note.getTimeStamp() == 0.0);
        REQUIRE(note.getNoteOffTimeStamp() == MidiNote::defaultNoteOffDelay);

        note = MidiNote(0, 1, 127, 1.0, -0.1);
        REQUIRE(note.getNoteOffTimeStamp() == Approx(1.0 + MidiNote::defaultNoteOffDelay));
    }
}