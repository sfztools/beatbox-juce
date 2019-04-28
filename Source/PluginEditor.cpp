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
#include <utility>

//==============================================================================
RhythmBoxAudioProcessorEditor::RhythmBoxAudioProcessorEditor (RhythmBoxAudioProcessor& p, AudioProcessorValueTreeState& parameters, Configuration& config)
        :	AudioProcessorEditor (&p),
        processor (p),
        config(config),
        processorParameters(parameters)
{
    // openGLContext.attachTo(*getTopLevelComponent());
    
    addAndMakeVisible(beatManagerButton);
    beatManagerButton.setButtonText("Rhythms");
    beatManagerButton.onClick = [this] { rhythmList.setVisible(true); };

    addAndMakeVisible(sfzManagerButton);
    sfzManagerButton.setButtonText("Drums");
    sfzManagerButton.onClick = [this] { sfzList.setVisible(true); };

    addAndMakeVisible(stopButton);
    stopButton.setButtonText("Stop and\nReset");
    stopButton.onClick = [this] { processor.stopPlaying(); };
    stopButton.setColour(TextButton::buttonColourId, Colours::red);

    addAndMakeVisible(fillButton);
    fillButton.setButtonText("Fill");
    fillButton.onClick = [this] { processor.fillIn(); };
    
    addAndMakeVisible(nextButton);
    nextButton.setButtonText("Next");
    nextButton.onClick = [this] { processor.nextPart(); };

    addAndMakeVisible(endButton);
    endButton.setButtonText("Ending");
    endButton.onClick = [this] { processor.ending(); };

    addAndMakeVisible(tapButton);
    tapButton.setButtonText("Tap");
    tapButton.onClick = [this] { tapButtonClicked(); };

    addAndMakeVisible(tempoSlider);
    tempoSlider.setSliderStyle(Slider::RotaryHorizontalVerticalDrag);
    tempoSlider.setTextBoxStyle(Slider::TextBoxAbove, false, 80, 20);
    tempoAttachment.reset(new SliderAttachment(processorParameters, "tempo", tempoSlider));
    tempoSlider.setTextValueSuffix(" BPM");

    addAndMakeVisible(tempoLabel);
    tempoLabel.setText("Tempo", dontSendNotification);
    tempoLabel.attachToComponent(&tempoSlider, true);

    addAndMakeVisible(playButton);
    playButton.setButtonText("Play\nPause");
    playButton.onClick = [this] { playButtonClicked(); };
    playButton.setColour(TextButton::buttonColourId, Colours::green);
    
    addAndMakeVisible(statusBox);
    addAndMakeVisible(configBox);

    // Covers other components so they have to be last
    addChildComponent(rhythmList);
    addChildComponent(sfzList);

    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (700, 300);
}

RhythmBoxAudioProcessorEditor::~RhythmBoxAudioProcessorEditor()
{
    // openGLContext.detach();
}

void RhythmBoxAudioProcessorEditor::playButtonClicked()
{
    if (processor.getState() != RhythmBoxAudioProcessor::PluginState::Playing)
        processor.startPlaying();
    else  
        processor.pausePlaying();
}

void RhythmBoxAudioProcessorEditor::tapButtonClicked()
{
    const auto currentTap { Time::currentTimeMillis() };
    if (currentTap - lastTap < 6000)
    {
        const float tempo { 60.0f / (static_cast<float>(currentTap - lastTap) / 1000.0f) };
        DBG("Tempo: " << tempo);
        if (tempo > 10.0 && tempo < 300.0)
            processorParameters.getParameterAsValue("tempo").setValue(tempo);
        lastTap = -1; // So that we need 2 taps to select a "new" beat
    }
    lastTap = currentTap;
}

//==============================================================================
void RhythmBoxAudioProcessorEditor::paint (Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));
    auto bounds = getLocalBounds().reduced(5);
    auto firstRow = bounds.removeFromTop(proportionOfHeight(0.6f));
    auto secondRow = bounds;
    // openButton.setBounds(r.removeFromTop(proportionOfHeight(0.10f)));

    auto tempoColumn { firstRow.removeFromLeft(100) };
    tempoSlider.setBounds(tempoColumn.removeFromTop(120));
    tapButton.setBounds(tempoColumn.removeFromTop(30));
    firstRow.removeFromLeft(5);

    auto fileColumn = firstRow.removeFromRight(80);
    beatManagerButton.setBounds(fileColumn.removeFromTop(50));
    sfzManagerButton.setBounds(fileColumn.removeFromTop(30));
    firstRow.removeFromRight(5);
    
    statusBox.setBounds(firstRow.removeFromTop(80));
    firstRow.removeFromTop(10);
    configBox.setBounds(firstRow.removeFromTop(80));

    // Lower row of buttons
    const auto buttons = { &playButton, &fillButton, &nextButton, &endButton, &stopButton };
    const float borderProportion { 0.05f };
    const int buttonSize { secondRow.proportionOfWidth((1.0f - 2 * borderProportion) / buttons.size()) };
    secondRow.removeFromLeft(secondRow.proportionOfWidth(borderProportion));
    for (auto* button: buttons)
    {
        button->setBounds(secondRow.removeFromLeft(buttonSize).reduced(2));
    }

    rhythmList.setBounds(getLocalBounds());
    sfzList.setBounds(getLocalBounds());
}

