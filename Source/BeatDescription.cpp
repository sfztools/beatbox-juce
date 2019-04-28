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

#include "BeatDescription.h"
#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h"

BeatDescription::BeatDescription(const File& descriptionFile)
{    
    if (!descriptionFile.exists())
    {
        spdlog::error("Description file does not exist \"{}\"", descriptionFile.getFullPathName());
        return;
    }

    pathName = descriptionFile.getFullPathName();
    if (descriptionFile.getFileExtension() == ".zip")
    {
        ZipFileSearcher searcher(descriptionFile);

        auto fileStream = searcher.createFileStream("description.json");
        if (fileStream == nullptr)
        {
            spdlog::error("Could not create input file stream for the \"description.json\" file within \"{}\"", pathName);
            return;
        }

        const auto description = JSON::parse(*fileStream);
        DBG("File" << description.toString());
        if (!validateVarFile(description))
        {
            spdlog::error("Something went wrong when parsing the \"description.json\" file within \"{}\"", pathName);
            return;
        }

        parseDescription(description, searcher);
    }
    else
    {
        FileInputStream fileStream{ descriptionFile };
        if (!fileStream.openedOk())
        {
            spdlog::error("Could not create input file stream for \"{}\"", pathName);
            return;
        }

        const auto description = JSON::parse(fileStream);
        if (!validateVarFile(description))
        {
            spdlog::error("Something went wrong when parsing \"{}\"", pathName);
            return;
        }

        NormalFileSearcher searcher{ descriptionFile.getParentDirectory() };
        parseDescription(description, searcher);
    }  
}

void BeatDescription::parseDescription(const var& description, const FileSearcher& searcher)
{
    name = description.getProperty("name", "").toString();
    secondsPerQuarter = 60.0 / static_cast<double>(description.getProperty("bpm", 120));
    quartersPerBar = static_cast<int>(description.getProperty("quarters_per_bar", 4));
    groupName = description.getProperty("group", "");

    for (auto& part : *description.getProperty("parts", var()).getArray())
    {
        BeatDescription::BeatPart beatPart;
        beatPart.name = part.getProperty("name", "").toString();
        beatPart.part = readSequence(part.getProperty("midi_file", var()), searcher);
        // Checking for fills
        if (part.getProperty("fills", var()).isArray())
        {
            for (auto& fill : *part.getProperty("fills", var()).getArray())
            {
                auto fillSequence = readSequence(fill, searcher);
                if (fillSequence != nullptr && fillSequence->getNumEvents() > 0)
                    beatPart.fills.push_back(fillSequence);
            }
        }
        beatPart.transition = readSequence(part.getProperty("transition", var()), searcher);
        parts.push_back(std::move(beatPart));
    }
    
    partCounter = 0; // We have at least one part so this is the current one

    intro = readSequence(description.getProperty("intro", var()), searcher);
    ending = readSequence(description.getProperty("ending", var()), searcher);
    
    groupName = groupName;
}

std::shared_ptr<BeatSequence> BeatDescription::readSequence(const var& fileVar, const FileSearcher& searcher)
{

    const auto filename = fileVar.getProperty("filename", "").toString();
    const int numberOfBars = fileVar.getProperty("bars", this->quartersPerBar);
    const int ignoreBars = fileVar.getProperty("ignore_bars", 0);
    
    if (filename == "")
        return {};

    auto fileStream = searcher.createFileStream(filename);
    
    if (fileStream == nullptr)
    {
        spdlog::error("Can't open stream for \"{}\"", filename);
        return {};
    }

    MidiFile midiFile;
    if (!midiFile.readFrom(*fileStream))
    {
        spdlog::error("Can't read from MIDI file \"{}\" for some reason", filename);
        return {};
    }

    spdlog::debug("Reading MIDI file \"{}\"", filename);
    
    return std::make_shared<BeatSequence>(midiFile, numberOfBars, ignoreBars);
}

