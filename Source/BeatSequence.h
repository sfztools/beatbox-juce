/*
  ==============================================================================

  Copyright 2019 - Paul Ferrand (paulfd@outlook.fr)

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <https://www.gnu.org/licenses/>.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include <algorithm>
#include <set>
#include <cmath>

class MidiNote
{
public:
    MidiNote(int noteNumber, int channel, uint8 velocity, double timestamp, double noteOffDelay)
    : noteNumber { noteNumber },
        channel { channel }, 
        velocity {velocity},
        timestamp { timestamp },
        noteOffDelay { noteOffDelay }
    {
        keepInvariants();
    }
    MidiNote(const MidiMessage& message, double noteOffDelay) 
    : noteNumber { message.isNoteOnOrOff() ? message.getNoteNumber() : 0 },
        velocity { message.getVelocity() },
        channel{ message.getChannel() },
        timestamp { message.getTimeStamp() },
        noteOffDelay { noteOffDelay } 
    {
        keepInvariants();
    }
    MidiNote(const MidiMessage& noteOn, const MidiMessage& noteOff) 
    : MidiNote(noteOn, noteOff.getTimeStamp() - noteOn.getTimeStamp())
    {
        keepInvariants();
    }

    double getTimeStamp() const noexcept { return timestamp; }
    double getNoteOffTimeStamp() const noexcept { return timestamp + noteOffDelay; }
    double getNoteOffDelay() const noexcept { return noteOffDelay; }
    int getNoteNumber() const noexcept { return noteNumber; }
    int getChannel() const noexcept { return channel; }
    uint8 getVelocity() const noexcept { return velocity; }
    static constexpr double defaultNoteOffDelay{ 10/240 };
private:
    void keepInvariants()
    {
        noteNumber = std::max(0, noteNumber);
        noteNumber = std::min(127, noteNumber);
        channel = std::max(1, channel);
        channel = std::min(16, channel);
        velocity = std::min(juce::uint8(127), velocity);
        timestamp = std::max(0.0, timestamp);
        if (noteOffDelay < 0.0)
            noteOffDelay = defaultNoteOffDelay;
    }
    // The types are Juce's
    int noteNumber{ 0 };
    int channel{ 1 };
    uint8 velocity{ 63 };
    double timestamp{ 0 };
    double noteOffDelay{ defaultNoteOffDelay };
    JUCE_LEAK_DETECTOR(MidiNote);
};

struct TimestampComparison {
    bool operator() (const MidiMessage& lhs, const MidiMessage& rhs) const
    {
        return lhs.getTimeStamp() < rhs.getTimeStamp();
    }
    bool operator() (const MidiNote& lhs, const MidiNote& rhs) const
    {
        return lhs.getTimeStamp() < rhs.getTimeStamp();
    }
};

using MidiNoteMultiset = std::multiset<MidiNote, TimestampComparison>;

class BeatSequence
{
public:
    BeatSequence() {}
    BeatSequence(const MidiFile& midiFile, int numberOfBars = 0, int ignoreBars = 0);
    BeatSequence(int quartersPerBar)
    : quartersPerBar{ validateQuartersPerBar(quartersPerBar) } {}
    ~BeatSequence() {}
    BeatSequence(BeatSequence&& beatSequence)
    : quartersPerBar{ validateQuartersPerBar(beatSequence.quartersPerBar) }
    {
        sequence.swap(beatSequence.sequence);
    }
    BeatSequence(MidiNoteMultiset&& seq, int quartersPerBar)
    : quartersPerBar{ validateQuartersPerBar(quartersPerBar) }
    {
        std::swap(sequence, seq);
    }

    void setQuartersPerBar(int newQuartersPerBar) noexcept { quartersPerBar = validateQuartersPerBar(newQuartersPerBar); }
    int getQuartersPerBar() const noexcept { return quartersPerBar; }
    int getRemainingQuartersOnBar(double time) const noexcept { return quartersPerBar - (static_cast<int>(time) % quartersPerBar); }
    auto getNumEvents() const noexcept { return sequence.size(); }

    // Underlying iterators
    auto begin() { return sequence.begin(); }
    auto cbegin() { return sequence.cbegin(); }
    auto end() { return sequence.end(); }
    auto cend() { return sequence.cend(); }

    void addEvent(MidiNote&& message);
    void addEvent(const MidiNote& message);
    void replaceNote(int from, int to);
    double getLastBarTime() const noexcept;

#ifdef DEBUG
    void logSequence()
    {
        DBG("-------------------------------------");
        DBG("| " << " Timestamp " << " | " << "Note" << " | " << "Note off TS" << " |");
        DBG("-------------------------------------");
        for (auto& note : sequence)
        {
            DBG("| " << String(note.getTimeStamp()).paddedRight(' ', 11) << " | "
            << String(note.getNoteNumber()).paddedRight(' ', 4) << " | "
            << String(note.getNoteOffTimeStamp()).paddedRight(' ', 11) << " |");
        }
        DBG("-------------------------------------");
    }
#endif // DEBUG

    // TODO: when the time comes these should both be views
    std::shared_ptr<BeatSequence> getBarAtTime(double time) const;
    std::shared_ptr<BeatSequence> getEventsWithin(double startTime, double endTime) const;
private:
    static int validateQuartersPerBar(int newQuartersPerBar) noexcept { return std::max(1, newQuartersPerBar); }
    MidiNoteMultiset sequence;
    int quartersPerBar{ 4 };
    bool sorted{ true };
    JUCE_LEAK_DETECTOR(BeatSequence);
};