void RhythmBoxAudioProcessorEditor::resized()
{
        // This is generally where you'll want to lay out the positions of any
        // subcomponents in your editor..
}



StatusBox::StatusBox(RhythmBoxAudioProcessor& processor)
: Component("Status Box"), processor(processor)
{
    addAndMakeVisible(selectedBeatLabel);
    selectedBeatLabel.setJustificationType(Justification::centred);
    selectedBeatLabel.setFont(getLookAndFeel().getLabelFont(selectedBeatLabel).withHeight(24.0f));
    selectedBeatLabel.setText("Select a rhythm", dontSendNotification);

    addAndMakeVisible(selectedSfzLabel);
    selectedSfzLabel.setJustificationType(Justification::centred);
    selectedSfzLabel.setFont(getLookAndFeel().getLabelFont(selectedSfzLabel).withHeight(16.0f));
    selectedSfzLabel.setText("MIDI output only", dontSendNotification);

    addAndMakeVisible(lastCCLabel);
    lastCCLabel.setJustificationType(Justification::centred);
    lastCCLabel.setText("", dontSendNotification);

    processor.addStateListener(this);
}

void StatusBox::valueTreePropertyChanged (ValueTree& valueTree, const Identifier &property)
{
    if (property == IDs::SfzFile)
    {
        selectedSfzLabel.setText(valueTree.getProperty(IDs::SfzFile).toString(), dontSendNotification);
        return;
    }
     
    auto labelText { valueTree.getProperty(IDs::BeatName).toString() };
	switch (processor.getState())
	{
	case RhythmBoxAudioProcessor::PluginState::Playing:
		labelText << " | " << valueTree.getProperty(IDs::PartName).toString();
		break;
	case RhythmBoxAudioProcessor::PluginState::FillIn:
		labelText << " | " << valueTree.getProperty(IDs::PartName).toString()
			<< " (Fill " << valueTree.getProperty(IDs::FillIdx).toString() << "/" << valueTree.getProperty(IDs::NumFills).toString() << ")";
		break;
	case RhythmBoxAudioProcessor::PluginState::Transition:
		labelText << " -> " << valueTree.getProperty(IDs::PartName).toString();
		break;
	}
    selectedBeatLabel.setText(labelText, dontSendNotification);
}

StatusBox::~StatusBox()
{
    processor.removeStateListener(this);
}

void StatusBox::paint(Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));
    auto bounds { getLocalBounds() };
    selectedBeatLabel.setBounds(bounds.removeFromTop(50));
    selectedSfzLabel.setBounds(bounds.removeFromTop(50));
    lastCCLabel.setBounds(bounds.removeFromTop(30));
}

ConfigBox::ConfigBox(Configuration& config)
: config { config }
{
    addAndMakeVisible(mainCCLabel);
    addAndMakeVisible(mainCCBox);
    for (auto cc = 1; cc < 127; cc++)
        mainCCBox.addItem(String(cc), cc);
    mainCCBox.setSelectedId(config.getMainCC(), dontSendNotification);
    mainCCBox.onChange = [&](){ config.setMainCC(mainCCBox.getSelectedId()); };

    addAndMakeVisible(accentCCLabel);
    addAndMakeVisible(accentCCBox);
    for (auto cc = 1; cc < 127; cc++)
        accentCCBox.addItem(String(cc), cc);
    accentCCBox.setSelectedId(config.getAccentCC(), dontSendNotification);
    accentCCBox.onChange = [&](){ config.setAccentCC(accentCCBox.getSelectedId()); };

    addAndMakeVisible(inputChannelLabel);
    addAndMakeVisible(inputChannelBox);
    for (auto cc = 1; cc <= 16; cc++)
        inputChannelBox.addItem(String(cc), cc);
    inputChannelBox.setSelectedId(config.getInputChannel(), dontSendNotification);
    inputChannelBox.onChange = [&](){ config.setInputChannel(inputChannelBox.getSelectedId()); };

    addAndMakeVisible(outputChannelLabel);
    addAndMakeVisible(outputChannelBox);
    for (auto cc = 1; cc <= 16; cc++)
        outputChannelBox.addItem(String(cc), cc);
    outputChannelBox.setSelectedId(config.getOutputChannel(), dontSendNotification);
    outputChannelBox.onChange = [&](){ config.setOutputChannel(outputChannelBox.getSelectedId()); };
    
    addAndMakeVisible(randomFillsButton);
    randomFillsButton.setToggleState(config.useRandomFills(), dontSendNotification);
    randomFillsButton.onStateChange = [&] () { config.setRandomFills(randomFillsButton.getToggleState()); };
}

void ConfigBox::paint(Graphics & g)
{
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));
    auto secondColumn = getLocalBounds();
    auto firstColumn = secondColumn.removeFromLeft(proportionOfWidth(0.5f));

    auto lambdaTextComponent = [&](auto& region, auto& component, auto& label) {
        auto subregion = region.removeFromTop(20);
        label.setBounds(subregion.removeFromLeft(170));
        component.setBounds(subregion);
    };

    lambdaTextComponent(firstColumn, mainCCBox, mainCCLabel);
    lambdaTextComponent(firstColumn, accentCCBox, accentCCLabel);
    lambdaTextComponent(firstColumn, inputChannelBox, inputChannelLabel);
    lambdaTextComponent(firstColumn, outputChannelBox, outputChannelLabel);

    randomFillsButton.setBounds(secondColumn.removeFromTop(20));
}