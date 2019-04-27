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
#include "BeatSequence.h"
#include "FileSearchers.h"

class BeatDescription
{
public:
	BeatDescription() = delete;
	BeatDescription(const File& file);
	~BeatDescription() {}
	static bool validateVarFile(const var& file);

	bool hasIntro() const noexcept { return (intro != nullptr && intro->getNumEvents() > 0); }
	bool hasEnding() const noexcept { return (ending != nullptr && intro->getNumEvents() > 0); }
	bool isEmpty() const noexcept { return parts.size() == 0; }
	double getSecondsPerQuarter() const noexcept { return secondsPerQuarter; }
	double getBPM() const noexcept { return 60.0/getSecondsPerQuarter(); }
	double getQuartersPerBar() const noexcept { return quartersPerBar; }

	std::shared_ptr<BeatSequence> getIntro() const noexcept;
	std::shared_ptr<BeatSequence> getEnding() const noexcept;
	std::shared_ptr<BeatSequence> getNextPart() noexcept;
	std::shared_ptr<BeatSequence> getCurrentPart() const noexcept;
	std::shared_ptr<BeatSequence> getTransition() const noexcept;
	std::shared_ptr<BeatSequence> getFill() noexcept;
	std::shared_ptr<BeatSequence> getRandomFill() noexcept;

	int getNumberOfParts() const noexcept;
    int getCurrentPartIndex() const noexcept;
    int getNumberOfFills() const noexcept;
    int getCurrentFillIndex() const noexcept;
	String getCurrentPartName() const noexcept;
	String getFullName() const noexcept;
	String getBeatName() const noexcept { return name; }
	String getBeatGroupName() const noexcept { return groupName; }
	String getFullPathName() const noexcept { return pathName; }

	void resetCounters();

	struct BeatPart
	{
		bool hasTransition() const noexcept { return (transition != nullptr && transition->getNumEvents() > 0); }
		String name;
		std::shared_ptr<BeatSequence> part{ std::make_shared<BeatSequence>() };
		std::vector<std::shared_ptr<BeatSequence>> fills;
		std::shared_ptr<BeatSequence> transition{ std::make_shared<BeatSequence>() };
	};

private:
	Random rnd {};

	double secondsPerQuarter{ 0.5 };
	int quartersPerBar{ 4 };
	int partCounter{ 0 };
	int fillCounter{ 0 };
	String groupName{ "" };
	String name{ "" };
	String pathName{ "" };

	void parseDescription(const var& description, const FileSearcher& searcher);
	std::shared_ptr<BeatSequence> readSequence(const var& fileVar, const FileSearcher& searcher);

	std::vector<BeatPart> parts;
	std::shared_ptr<BeatSequence> intro{ std::make_shared<BeatSequence>() };
	std::shared_ptr<BeatSequence> ending{ std::make_shared<BeatSequence>() };

	JUCE_LEAK_DETECTOR(BeatDescription);
};