/*
    ==============================================================================

    Copyright 2019 - Paul Ferrand (paulfd@outlook.fr)

    This file was partly auto-generated by Juce.

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

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h"
#include <algorithm>
// #include <range/v3/all.hpp>

//==============================================================================
RhythmBoxAudioProcessor::RhythmBoxAudioProcessor()
    : 	AudioProcessor(BusesProperties().withOutput("Output", AudioChannelSet::stereo(), true))
    ,	parameters(*this, nullptr, Identifier("RhythmBoxParameters"), setUpParameters())
    ,	config(*this)
{
    tempo = parameters.getRawParameterValue ("tempo");
    
    // Parse the default configuration
    config.parseConfiguration();
    
    // sfzero setup
    for(auto i = 0; i < 128; ++i)
        sfzSynth.addVoice(new sfzero::Voice());
    
    formatManager.registerBasicFormats();

    // Set basic vtState properties
    vtState.setProperty(IDs::BeatName, "", nullptr);
    vtState.setProperty(IDs::SfzFile, "", nullptr);
    vtState.setProperty(IDs::PartName, "", nullptr);
    vtState.setProperty(IDs::FillIdx, 0, nullptr);
    vtState.setProperty(IDs::NumFills, 0, nullptr);

    // Preallocate the deferred events
    deferredEvents.reserve(256);

    // Preallocate the poppedSequences
    poppedSequences.reserve(16);
}

RhythmBoxAudioProcessor::~RhythmBoxAudioProcessor()
{
    
}

AudioProcessorValueTreeState::ParameterLayout RhythmBoxAudioProcessor::setUpParameters()
{
    return {
        std::make_unique<AudioParameterFloat>("tempo", "Tempo", 1.0f, 300.0f, defaultTempo)
    };
}

//==============================================================================
void RhythmBoxAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    ignoreUnused(samplesPerBlock);
    // Clear note-off events
    deferredEvents.clear();

    sequences.clear();
    resetPlayHeads();
    samplePeriod = 1.0 / sampleRate;

    sfzSynth.setCurrentPlaybackSampleRate(sampleRate);
}

void RhythmBoxAudioProcessor::sendNoteOffsAndClear()
{
    const auto midiOutputIndex = MidiOutput::getDefaultDeviceIndex();
    const auto midiOutput = std::unique_ptr<MidiOutput>(MidiOutput::openDevice(midiOutputIndex));

    if (midiOutput == nullptr)
        return;

    // Send notes offs to the default channel
    for (auto& evt : deferredEvents)
    {
        if (evt.isNoteOff())
            midiOutput->sendMessageNow(evt);
    }
    deferredEvents.clear();

    // Clear the sequences
    // const ScopedSuspender suspender { *this };
    const ScopedLock lock { getCallbackLock() };
    sequences.clear();
}

void RhythmBoxAudioProcessor::releaseResources()
{
    sendNoteOffsAndClear();
}

void RhythmBoxAudioProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
    ScopedNoDenormals noDenormals;
    // Clear all outputs
    const auto totalNumOutputChannels = getTotalNumOutputChannels();
    for (auto i = 0; i < totalNumOutputChannels; ++i)
            buffer.clear (i, 0, buffer.getNumSamples());

    // Relevant constants for this block
    const auto numSamples = buffer.getNumSamples();
    const auto blockTempo = getTempoParameter(); // This can change from block to block
    const double secondsPerQuarter { 60.0 / blockTempo };
    const double normalizedSamplePeriod {samplePeriod / secondsPerQuarter};

    processInputMidiEvents(midiMessages, normalizedSamplePeriod);
    midiMessages.clear();

    // Return if we're not playing: we're done
    if (state == PluginState::Stopped || sequences.empty())
    {
        midiMessages.addEvent(MidiMessage::allNotesOff(10), 0);
        sfzSynth.renderNextBlock(buffer, midiMessages, 0, numSamples);
        return;
    }

    const double deltaTime { numSamples * normalizedSamplePeriod };
    const double endTime { playTimeHead + deltaTime };
    const auto outputMidiChannel = config.getOutputChannel();
    
    auto remainingTime = deltaTime;
    while (remainingTime > normalizedSamplePeriod)
    {
        const auto currentSequenceDuration = sequences.front()->getLastBarTime();

        // If we have "changed" the sequence from a larger one (i.e. fill in) we have to adapt the sequence play head
        // So we wrap it into the current sequence duration
        sequencePlayTimeHead = std::fmod(sequencePlayTimeHead, currentSequenceDuration);
        const double chunkEnd = std::min(currentSequenceDuration, sequencePlayTimeHead + remainingTime);
        const double sequenceStartTime { playTimeHead - sequencePlayTimeHead }; // in absolute playtime

        // Get notes from this chunk and insert them in the deferred events
        auto midiNotes = sequences.front()->getEventsWithin(sequencePlayTimeHead, chunkEnd);
        for (auto& note : *midiNotes)
        {
            const double noteOnTime { note.getTimeStamp() + sequenceStartTime }; // in absolute playtime
            const double noteOffTime { note.getNoteOffTimeStamp() + sequenceStartTime }; // in absolute playtime
            deferNoteOn(note.getNoteNumber(), note.getVelocity(), noteOnTime);
            deferNoteOff(note.getNoteNumber(), noteOffTime);
        }

        // Update the remaining time and the sequence play head
        remainingTime -= chunkEnd - sequencePlayTimeHead;
        sequencePlayTimeHead = chunkEnd;

        // We reached the end of a sequence
        if (currentSequenceDuration - sequencePlayTimeHead < normalizedSamplePeriod)
        {
            sequencePlayTimeHead = 0.0;

            // Ending condition
            if (state == PluginState::Ending && sequences.size() == 1)
            {
                stopPlaying(); // This will clear the sequences
                remainingTime = 0.0;
            }
            // If we have a queue of sequences we go to the next one
            else if (sequences.size() > 1)
            {
                poppedSequences.emplace_back(sequences.front());
                sequences.pop_front();
            }
            // If we emptied the sequences (except the last) while in a transition/fillin state, we're back in the normal state
            if ((state == PluginState::FillIn || state == PluginState::Transition) && sequences.size() == 1)
            {
                setState(PluginState::Playing);
            }
        }
    }
    processDeferredEvents(midiMessages, endTime, normalizedSamplePeriod);
    sfzSynth.renderNextBlock(buffer, midiMessages, 0, numSamples);
}

void RhythmBoxAudioProcessor::deferNoteOn(int noteNumber, uint8 velocity, double timestamp)
{
    if (noteNumber < 0 || noteNumber > 127)
        return;
    if (velocity > 127)
        return;
    if (timestamp < playTimeHead)
        return;

    const auto outputMidiChannel = config.getOutputChannel();
    deferredEvents.emplace_back(MidiMessage::noteOn(outputMidiChannel, noteNumber, velocity).withTimeStamp(timestamp));
}

void RhythmBoxAudioProcessor::deferNoteOff(int noteNumber, double timestamp)
{
    if (noteNumber < 0 || noteNumber > 127)
        return;
    if (timestamp < playTimeHead)
        return;

    const auto outputMidiChannel = config.getOutputChannel();
    deferredEvents.emplace_back(MidiMessage::noteOff(outputMidiChannel, noteNumber).withTimeStamp(timestamp));
}

void RhythmBoxAudioProcessor::processInputMidiEvents(MidiBuffer& midiMessages, double normalizedSamplePeriod)
{
    auto midiIterator = MidiBuffer::Iterator(midiMessages);
    MidiMessage evt;
    int position;
    while (midiIterator.getNextEvent(evt, position))
    {
        if (evt.isController() && evt.getChannel() == config.getInputChannel())
        {
            const bool controllerStatus { evt.getControllerValue() > 0 };
            // Main switch events
            if (evt.getControllerNumber() == config.getMainCC())
            {
                const int64 switchTime { Time::currentTimeMillis() + static_cast<int64>(position * samplePeriod) * 1000 };
                if (controllerStatus)
                    mainSwitchDown(switchTime);
                else
                    mainSwitchUp(switchTime);
            }

            // Accent switch events
            if (controllerStatus && evt.getControllerNumber() == config.getAccentCC() && state != PluginState::Stopped)
            {
                const double accentTime { position * normalizedSamplePeriod + playTimeHead };
                deferNoteOn(49, 100, accentTime);
                deferNoteOff(49, accentTime + config.getAccentDuration());
            }
        }
    }
}

void RhythmBoxAudioProcessor::processDeferredEvents(MidiBuffer& midiMessages, double endTime, double normalizedSamplePeriod)
{
    // Sort the event vector
    std::sort(deferredEvents.begin(), deferredEvents.end(), TimestampComparison());

    // Check the deferred events
    auto eventIterator = deferredEvents.begin();
    while (eventIterator != deferredEvents.end() && eventIterator->getTimeStamp() < endTime)
    {
        const auto eventTimeStamp = eventIterator->getTimeStamp();
        const int eventBufferPosition { static_cast<int>((eventTimeStamp - playTimeHead) / normalizedSamplePeriod) };
        jassert(eventBufferPosition >= 0);
        midiMessages.addEvent(*eventIterator, eventBufferPosition);
        eventIterator = deferredEvents.erase(eventIterator);
    }

    // We consumed the events: advance the global play head
    playTimeHead = endTime;
}

void RhythmBoxAudioProcessor::setBeatDescription(BeatDescription* newDescription)
{
    description = newDescription;
    if (description != nullptr)
    {
        parameters.getParameterAsValue("tempo").setValue(60.0 / description->getSecondsPerQuarter());
        vtState.setProperty(IDs::BeatName, description->getBeatName(), nullptr);
        vtState.setProperty(IDs::PartName, description->getCurrentPartName(), nullptr);
        vtState.setProperty(IDs::NumFills, description->getNumberOfFills(), nullptr);
    }
    stopPlaying(); // Reset everything and setup the intro
}

void RhythmBoxAudioProcessor::loadSfzPatch(const File& sfzFile)
{
    sfzSynth.clearSounds();

    if (!sfzFile.exists())
    {
        vtState.setProperty(IDs::SfzFile, "MIDI output", nullptr);
        spdlog::error("File \"{}\" does not exist", sfzFile.getFileName());
        return; 
    }

    auto sound = new sfzero::Sound(sfzFile);
    sound->loadRegions();
    sound->loadSamples(formatManager);
    sfzSynth.addSound(sound);
    vtState.setProperty(IDs::SfzFile, sfzFile.getFileNameWithoutExtension(), nullptr);
}

void RhythmBoxAudioProcessor::startPlaying()
{
    setState(PluginState::Playing);
}

void RhythmBoxAudioProcessor::pausePlaying()
{
    setState(PluginState::Stopped);
}

void RhythmBoxAudioProcessor::stopPlaying()
{
    // const ScopedSuspender suspender { *this };
    const ScopedLock lock { getCallbackLock() };
    resetPlayHeads();
    sequences.clear();
    setState(PluginState::Stopped);

    if (description == nullptr)
        return;
     
    description->resetCounters();
    pushSequenceBack(description->getIntro());
    pushSequenceBack(description->getCurrentPart());
}

void RhythmBoxAudioProcessor::fillIn()
{
    if (description == nullptr)
        return;

    if (sequences.empty())
        return;

    if (state != PluginState::Playing)
        return;
    
    // const ScopedLock lock{ sequencesLock };
    if (sequences.front()->getRemainingQuartersOnBar(sequencePlayTimeHead) <= 1)
    {
        const auto barToFinish = sequences.front()->getBarAtTime(sequencePlayTimeHead);
        // const ScopedSuspender suspender { *this };
        const ScopedLock lock { getCallbackLock() };
        if (config.useRandomFills())
            pushSequenceFront(description->getRandomFill());
        else
            pushSequenceFront(description->getFill());
        
        pushSequenceFront(barToFinish);
    }
    else
    {
        // const ScopedSuspender suspender { *this };
        const ScopedLock lock { getCallbackLock() };
        if (config.useRandomFills())
            pushSequenceFront(description->getRandomFill());
        else
            pushSequenceFront(description->getFill());
    }
    setState(PluginState::FillIn);
}

void RhythmBoxAudioProcessor::nextPart()
{
    if (description == nullptr)
        return;

    if (sequences.empty())
        return;

    if (state != PluginState::Playing)
        return;

    if (sequences.front()->getRemainingQuartersOnBar(sequencePlayTimeHead) <= 1)
    {
        const auto barToFinish = sequences.front()->getBarAtTime(sequencePlayTimeHead);
        // const ScopedSuspender suspender { *this };
        const ScopedLock lock { getCallbackLock() };
        sequences.clear();
        pushSequenceFront(description->getTransition());
        pushSequenceBack(description->getNextPart());
        pushSequenceFront(barToFinish);
    }
    else
    {
        // const ScopedSuspender suspender { *this };
        const ScopedLock lock { getCallbackLock() };
        sequences.clear();
        pushSequenceFront(description->getTransition());
        pushSequenceBack(description->getNextPart());
    }
    setState(PluginState::Transition);
}

void RhythmBoxAudioProcessor::ending()
{
    if (description == nullptr)
        return;

    if (sequences.empty())
        return;

    if (state == PluginState::Stopped)
        return;

    // const ScopedSuspender suspender { *this };
    const ScopedLock lock { getCallbackLock() };
    const auto barToFinish = sequences.front()->getBarAtTime(sequencePlayTimeHead);
    sequences.clear();
    pushSequenceFront(barToFinish);
    pushSequenceBack(description->getEnding());
    setState(PluginState::Ending);
}

void RhythmBoxAudioProcessor::setState(PluginState newState)
{
    state = newState;
    triggerAsyncUpdate(); // Update the state values
}

String RhythmBoxAudioProcessor::getCurrentStatusString()
{
    if (description == nullptr)
        return { "--" };

    auto returnedString = description->getBeatName();
    switch (state)
    {
    case PluginState::Playing:
        returnedString << " / " << description->getCurrentPartName();
        break;
    case PluginState::FillIn:
        returnedString << " / " << description->getCurrentPartName()
            << " (Fill " << description->getCurrentFillIndex() << "/" << description->getNumberOfFills() << ")";
        break;
    case PluginState::Transition:
        returnedString << " -> " << description->getCurrentPartName();
        break;
    }
    return returnedString;
}

void RhythmBoxAudioProcessor::mainSwitchDown(int64 switchTime)
{
    if (state == PluginState::FillIn && switchTime - lastSwitchUpTime < 500)
        ending();
    lastSwitchUpTime = switchTime;
}

void RhythmBoxAudioProcessor::mainSwitchUp(int64 switchTime)
{
    lastSwitchDownTime = switchTime;
    switch (state)
    {
    case PluginState::Playing:
        if (lastSwitchDownTime - lastSwitchUpTime < 200)
            fillIn();
        else
            nextPart();
        break;

    case PluginState::Stopped:
        startPlaying();
        break;

    default:
        break;
    }
}

void RhythmBoxAudioProcessor::pushSequenceFront(const std::shared_ptr<BeatSequence>& newSequence)
{
    if (newSequence == nullptr)
        return;
    if (newSequence->getNumEvents() == 0)
        return;

    sequences.push_front(newSequence);
}

void RhythmBoxAudioProcessor::pushSequenceBack(const std::shared_ptr<BeatSequence>& newSequence)
{
    if (newSequence == nullptr)
        return;
    if (newSequence->getNumEvents() == 0)
        return;

    sequences.push_back(newSequence);
}

void RhythmBoxAudioProcessor::resetPlayHeads()
{
    sequencePlayTimeHead = 0.0;
    playTimeHead = 0.0;
}

void RhythmBoxAudioProcessor::handleAsyncUpdate()
{
    // Garbage collect the popped sequences
    poppedSequences.clear();

    // Update the valueTree with the current state
    if (description == nullptr)
        return;
     
    vtState.setProperty(IDs::PartName, description->getCurrentPartName(), nullptr);
    vtState.setProperty(IDs::NumFills, description->getNumberOfFills(), nullptr);
    vtState.setProperty(IDs::FillIdx, description->getCurrentFillIndex(), nullptr);
    vtState.sendPropertyChangeMessage(IDs::PartName);
}

// JUCE boilerplate stuff ==============================================================
AudioProcessorEditor* RhythmBoxAudioProcessor::createEditor()
{
    return new RhythmBoxAudioProcessorEditor(*this, this->parameters, this->config);
}

void RhythmBoxAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // auto parameterStates = parameters.copyState();
    // std::unique_ptr<XmlElement> xml (parameterStates.createXml());
    // copyXmlToBinary (*xml, destData);
    ignoreUnused(destData);
}

void RhythmBoxAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // std::unique_ptr<XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));
    // if (xmlState.get() != nullptr)
    //   if (xmlState->hasTagName (parameters.state.getType()))
    //     parameters.replaceState (ValueTree::fromXml (*xmlState));
    ignoreUnused(sizeInBytes);
    ignoreUnused(data);

    config.saveConfiguration();
}

// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new RhythmBoxAudioProcessor();
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool RhythmBoxAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
#if JucePlugin_IsMidiEffect
    ignoreUnused(layouts);
    return true;
#else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono()
        && layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
#if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
#endif

    return true;
#endif
}
#endif

//==============================================================================

