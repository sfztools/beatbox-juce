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

#include "BeatSequence.h"
#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h"

std::shared_ptr<BeatSequence> BeatSequence::getEventsWithin(double startTime, double endTime) const
{
    auto startIt = std::find_if(sequence.cbegin(), sequence.cend(), [startTime](const MidiNote& note) { return note.getTimeStamp() >= startTime; });
    auto endIt = std::find_if(sequence.cbegin(), sequence.cend(), [endTime](const MidiNote& note) { return note.getTimeStamp() >= endTime; });

    return std::make_shared<BeatSequence>(MidiNoteMultiset(startIt, endIt), quartersPerBar);
}

std::shared_ptr<BeatSequence> BeatSequence::getBarAtTime(double time) const
{
    const int barIndex = static_cast<int>(time);
    const double barStart = barIndex - (barIndex % quartersPerBar);
    const double barEnd = barStart + quartersPerBar;
    return getEventsWithin(barStart, barEnd);
}

void BeatSequence::addEvent(MidiNote&& message)
{
    sequence.emplace(message);
}
void BeatSequence::addEvent(const MidiNote& message)
{
    sequence.emplace(message);
}

double BeatSequence::getLastBarTime() const noexcept
{
    if (sequence.empty())
        return 0.0;

    return static_cast<double>(std::ceil(sequence.rbegin()->getTimeStamp() / quartersPerBar) * quartersPerBar);
}

/**
 * Extracts the number of quarters per bar from a Juce Midi file
 */
int extractFileQuarterPerBars(const MidiFile& midiFile)
{
    // Look for time signatures
    MidiMessageSequence timeSigEvents;
    midiFile.findAllTimeSigEvents(timeSigEvents);
    if (timeSigEvents.getNumEvents() > 0)
    {
        int num;
        int denom;
        timeSigEvents.getEventPointer(0)->message.getTimeSignatureInfo(num, denom);
        if (denom == 4)
            return num;
        else if (denom == 8)
            return static_cast<int>(num / 2);
        spdlog::debug("Weird time signature {}/{}; ignoring...", num, denom);
    }

    return 4;
};

BeatSequence::BeatSequence(const MidiFile& midiFile, int numberOfBars, int ignoreBars)
{
    const auto ticksPerQuarter = midiFile.getTimeFormat();
    if (ticksPerQuarter < 0)
    {
        spdlog::error("We don't handle SMTPE time scale yet");
        return;
    }

    quartersPerBar = extractFileQuarterPerBars(midiFile);

    const auto fileNumberOfBars = static_cast<int>(std::ceil(midiFile.getLastTimestamp() / ticksPerQuarter / quartersPerBar));
    if (fileNumberOfBars == 0)
    {
        spdlog::error("Empty MIDI file \"{}\"");
        return;
    }
    if (numberOfBars == 0)
        numberOfBars = fileNumberOfBars - ignoreBars;

    const auto deltaTime = ignoreBars * ticksPerQuarter * quartersPerBar;
    for (auto trackIdx = 0; trackIdx < midiFile.getNumTracks(); ++trackIdx)
    {
        auto track = midiFile.getTrack(trackIdx);
        // Check notes before the start of the sequence
        // We check 1 bar before the ignore, in case there are some off-beat note going on
        if (ignoreBars > 0)
        {
            auto currentIdx = track->getNextIndexAtTime(deltaTime - ticksPerQuarter);
            while (currentIdx < track->getNumEvents() && track->getEventTime(currentIdx) < deltaTime)
            {
                const auto midiEvent = track->getEventPointer(currentIdx);
                if (track->getTimeOfMatchingKeyUp(currentIdx) > deltaTime)
                {
                    // We have to map it to timestamp 0 with the proper duration
                    const auto noteOnTime = midiEvent->message.getTimeStamp();
                    const auto noteOffTime = midiEvent->noteOffObject != nullptr ? midiEvent->noteOffObject->message.getTimeStamp() : noteOnTime + 1.0 / 24.0;
                    // Check that we are more in that out
                    if (deltaTime - noteOnTime < noteOffTime - deltaTime)
                    {
                        const auto noteDuration = midiEvent->message.getTimeStamp() - noteOffTime;
                        const auto normalizedNoteDuration = (noteOffTime - noteOnTime) / ticksPerQuarter;
                        addEvent({ midiEvent->message.withTimeStamp(0), normalizedNoteDuration });
                    }
                }
                currentIdx++;
            }
        }

        // Check notes within the sequence
        auto currentIdx = track->getNextIndexAtTime(deltaTime);
        const double endTime = deltaTime + numberOfBars * ticksPerQuarter * quartersPerBar;
        while (currentIdx < track->getNumEvents() && track->getEventTime(currentIdx) < endTime)
        {
            auto midiEvent = track->getEventPointer(currentIdx);
            if (midiEvent->message.isNoteOn())
            {
                const auto noteOnTime = midiEvent->message.getTimeStamp();
                const auto noteOffTime = midiEvent->noteOffObject != nullptr ? midiEvent->noteOffObject->message.getTimeStamp() : noteOnTime + 1.0/24.0;
                // Check that we are more in that out
                if ((noteOffTime < endTime) || (endTime - noteOnTime > noteOffTime - endTime))
                {
                    const auto normalizedNoteOnTimestamp = (noteOnTime - deltaTime) / ticksPerQuarter;
                    const auto normalizedNoteDuration = (noteOffTime - noteOnTime) / ticksPerQuarter;
                    addEvent({ midiEvent->message.withTimeStamp(normalizedNoteOnTimestamp), normalizedNoteDuration });
                }
            }
            currentIdx++;
        }
    } // Loop on tracks
}

void BeatSequence::replaceNote(int from, int to)
{
    auto noteIterator = sequence.cbegin();
    while (noteIterator != sequence.cend())
    {
        if (noteIterator->getNoteNumber() == from)
        {
            sequence.emplace(to, noteIterator->getChannel(), noteIterator->getVelocity(), noteIterator->getTimeStamp(), noteIterator->getNoteOffDelay());
            noteIterator = sequence.erase(noteIterator);
        }
        else
        {
            noteIterator++;
        }
    }
}