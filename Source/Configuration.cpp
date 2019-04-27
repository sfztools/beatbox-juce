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

#include "Configuration.h"
#include "PluginProcessor.h"
#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h"

bool Configuration::parseConfiguration(const String& filename)
{
    const auto file = dataDirectory.getChildFile(filename);

    if (!file.exists())
    {
        spdlog::error("Can't find the configuration file at \"{}\"", file.getFileName());
        return false;
    }

    FileInputStream fileStream(file);
    if (!fileStream.openedOk())
    {
        spdlog::error("Could not create input file stream for \"{}\"");
        return {};
    }

    originalConfig = JSON::parse(fileStream);

    // General parameters
    setInputChannel( originalConfig.getProperty("input_channel", defaultInputChannel) );
    setOutputChannel( originalConfig.getProperty("output_channel", defaultOutputChannel) );
    setMainCC( originalConfig.getProperty("main_cc", defaultMainCC) );
    setAccentCC( originalConfig.getProperty("accent_cc", defaultAccentCC) );
    setAccentDuration( originalConfig.getProperty("accent_duration", defaultAccentDuration) );
    setRandomFills( originalConfig.getProperty("random_fills", defaultRandomFills) );    
    
    if (originalConfig.hasProperty("rhythms"))
    {
        for (auto& rythm : *originalConfig.getProperty("instruments", var()).getArray())
        {
            const auto descriptionFile = dataDirectory.getChildFile(rythm.toString());
            addDescription(descriptionFile);
        }
    }

    if (originalConfig.hasProperty("instruments"))
    {
        for (auto& instrument : *originalConfig.getProperty("instruments", var()).getArray())
        {
            const auto sfzFile = dataDirectory.getChildFile(instrument.toString());
            if (sfzFile.exists())
                sfzFiles.push_back(sfzFile);
        }
    }
    
    return true;
}

bool Configuration::addDescription(const File& descriptionFile)
{
    BeatDescription newDescription { descriptionFile };
    if (!newDescription.isEmpty())
    {
        beatDescriptions.push_back(std::move(descriptionFile));
    }
    return !newDescription.isEmpty();
}

bool Configuration::saveConfiguration(const String& filename)
{
    auto* configToSave = new DynamicObject();
    configToSave->setProperty("input_channel", inputChannel);
    configToSave->setProperty("output_channel", outputChannel);
    configToSave->setProperty("accent_duration", accentDuration);
    configToSave->setProperty("main_cc", mainCC);
    configToSave->setProperty("accent_cc", accentCC);
    configToSave->setProperty("random_fills", randomFills);

    var beatsToSave;
    beatsToSave.resize(0); // Empty array
    for (auto& beat: beatDescriptions)
    {
        beatsToSave.append(beat.getFullPathName());
    }
    configToSave->setProperty("rhythms", beatsToSave);
    
    var sfzToSave;
    sfzToSave.resize(0); // Empty array
    for (auto& sfz: sfzFiles)
    {
        sfzToSave.append(sfz.getFullPathName());
    }
    configToSave->setProperty("instruments", sfzToSave);

    FileOutputStream outStream{ dataDirectory.getChildFile(filename) };
    if (outStream.failedToOpen())
    {
        spdlog::error("Could not open output config file for writing");
        return false; 
    }

    outStream.setPosition (0);
    outStream.truncate();
    var outputConfig { configToSave };
    JSON::writeToStream(outStream, outputConfig);
    outStream.flush();
    return true;
}

StringArray Configuration::getBeatList() const
{
    StringArray returnedValue {};
    for (auto& beat : beatDescriptions)
        returnedValue.add(beat.getBeatName());
    return returnedValue;
}

StringArray Configuration::getSfzList() const
{
    StringArray returnedValue {};
    for (auto& file: sfzFiles)
        returnedValue.add(file.getFileName());
    return returnedValue;
}

