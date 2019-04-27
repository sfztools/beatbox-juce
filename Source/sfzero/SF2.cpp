/*************************************************************************************
 * Original code copyright (C) 2012 Steve Folta
 * Converted to Juce module (C) 2016 Leo Olivers
 * Forked from https://github.com/stevefolta/SFZero
 * For license info please see the LICENSE file distributed with this source code
 *************************************************************************************/
#include "SF2.h"
#include "RIFF.h"

void sfzero::SF2::iver::readFrom(juce::InputStream& file)
{
    major = (word)file.readShort();
    minor = (word)file.readShort();
}

void sfzero::SF2::phdr::readFrom(juce::InputStream& file)
{
    file.read(presetName, 20);
    preset = (word)file.readShort();
    bank = (word)file.readShort();
    presetBagNdx = (word)file.readShort();
    library = (dword)file.readInt();
    genre = (dword)file.readInt();
    morphology = (dword)file.readInt();
}

void sfzero::SF2::pbag::readFrom(juce::InputStream& file)
{
    genNdx = (word)file.readShort();
    modNdx = (word)file.readShort();
}

void sfzero::SF2::pmod::readFrom(juce::InputStream& file)
{
    modSrcOper = (word)file.readShort();
    modDestOper = (word)file.readShort();
    modAmount = file.readShort();
    modAmtSrcOper = (word)file.readShort();
    modTransOper = (word)file.readShort();
}

void sfzero::SF2::pgen::readFrom(juce::InputStream& file)
{
    genOper = (word)file.readShort();
    genAmount.shortAmount = file.readShort();
}

void sfzero::SF2::inst::readFrom(juce::InputStream& file)
{
    file.read(instName, 20);
    instBagNdx = (word)file.readShort();
}

void sfzero::SF2::ibag::readFrom(juce::InputStream& file)
{
    instGenNdx = (word)file.readShort();
    instModNdx = (word)file.readShort();
}

void sfzero::SF2::imod::readFrom(juce::InputStream& file)
{
    modSrcOper = (word)file.readShort();
    modDestOper = (word)file.readShort();
    modAmount = file.readShort();
    modAmtSrcOper = (word)file.readShort();
    modTransOper = (word)file.readShort();
}

void sfzero::SF2::igen::readFrom(juce::InputStream& file)
{
    genOper = (word)file.readShort();
    genAmount.shortAmount = file.readShort();
}

void sfzero::SF2::shdr::readFrom(juce::InputStream& file)
{
    file.read(sampleName, 20);
    start = (dword)file.readInt();
    end = (dword)file.readInt();
    startLoop = (dword)file.readInt();
    endLoop = (dword)file.readInt();
    sampleRate = (dword)file.readInt();
    originalPitch = (byte)file.readByte();
    pitchCorrection = file.readByte();
    sampleLink = (word)file.readShort();
    sampleType = (word)file.readShort();
}

void sfzero::SF2::Hydra::readFrom(juce::InputStream& file, juce::int64 pdtaChunkEnd)
{
    auto check = [](sfzero::RIFFChunk& chunk, sfzero::fourcc chunkName)
    {
        sfzero::fourcc& chunkID = chunk.id;
        return
        chunkID[0] == chunkName[0] &&
        chunkID[1] == chunkName[1] &&
        chunkID[2] == chunkName[2] &&
        chunkID[3] == chunkName[3];
    };
    
    sfzero::fourcc phdrType = {'p','h','d','r'};
    sfzero::fourcc pbagType = {'p','b','a','g'};
    sfzero::fourcc pmodType = {'p','m','o','d'};
    sfzero::fourcc pgenType = {'p','g','e','n'};
    sfzero::fourcc instType = {'i','n','s','t'};
    sfzero::fourcc ibagType = {'i','b','a','g'};
    sfzero::fourcc imodType = {'i','m','o','d'};
    sfzero::fourcc igenType = {'i','g','e','n'};
    sfzero::fourcc shdrType = {'s','h','d','r'};
    
    while( file.getPosition() < pdtaChunkEnd )
    {
        sfzero::RIFFChunk chunk;
        chunk.readFrom(file);
        
        if( check(chunk, phdrType ) )
        {
            readHelper(chunk, presetHeaderList, file);
        }
        else if( check(chunk, pbagType ) )
        {
            readHelper(chunk, pbagItems, file);
        }
        else if( check(chunk, pmodType ) )
        {
            readHelper(chunk, pmodItems, file);
        }
        else if( check(chunk, pgenType ) )
        {
            readHelper(chunk, pgenItems, file);
        }
        else if( check(chunk, instType) )
        {
            readHelper(chunk, instItems, file);
        }
        else if( check(chunk, ibagType ) )
        {
            readHelper(chunk, ibagItems, file);
        }
        else if( check(chunk, imodType) )
        {
            readHelper(chunk, imodItems, file);
        }
        else if( check(chunk, igenType ) )
        {
            readHelper(chunk, igenItems, file);
        }
        else if( check(chunk, shdrType ) )
        {
            readHelper(chunk, shdrItems, file);
        }
        
        chunk.seekAfter(file);
    }
}

bool sfzero::SF2::Hydra::isComplete()
{
    return
    !presetHeaderList.empty() &&
    !pbagItems.empty() &&
    !pmodItems.empty() &&
    !pgenItems.empty() &&
    !instItems.empty() &&
    !ibagItems.empty() &&
    !imodItems.empty() &&
    !igenItems.empty() &&
    !shdrItems.empty();
}
