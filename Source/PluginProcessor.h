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

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "BeatDescription.h"
#include "BeatSequence.h"
#include "Configuration.h"
#include "SFZero.h"
#include <deque>
#include <set>

// State value tree
namespace IDs
{
	inline static Identifier SfzFile { "SfzFile" };
	inline static Identifier BeatName { "BeatName" };
	inline static Identifier PartName { "PartName" };
	inline static Identifier FillIdx { "FillIdx" };
	inline static Identifier NumFills { "NumFills" };
	inline static Identifier CurrentState { "CurrentState" };
}

struct ScopedSuspender
{
public:
	ScopedSuspender() = delete;
	ScopedSuspender(AudioProcessor& processor) : processor(processor) { processor.suspendProcessing(true); }
	~ScopedSuspender() { processor.suspendProcessing(false); }
private:
	AudioProcessor& processor;
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ScopedSuspender)
};

class RhythmBoxAudioProcessor  : public AudioProcessor, public AsyncUpdater
{
public:
	//==============================================================================
	
	RhythmBoxAudioProcessor();
	~RhythmBoxAudioProcessor();

	//==============================================================================

	void prepareToPlay (double sampleRate, int samplesPerBlock) override;
	void releaseResources() override;
	void processBlock (AudioBuffer<float>&, MidiBuffer&) override;

	// Juce boilerplate ==============================================================

	#ifndef JucePlugin_PreferredChannelConfigurations
	bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
	#endif
	AudioProcessorEditor* createEditor() override;
	bool hasEditor() const override { return true; }
	const String getName() const override { return JucePlugin_Name; }
	bool acceptsMidi() const override { return true; }
	bool producesMidi() const override { return true; }
	bool isMidiEffect() const override { return false; }
	double getTailLengthSeconds() const override { return 0.0; }
	int getNumPrograms() override { return 1; }
	int getCurrentProgram() override { return 1; }
	void setCurrentProgram (int /*index*/) override { }
	const String getProgramName(int /*index*/) override { return {}; }
	void changeProgramName(int /*index*/, const String& /*newName*/) override { }
	void getStateInformation (MemoryBlock& destData) override;
	void setStateInformation (const void* data, int sizeInBytes) override;

	//==============================================================================

	String getCurrentStatusString();

	void setBeatDescription(BeatDescription* newDescription);
	void loadSfzPatch(const File& sfzFile);

	// Public actions
	void mainSwitchUp(int64 switchTime);
	void mainSwitchDown(int64 switchTime);

	enum class PluginState { Stopped, Playing, FillIn, Transition, Ending };
	PluginState getState() { return state; }
	void startPlaying();
	void stopPlaying();
	void pausePlaying();
	void fillIn();
	void nextPart();
	void ending();

	void addStateListener(ValueTree::Listener* listener) { vtState.addListener(listener); }
	void removeStateListener(ValueTree::Listener* listener) { vtState.removeListener(listener); }
	void handleAsyncUpdate() override;
private:
	// Configurations
	Configuration config { *this };

	ValueTree vtState { IDs::CurrentState };
	// Current Beat Description
	BeatDescription* description { nullptr };

	// Constants
	static constexpr float defaultTempo { 120.0f };

	// Actions
	PluginState state { PluginState::Stopped };
	
	// Process block submethods
	void processInputMidiEvents(MidiBuffer& midiMessages, double normalizedSamplePeriod);
	void processDeferredEvents(MidiBuffer& midiMessages, double endTime, double normalizedSamplePeriod);

	// Parameter mirrors and getters
	float* tempo{ nullptr };

	template<class T, class Ptr> static T checkNullWithDefault(Ptr* pointer, T def)
	{
		if (pointer != nullptr)
			return static_cast<T>(*pointer);
		else
			return def;
	}

	float getTempoParameter() const
	{ 
		auto head = getPlayHead();
		if (head == nullptr)
			return checkNullWithDefault(tempo, defaultTempo);
		
		AudioPlayHead::CurrentPositionInfo positionInfo;
		head->getCurrentPosition(positionInfo);
		if (positionInfo.bpm > 0)
		{
			return static_cast<float>(positionInfo.bpm);
		}
		
		return checkNullWithDefault(tempo, defaultTempo);
	}

	// Variables for the ain switch logic
	int64 lastSwitchUpTime { 0 };
	int64 lastSwitchDownTime { 0 };

	// Playheads (global and sequence playheads)
	double playTimeHead { 0.0 };
	double sequencePlayTimeHead { 0.0 };
	double samplePeriod { 1.0/44100.0 };
	void resetPlayHeads();

	// Data structures and methods for the current midi sequences
	// The sequences are garbage collected into poppedSequences to be deleted out of the audio thread since we have temporaries that may be destroyed when popping.
	std::deque<std::shared_ptr<BeatSequence>> sequences;
	std::vector<std::shared_ptr<BeatSequence>> poppedSequences;

	void pushSequenceFront(const std::shared_ptr<BeatSequence>& newSequence);
	void pushSequenceBack(const std::shared_ptr<BeatSequence>& newSequence);

	// Data structure for the deferred midi events we're sending back
	std::vector<MidiMessage> deferredEvents;
	void deferNoteOn(int noteNumber, uint8 velocity, double timestamp);
	void deferNoteOff(int noteNumber, double timestamp);
	void sendNoteOffsAndClear();

	// Private methods
	void setState(PluginState newState);

	sfzero::Synth sfzSynth;
	AudioFormatManager formatManager;

	// Parameters (Has to be last otherwise it create problem)
	AudioProcessorValueTreeState::ParameterLayout setUpParameters();
	AudioProcessorValueTreeState parameters{ *this, nullptr, Identifier("RhythmBoxParameters"), {} };
	//==============================================================================
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RhythmBoxAudioProcessor)
};