void Configuration::setInputChannel(int channel)
{
    if (channel < 1 || channel > 16)
    {
        spdlog::error("Wrong value for the input channel: {} (ignored)", channel);
        return;
    }
    inputChannel = channel;
}

void Configuration::setRandomFills(bool useRandom)
{
    randomFills = useRandom;
}

void Configuration::setOutputChannel(int channel)
{
    if (channel < 1 || channel > 16)
    {
        spdlog::error("Wrong value for the output channel: {} (ignored)", channel);
        return;
    }
    outputChannel = channel;
}
void Configuration::setMainCC(int CC)
{
    if (CC < 1 || CC > 127)
    {
        spdlog::error("Wrong value for the main CC: {} (ignored)", CC);
        return;
    }
    mainCC = CC;
}
void Configuration::setAccentCC(int CC)
{
    if (CC < 1 || CC > 127)
    {
        spdlog::error("Wrong value for the accent CC: {} (ignored)", CC);
        return;
    }
    accentCC = CC;
}
void Configuration::setAccentDuration(double duration)
{
    if (duration < 0.0)
    {
        spdlog::error("Negative accent duration: {} (ignored)", duration);
        return;
    }
    accentDuration = duration;
}

void Configuration::loadBeatDescription(int idx)
{
    beatIndex = idx;
    BeatDescription* description {};
    if (beatIndex < beatDescriptions.size() && beatIndex >= 0)
        description = &beatDescriptions[beatIndex];
    processor.setBeatDescription(description);
}

void Configuration::loadSfzPatch(int idx)
{
    sfzIndex = idx;
    if (sfzIndex < sfzFiles.size() && sfzIndex >= 0)
        processor.loadSfzPatch(sfzFiles[sfzIndex]);
}

void Configuration::clearSfzPatch()
{
    processor.loadSfzPatch({});
}

const BeatDescription* Configuration::getBeatAt(int idx) const
{
    if (idx < beatDescriptions.size())
        return &beatDescriptions[idx];
    
    return nullptr;
}
const File* Configuration::getSfzAt(int idx) const
{
    if (idx < sfzFiles.size())
        return &sfzFiles[idx];
    
    return nullptr;
}

void Configuration::sortByName(bool forward)
{
    std::sort(beatDescriptions.begin(), beatDescriptions.end(), [forward](const BeatDescription& lhs, const BeatDescription& rhs){
        return forward ? lhs.getBeatName() > rhs.getBeatName() : lhs.getBeatName() < rhs.getBeatName();
    });
}

void Configuration::sortByGroupName(bool forward)
{
    std::sort(beatDescriptions.begin(), beatDescriptions.end(), [forward](const BeatDescription& lhs, const BeatDescription& rhs){
        if (forward)
        {
            if (lhs.getBeatGroupName() > rhs.getBeatGroupName())
                return true;
            if (lhs.getBeatGroupName() == rhs.getBeatGroupName() && lhs.getBPM() > rhs.getBPM())
                return true;
            return false;
        }
        else
        {
            if (lhs.getBeatGroupName() < rhs.getBeatGroupName())
                return true;
            if (lhs.getBeatGroupName() == rhs.getBeatGroupName() && lhs.getBPM() < rhs.getBPM())
                return true;
            return false;
        }
    });
}

void Configuration::sortByBPM(bool forward)
{
    std::sort(beatDescriptions.begin(), beatDescriptions.end(), [forward](const BeatDescription& lhs, const BeatDescription& rhs){
        return forward ? lhs.getBPM() > rhs.getBPM() : lhs.getBPM() < rhs.getBPM();
    });
}

void Configuration::addSfzFile(const File& sfzFile)
{
    if (sfzFile.exists())
        sfzFiles.push_back(sfzFile);
}

void Configuration::removeDescription(int idx)
{
    if (idx < beatDescriptions.size())
        beatDescriptions.erase(beatDescriptions.begin() + idx);
}

void Configuration::removeSfzFile(int idx)
{
    if (idx < sfzFiles.size())
        sfzFiles.erase(sfzFiles.begin() + idx);
}