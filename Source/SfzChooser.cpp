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

#include "SfzChooser.h"

SfzListComponent::SfzListComponent(Configuration& config)
: Component("Sfz List"), config(config), fileBrowser(config)
{		
    auto* header = new TableHeaderComponent;
    header->addColumn ("Name", 1, 600, 100, -1, TableHeaderComponent::visible);
    header->addColumn ("", 2, 20, 20, 20, TableHeaderComponent::visible);
    header->setStretchToFitActive(true);
    sfzList.setHeader(header);
    addAndMakeVisible(sfzList);
    sfzList.getViewport()->setScrollOnDragEnabled(true);
    sfzList.getViewport()->setScrollBarsShown(true, false);

    addAndMakeVisible(addButton);
    addButton.setButtonText("Add...");
    addButton.onClick = [this]() { fileBrowser.setVisible(true); };

    addAndMakeVisible(closeButton);
    closeButton.setButtonText("Close");
    closeButton.onClick = [this]() { this->setVisible(false); };

    addChildComponent(fileBrowser);
}

int SfzListComponent::getNumRows()
{
    return config.getNumSfz() + 1;
}
void SfzListComponent::paintRowBackground(Graphics & g, [[maybe_unused]] int rowNumber, [[maybe_unused]] int width, [[maybe_unused]] int height, bool rowIsSelected )
{
    if (rowIsSelected)
        g.fillAll (Colours::lightblue);
    else
        g.fillAll(getLookAndFeel().findColour (ResizableWindow::backgroundColourId));
}
void SfzListComponent::paintCell(Graphics & g, int rowNumber, int columnId, int width, int height, [[maybe_unused]] bool rowIsSelected)
{
    g.setColour(Colours::white);
    switch(columnId)
    {
        case 1:
            if (rowNumber == 0)
            {
                g.drawText("Use MIDI output only", juce::Rectangle<int>(0, 0, width, height), Justification::left);
            }
            else
            {
                auto* file = config.getSfzAt(rowNumber - 1); // The first is always no file
                if (file == nullptr)
                    return;
                    
                g.drawText(file->getFileNameWithoutExtension(), juce::Rectangle<int>(0, 0, width, height), Justification::left);
            }
            break;
        case 2:
            if (rowNumber > 0)
                g.drawText("X", juce::Rectangle<int>(0, 0, width, height), Justification::left);
            break;
    }
}
void SfzListComponent::cellDoubleClicked(int rowNumber, int columnId, [[maybe_unused]] const MouseEvent& evt)
{
    if (columnId != 2 && (rowNumber - 1) < config.getNumSfz())
    {
        config.loadSfzPatch(rowNumber - 1);
        setVisible(false);
    }
    if (rowNumber == 0)
    {
        config.clearSfzPatch();
    }
}
void SfzListComponent::cellClicked(int rowNumber, int columnId, [[maybe_unused]] const MouseEvent& evt)
{
    if (columnId == 2 && (rowNumber - 1) < config.getNumSfz())
    {
        config.removeSfzFile(rowNumber - 1);
        DBG("Removed description index " << rowNumber - 1);
        refreshList();
    }
}

void SfzListComponent::refreshList()
{
    sfzList.updateContent();
}

void SfzListComponent::paint(Graphics & g)
{
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));
    auto r = getLocalBounds();
    fileBrowser.setBounds(r);
    auto rightColumn = r.removeFromRight(80).reduced(5);
    addButton.setBounds(rightColumn.removeFromTop(30));
    closeButton.setBounds(rightColumn.removeFromTop(30));
    sfzList.setBounds(r);
    auto& header = sfzList.getHeader();
    header.resizeAllColumnsToFit(r.getWidth());
}

void SfzListComponent::sortOrderChanged ([[maybe_unused]] int newSortColumnId, [[maybe_unused]] bool isForwards)
{
    
}

SfzFileChooser::SfzFileChooser(Configuration& config)
: fileBrowser(FileBrowserComponent( FileBrowserComponent::saveMode | FileBrowserComponent::canSelectFiles | FileBrowserComponent::canSelectMultipleItems,
    File::getSpecialLocation(File::SpecialLocationType::userDocumentsDirectory), nullptr, nullptr ))
, config(config)
{
    addAndMakeVisible(fileBrowser);
    fileBrowser.addListener(this);
    if (auto* innerFileListComponent = dynamic_cast<FileListComponent*>(fileBrowser.getDisplayComponent()))
    {
        innerFileListComponent->getViewport()->setScrollOnDragEnabled(true);
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

void SfzFileChooser::paint(Graphics & g)
{
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));
    auto r = getLocalBounds();
    auto rightColumn = r.removeFromRight(80).reduced(5);
    okButton.setBounds(rightColumn.removeFromTop(30));
    cancelButton.setBounds(rightColumn.removeFromTop(30));
    fileBrowser.setBounds(r);
}

void SfzFileChooser::addSelectedFiles()
{
    for (auto idx = 0; idx < fileBrowser.getNumSelectedFiles(); ++idx)
    {
        const auto selectedFile = fileBrowser.getSelectedFile(idx);
        config.addSfzFile(selectedFile);
    }
    fileBrowser.deselectAllFiles();
    this->setVisible(false);
}