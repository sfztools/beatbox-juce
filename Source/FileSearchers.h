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

#include "../JuceLibraryCode/JuceHeader.h"

/**
 * Interface that allows to search within some file container (e.g. a directory or a zip file)
 */
class FileSearcher
{
public:
    virtual std::unique_ptr<InputStream> createFileStream(const String& filename) const = 0;
};

class ZipFileSearcher: public FileSearcher
{
public:
    ZipFileSearcher() = delete;
    ZipFileSearcher(const File& file);
    std::unique_ptr<InputStream> createFileStream(const String& filename) const override;
private:
    std::unique_ptr<ZipFile> zipFile { };
    String zipFilename;
    JUCE_LEAK_DETECTOR(ZipFileSearcher);
};

class NormalFileSearcher: public FileSearcher
{
public:
    NormalFileSearcher() = delete;
    NormalFileSearcher(const File& file);
    std::unique_ptr<InputStream> createFileStream(const String& filename) const override;
private:
    std::unique_ptr<File> rootDirectory{};
    JUCE_LEAK_DETECTOR(NormalFileSearcher);
};