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

#include "RhythmChooser.h"

RhythmListComponent::RhythmListComponent(Configuration& config)
: Component("Rhythm List"), config(config), fileBrowser(config)
{		
    auto* header = new TableHeaderComponent;
    header->addColumn ("Name", 1, 520, 100, -1, TableHeaderComponent::visible | TableHeaderComponent::sortable);
    header->addColumn ("Group", 2, 80, 20, -1, TableHeaderComponent::visible | TableHeaderComponent::sortable);
    header->addColumn ("Tempo", 3, 80, 20, -1, TableHeaderComponent::visible | TableHeaderComponent::sortable);
    header->addColumn ("", 4, 20, 20, 20, TableHeaderComponent::visible);
    header->setStretchToFitActive(true);
    beatList.setHeader(header);
    addAndMakeVisible(beatList);
    beatList.getViewport()->setScrollOnDragEnabled(true);
    beatList.getViewport()->setScrollBarsShown(true, false);

    addAndMakeVisible(addButton);
    addButton.setButtonText("Add...");
    addButton.onClick = [this]() { fileBrowser.setVisible(true); };

    addAndMakeVisible(closeButton);
    closeButton.setButtonText("Close");
    closeButton.onClick = [this]() { this->setVisible(false); };

    addChildComponent(fileBrowser);
}

int RhythmListComponent::getNumRows()
{
    return config.getNumBeats();
}
void RhythmListComponent::paintRowBackground(Graphics & g, [[maybe_unused]] int rowNumber, [[maybe_unused]] int width, [[maybe_unused]] int height, bool rowIsSelected )
{
    if (rowIsSelected)
        g.fillAll (Colours::lightblue);
    else
        g.fillAll(getLookAndFeel().findColour (ResizableWindow::backgroundColourId));
}
void RhythmListComponent::paintCell(Graphics & g, int rowNumber, int columnId, int width, int height, bool rowIsSelected)
{
    ignoreUnused(rowIsSelected);

    auto* description = config.getBeatAt(rowNumber);
    if (description == nullptr)
        return;
    g.setColour(Colours::white);
    switch(columnId)
    {
        case 1:
            g.drawText(description->getBeatName(), juce::Rectangle<int>(0, 0, width, height), Justification::left);
            break;
        case 2:
            g.drawText(description->getBeatGroupName(), juce::Rectangle<int>(0, 0, width, height), Justification::left);
            break;
        case 3:
            g.drawText(String(description->getBPM(), 1), juce::Rectangle<int>(0, 0, width, height), Justification::left);
            break;
        case 4:
            g.drawText("X", juce::Rectangle<int>(0, 0, width, height), Justification::left);
            break;
    }
}

void RhythmListComponent::cellDoubleClicked(int rowNumber, int columnId, [[maybe_unused]] const MouseEvent& evt)
{
    if (columnId != 4 && rowNumber < config.getNumBeats())
    {
        config.loadBeatDescription(rowNumber);
        setVisible(false);
    }		
}
void RhythmListComponent::cellClicked(int rowNumber, int columnId, [[maybe_unused]] const MouseEvent& evt)
{
    if (columnId == 4 && rowNumber < config.getNumBeats())
    {
        config.removeDescription(rowNumber);
        DBG("Removed description index " << rowNumber);
        refreshList();
    }
}

void RhythmListComponent::refreshList()
{
    beatList.updateContent();
    auto& header = beatList.getHeader();
    header.reSortTable();
}

void RhythmListComponent::paint(Graphics & g)
{
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));
    auto r = getLocalBounds();
    fileBrowser.setBounds(r);
    auto rightColumn = r.removeFromRight(80).reduced(5);
    addButton.setBounds(rightColumn.removeFromTop(30));
    closeButton.setBounds(rightColumn.removeFromTop(30));
    beatList.setBounds(r);
    auto& header = beatList.getHeader();
    header.resizeAllColumnsToFit(r.getWidth());
}

void RhythmListComponent::sortOrderChanged (int newSortColumnId, bool isForwards)
{
    switch(newSortColumnId)
    {
        case 1:
            config.sortByName(isForwards);
            break;
        case 2:
            config.sortByGroupName(isForwards);
            break;
        case 3:
            config.sortByBPM(isForwards);
            break;
        default:
            break;
    }
}

RhythmFileChooser::RhythmFileChooser(Configuration& config)
: fileBrowser(FileBrowserComponent( FileBrowserComponent::saveMode | FileBrowserComponent::canSelectFiles | FileBrowserComponent::canSelectMultipleItems,
    File::getSpecialLocation(File::SpecialLocationType::userDocumentsDirectory), nullptr, nullptr ))
, config(config)
{
    addAndMakeVisible(fileBrowser);
    fileBrowser.addListener(this);
    if (auto* innerFileListComponent = dynamic_cast<FileListComponent*>(fileBrowser.getDisplayComponent()))
    {
        innerFileListComponent->getViewport()->setScrollOnDragEnabled(true);
        // This allows multiple selection on touch devices
        // innerFileListComponent->setClickingTogglesRowSelection(false);
    }

    addAndMakeVisible(okButton);
    okButton.setButtonText("OK");
    okButton.onClick = [&](){ addSelectedFiles(); };
    addAndMakeVisible(cancelButton);
    cancelButton.setButtonText("Cancel");
    cancelButton.onClick = [&](){
        fileBrowser.deselectAllFiles();
        this->setVisible(false);
    };
}

void RhythmFileChooser::paint(Graphics & g)
{
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));
    auto r = getLocalBounds();
    auto rightColumn = r.removeFromRight(80).reduced(5);
    okButton.setBounds(rightColumn.removeFromTop(30));
    cancelButton.setBounds(rightColumn.removeFromTop(30));
    fileBrowser.setBounds(r);
}

void RhythmFileChooser::addSelectedFiles()
{
    for (auto idx = 0; idx < fileBrowser.getNumSelectedFiles(); ++idx)
    {
        const auto selectedFile = fileBrowser.getSelectedFile(idx);
        auto added = config.addDescription(selectedFile);
        if (added)
            DBG("Added 1 file " << selectedFile.getFullPathName());
        else
            DBG("Failed 1 file " << selectedFile.getFullPathName());
    }
    fileBrowser.deselectAllFiles();
    this->setVisible(false);
}