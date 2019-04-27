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
#include "Configuration.h"
#include "Watchers.h"

class RhythmFileChooser: public Component, public FileBrowserListener 
{
public:
	RhythmFileChooser(Configuration& config);
	void paint(Graphics & g);

	// FileBrowserListener callbacks
	void selectionChanged() override {}
	void fileClicked (const File&, const MouseEvent&) override {}
	void fileDoubleClicked (const File&) override { addSelectedFiles(); }
	void browserRootChanged (const File&) override {}
private:
	void addSelectedFiles();
	Configuration& config;
	FileBrowserComponent fileBrowser;
	TextButton okButton;
	TextButton cancelButton;
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RhythmFileChooser)
};

class RhythmListComponent: public Component, public TableListBoxModel
{
public:
	RhythmListComponent(Configuration& config);
	void paint(Graphics & g);
	
	// TableListBoxModel callbacks
	int getNumRows() override;
	void paintRowBackground(Graphics & g, int rowNumber, int width, int height, bool rowIsSelected ) override;
	void paintCell(Graphics & g, int rowNumber, int columnId, int width, int height, bool rowIsSelected) override;
	void cellDoubleClicked(int rowNumber, int columnId, const MouseEvent& evt);
	void cellClicked(int rowNumber, int columnId, const MouseEvent& evt);
	void sortOrderChanged (int newSortColumnId,	bool isForwards) override;

private:
	Configuration& config;
	RhythmFileChooser fileBrowser { config };
	SimpleVisibilityWatcher<RhythmFileChooser> watcher { fileBrowser, [this] () { this->refreshList(); } };
	TableListBox beatList { "Rhythm List", this };
	TextButton addButton;
	TextButton closeButton;
	void refreshList();
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RhythmListComponent)
};