bool BeatDescription::validateVarFile(const var& fileVar)
{
    if (!fileVar.getProperty("name", var()).isString())
    {
        spdlog::error("Error: invalid or absent name field in JSON file.");
        return false;
    }
    if (!fileVar.getProperty("bpm", var()).isDouble() && !fileVar.getProperty("bpm", var()).isInt())
    {
        spdlog::error("Error: invalid or absent bpm field in JSON file.");
        return false;
    }
    else if (static_cast<double>(fileVar.getProperty("bpm", var())) < 0.0)
    {
        spdlog::error("Error: negative BPM in JSON file.");
        return false;
    }
    if (!fileVar.getProperty("quarters_per_bar", var()).isInt())
    {
        spdlog::error("Error: invalid or absent quarters_per_bar field in JSON file.");
        return false;
    }
    if (!fileVar.getProperty("parts", var()).isArray() ||
        fileVar.getProperty("parts", var()).getArray()->size() == 0)
    {
        spdlog::error("Error: invalid, absent or empty parts array in JSON file.");
        return false;
    }
    else
    {
        auto partArray = fileVar.getProperty("parts", var()).getArray();
        if (!partArray->getFirst().getProperty("name", var()).isString())
        {
            spdlog::error("Error: invalid or absent name field in the first part array of JSON file.");
            return false;
        }
        if (!partArray->getFirst().getProperty("midi_file", var()).getProperty("filename", var()).isString())
        {
            spdlog::error("Error: invalid or absent midi_file field in the first part array of JSON file.");
            return false;
        }
    }
    return true;
}

std::shared_ptr<BeatSequence> BeatDescription::getIntro() const noexcept
{
    return intro;
}
std::shared_ptr<BeatSequence> BeatDescription::getEnding() const noexcept
{
    return ending;
}

std::shared_ptr<BeatSequence> BeatDescription::getNextPart() noexcept
{
    if (!parts.empty())
    {
        fillCounter = 0;
        return parts[++partCounter % parts.size()].part;
    }
    return {};
}

std::shared_ptr<BeatSequence> BeatDescription::getCurrentPart() const noexcept
{
    if (!parts.empty())
        return parts[partCounter % parts.size()].part;
    return {};
}

std::shared_ptr<BeatSequence> BeatDescription::getTransition() const noexcept
{
    if (!parts.empty())
        return parts[partCounter % parts.size()].transition;
    return {};
}
std::shared_ptr<BeatSequence> BeatDescription::getFill() noexcept
{
    auto partIdx = partCounter % parts.size();
    if (parts[partIdx].fills.size() != 0)
    {
        auto nextFillIdx = fillCounter++ % parts[partIdx].fills.size();
        return parts[partIdx].fills[nextFillIdx];
    }
    return {};
}

std::shared_ptr<BeatSequence> BeatDescription::getRandomFill() noexcept
{
    if (parts.size() == 0)
        return {};
    
    const int partIdx { static_cast<int>(partCounter % parts.size()) };
    const int numFills { static_cast<int>(parts[partIdx].fills.size()) };
    if (parts[partIdx].fills.size() != 0)
    {
        fillCounter = rnd.nextInt(numFills);
        return parts[partIdx].fills[fillCounter];
    }

    return {};
}

int BeatDescription::getNumberOfParts() const noexcept
{
    return static_cast<int>(parts.size());
}

int BeatDescription::getCurrentPartIndex() const noexcept
{
    if (!parts.empty())
        return static_cast<int>((partCounter % parts.size()) + 1);
    return 0;
}

int BeatDescription::getNumberOfFills() const noexcept
{
    if (!parts.empty())
        return static_cast<int>(parts[partCounter % parts.size()].fills.size());
    return 0;
}

int BeatDescription::getCurrentFillIndex() const noexcept
{
    if (!parts.empty() && !parts[partCounter % parts.size()].fills.empty())
        return static_cast<int>(((fillCounter - 1) % parts[partCounter % parts.size()].fills.size()) + 1);
    return 0;
}

String BeatDescription::getCurrentPartName() const noexcept
{
    if (!parts.empty() && partCounter >= 0)
        return parts[partCounter % parts.size()].name;
    return {};
}

String BeatDescription::getFullName() const noexcept
{
    String returnedString {}; 
    if (groupName != "")
        returnedString += groupName + " - " ;
    
    returnedString += String(getBPM(), 1) + " - " + name;
    return returnedString;
}

void BeatDescription::resetCounters()
{
    partCounter = 0;
    fillCounter = 0;
}