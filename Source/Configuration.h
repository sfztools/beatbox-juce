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
#include "BeatDescription.h"

inline static File dataDirectory { File::getSpecialLocation(File::SpecialLocationType::userApplicationDataDirectory).getChildFile("RhythmBox") };

class RhythmBoxAudioProcessor;

class Configuration
{
public:
    Configuration() = delete;
    ~Configuration() { saveConfiguration(); }
    Configuration(RhythmBoxAudioProcessor& p) : processor(p) {	}
    
    bool parseConfiguration(const String& filename = "config.json");
    bool saveConfiguration(const String& filename = "config.json");

    bool addDescription(const File& descriptionFile);
    void addSfzFile(const File& sfzFile);
    void removeDescription(int idx);
    void removeSfzFile(int idx);

    StringArray getBeatList() const;
    int getNumBeats() const { return static_cast<int>(beatDescriptions.size()); }
    StringArray getSfzList() const;
    int getNumSfz() const { return static_cast<int>(sfzFiles.size()); }
    const BeatDescription* getBeatAt(int idx) const;
    const File* getSfzAt(int idx) const;
    
    void sortByName(bool forward);
    void sortByGroupName(bool forward);
    void sortByBPM(bool forward);

    auto getInputChannel() const { return inputChannel; }
    auto getOutputChannel() const { return outputChannel; }
    auto getMainCC() const { return mainCC; }
    auto getAccentCC() const { return accentCC; }
    auto getAccentDuration() const { return accentDuration; }
    bool useRandomFills() const { return randomFills; }
    bool syncTempoToHost() const { return syncToHost; }

    void setInputChannel(int channel);
    void setOutputChannel(int channel);
    void setMainCC(int CC);
    void setAccentCC(int CC);
    void setAccentDuration(double duration);
    void setRandomFills(bool useRandom);
    void setSyncToHost(bool sync);

    void loadBeatDescription(int beatIndex);
    void loadSfzPatch(int sfzFileIndex);
    void clearSfzPatch();
    int getCurrentBeatIndex() const { return beatIndex; }
    int getCurrentSfzIndex() const { return sfzIndex; }

    static constexpr double defaultAccentDuration { 0.1 };	
    static constexpr int defaultInputChannel { 1 };
    static constexpr int defaultOutputChannel { 10 };
    static constexpr int defaultMainCC { 68 };
    static constexpr int defaultAccentCC { 67 };
    static constexpr bool defaultRandomFills { true };
    static constexpr bool defaultSyncToHost { true };
private:
    RhythmBoxAudioProcessor& processor;

    int mainCC { defaultMainCC };
    int accentCC { defaultAccentCC };
    int inputChannel { defaultInputChannel };
    int outputChannel { defaultOutputChannel };
    double accentDuration { defaultAccentDuration };
    bool randomFills { defaultRandomFills };
    bool syncToHost { defaultSyncToHost };

    // Current beat description
    int beatIndex { -1 };
    // Current sfzFile description
    int sfzIndex { -1 };

    std::vector<BeatDescription> beatDescriptions;
    std::vector<File> sfzFiles;

    var originalConfig;
    // File dataDirectory{ File::getSpecialLocation(File::SpecialLocationType::userApplicationDataDirectory).getChildFile("RhythmBox") };

    JUCE_LEAK_DETECTOR(Configuration);
};