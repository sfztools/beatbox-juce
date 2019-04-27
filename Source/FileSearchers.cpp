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

#include "FileSearchers.h"
#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h"

NormalFileSearcher::NormalFileSearcher(const File& file)
{
    if (file.exists() && file.isDirectory())
        rootDirectory = std::make_unique<File>(file);
}

std::unique_ptr<InputStream> NormalFileSearcher::createFileStream(const String& filename) const
{
    if (rootDirectory == nullptr)
        return {};

    const auto file = rootDirectory->getChildFile(filename);
    if (!file.exists())
    {
        spdlog::error("File \"{}\" does not exist in {}", filename, rootDirectory->getFileName());
        return {};
    }

    auto fileStream = std::make_unique<FileInputStream>(file);
    if (fileStream->failedToOpen())
        return {};

    return std::move(fileStream);    
}

ZipFileSearcher::ZipFileSearcher(const File& file)
{
    if (file.exists() && file.getFileExtension() == ".zip")
    {
      zipFile = std::make_unique<ZipFile>(file);
      zipFilename = file.getFileName();
    }
}

std::unique_ptr<InputStream> ZipFileSearcher::createFileStream(const String& filename) const
{
    if (zipFile == nullptr)
      return {};
    
    auto zipEntry = zipFile->getEntry(filename);
    if (zipEntry == nullptr)
    {
      spdlog::error("File \"{}\" does not exist in {}", filename, zipFilename);
      return {};
    }
    
    return std::unique_ptr<InputStream>(zipFile->createStreamForEntry(*zipEntry));
}