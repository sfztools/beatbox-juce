#include "../JuceLibraryCode/JuceHeader.h"
#include "catch2/catch.hpp"
#include "../Source/BeatDescription.h"
#include <functional>
#include <numeric>
#include <algorithm>
#include <random>
using namespace Catch::literals;


TEST_CASE("BeatDescription construction", "[BeatDescription]")
{
    const auto currentDirectory = File::getSpecialLocation(File::SpecialLocationType::invokedExecutableFile).getParentDirectory();
    const auto testFilesDirectory = currentDirectory.getChildFile("TestFiles");

    SECTION("Empty")
    {
        BeatDescription description(testFilesDirectory.getChildFile("nonexistentfile.json"));

        REQUIRE( description.isEmpty() );
    }

    SECTION("Build from JSON file")
    {
        BeatDescription description(testFilesDirectory.getChildFile("minimal.json"));

        REQUIRE( !description.isEmpty() );
        REQUIRE( description.getNumberOfParts() == 1 );
        REQUIRE( description.getBeatName() == "minimal" );
    }

    SECTION("Build from Zip file")
    {
        BeatDescription description(testFilesDirectory.getChildFile("minimal.zip"));

        REQUIRE( !description.isEmpty() );
        REQUIRE( description.getNumberOfParts() == 1 );
        REQUIRE( description.getBeatName() == "minimal" );
    }

    SECTION("Build from JSON file with subdirectories")
    {
        BeatDescription description(testFilesDirectory.getChildFile("subdirectories.json"));

        REQUIRE( !description.isEmpty() );
        REQUIRE( description.getNumberOfParts() == 1 );
        REQUIRE( description.getBeatName() == "subdirectories" );
    }

    SECTION("Build from Zip file with subdirectories")
    {
        BeatDescription description(testFilesDirectory.getChildFile("subdirectories.zip"));

        REQUIRE( !description.isEmpty() );
        REQUIRE( description.getNumberOfParts() == 1 );
        REQUIRE( description.getBeatName() == "subdirectories" );
    }

    SECTION("Build a full example")
    {
        BeatDescription description(testFilesDirectory.getChildFile("full.json"));

        REQUIRE( !description.isEmpty() );
        REQUIRE( description.getNumberOfParts() == 2 );
        REQUIRE( description.getBeatName() == "full" );
        REQUIRE( description.hasIntro() );
        REQUIRE( description.hasEnding() );
        REQUIRE( description.getQuartersPerBar() == 3 );
        REQUIRE( description.getSecondsPerQuarter() == 0.6_a );
        REQUIRE( description.getNumberOfFills() == 3 );
        description.getNextPart();
        REQUIRE( description.getNumberOfFills() == 2 );

    }
}