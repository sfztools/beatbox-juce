/*************************************************************************************
 * Original code copyright (C) 2012 Steve Folta
 * Converted to Juce module (C) 2016 Leo Olivers
 * Forked from https://github.com/stevefolta/SFZero
 * For license info please see the LICENSE file distributed with this source code
 *************************************************************************************/
#include "SF2Reader.h"
#include "RIFF.h"
#include "SF2.h"
#include "SF2Generator.h"
#include "SF2Sound.h"
#include <memory>

sfzero::SF2Reader::SF2Reader(sfzero::SF2Sound& soundIn, const juce::File &fileIn) :
sf2Sound(soundIn),
fileInputStream( fileIn.createInputStream() )
{
//    fileInputStream = fileIn.createInputStream();
}

//sfzero::SF2Reader::~SF2Reader() { delete fileInputStream_; }

void sfzero::SF2Reader::read()
{
    if (fileInputStream == nullptr)
    {
        sf2Sound.addError("Couldn't open file.");
        return;
    }
    
    fileInputStream->setPosition(0);
    sfzero::RIFFChunk riffChunk;
    riffChunk.readFrom(*fileInputStream);
    
    // Read the hydra.
    sfzero::SF2::Hydra hydra;
    while (fileInputStream->getPosition() < riffChunk.end())
    {
        sfzero::RIFFChunk chunk;
        chunk.readFrom(*fileInputStream);
        if (FourCCEquals(chunk.id, "pdta"))
        {
            hydra.readFrom(*fileInputStream, chunk.end());
            break;
        }
        chunk.seekAfter(*fileInputStream);
    }
    if (!hydra.isComplete())
    {
        sf2Sound.addError("Invalid SF2 file (missing or incomplete hydra).");
        return;
    }
    
    // Read each preset.
    //for (int whichPreset = 0; whichPreset < hydra.phdrNumItems - 1; ++whichPreset)
    for( int whichPreset = 0; whichPreset < hydra.presetHeaderList.size() - 1; ++whichPreset)
    {
        sfzero::SF2::phdr *presetHeader = &hydra.presetHeaderList[whichPreset];
        //sfzero::SF2Sound::Preset *preset = new sfzero::SF2Sound::Preset(phdr->presetName, phdr->bank, phdr->preset);
        auto preset = std::make_unique<sfzero::SF2Sound::Preset>(presetHeader->presetName,
                                                                 presetHeader->bank,
                                                                 presetHeader->preset);
        
        // Zones.
        //*** TODO: Handle global zone (modulators only).
        int zoneEnd = hydra.presetHeaderList[whichPreset+1].presetBagNdx;
        for (int whichZone = presetHeader->presetBagNdx; whichZone < zoneEnd; ++whichZone)
        {
            sfzero::SF2::pbag *presetZone = &hydra.pbagItems[whichZone];
            sfzero::Region presetRegion;
            presetRegion.clearForRelativeSF2();
            
            // Generators.
            int genEnd = hydra.pbagItems[whichZone+1].genNdx;
            for (int whichGen = presetZone->genNdx; whichGen < genEnd; ++whichGen)
            {
                sfzero::SF2::pgen *presetGenerator = &hydra.pgenItems[whichGen];
                
                // Instrument.
                if (presetGenerator->genOper == sfzero::SF2Generator::instrument)
                {
                    sfzero::word whichInst = presetGenerator->genAmount.wordAmount;
                    //if (whichInst < hydra.instNumItems)
                    if( whichInst < hydra.instItems.size() )
                    {
                        sfzero::Region instRegion;
                        instRegion.clearForSF2();
                        // Preset generators are supposed to be "relative" modifications of
                        // the instrument settings, but that makes no sense for ranges.
                        // For those, we'll have the instrument's generator take
                        // precedence, though that may not be correct.
                        instRegion.lokey = presetRegion.lokey;
                        instRegion.hikey = presetRegion.hikey;
                        instRegion.lovel = presetRegion.lovel;
                        instRegion.hivel = presetRegion.hivel;
                        
                        sfzero::SF2::inst *inst = &hydra.instItems[whichInst];
                        int firstZone = inst->instBagNdx;
                        int zoneEnd2 = inst[1].instBagNdx;
                        for (int whichZone2 = firstZone; whichZone2 < zoneEnd2; ++whichZone2)
                        {
                            sfzero::SF2::ibag *ibag = &hydra.ibagItems[whichZone2];
                            
                            // Generators.
                            sfzero::Region zoneRegion = instRegion;
                            bool hadSampleID = false;
                            int genEnd2 = ibag[1].instGenNdx;
                            for (int whichGen2 = ibag->instGenNdx; whichGen2 < genEnd2; ++whichGen2)
                            {
                                sfzero::SF2::igen *igen = &hydra.igenItems[whichGen2];
                                if (igen->genOper == sfzero::SF2Generator::sampleID)
                                {
                                    int whichSample = igen->genAmount.wordAmount;
                                    sfzero::SF2::shdr *shdr = &hydra.shdrItems[whichSample];
                                    zoneRegion.addForSF2(&presetRegion);
                                    zoneRegion.sf2ToSFZ();
                                    zoneRegion.offset += shdr->start;
                                    zoneRegion.end += shdr->end;
                                    zoneRegion.loop_start += shdr->startLoop;
                                    zoneRegion.loop_end += shdr->endLoop;
                                    if (shdr->endLoop > 0)
                                    {
                                        zoneRegion.loop_end -= 1;
                                    }
                                    if (zoneRegion.pitch_keycenter == -1)
                                    {
                                        zoneRegion.pitch_keycenter = shdr->originalPitch;
                                    }
                                    zoneRegion.tune += shdr->pitchCorrection;
                                    
                                    // Pin initialAttenuation to max +6dB.
                                    if (zoneRegion.volume > 6.0)
                                    {
                                        zoneRegion.volume = 6.0;
                                        sf2Sound.addUnsupportedOpcode("extreme gain in initialAttenuation");
                                    }
                                    
                                    //sfzero::Region *newRegion = new sfzero::Region();
                                    auto newRegion = std::make_unique<sfzero::Region>();
                                    *newRegion = zoneRegion;
                                    newRegion->sample = sf2Sound.sampleFor(shdr->sampleRate).get();
                                    preset->addRegion(std::move(newRegion));
                                    hadSampleID = true;
                                }
                                else
                                {
                                    addGeneratorToRegion(igen->genOper, &igen->genAmount, &zoneRegion);
                                }
                            }
                            
                            // Handle instrument's global zone.
                            if ((whichZone2 == firstZone) && !hadSampleID)
                            {
                                instRegion = zoneRegion;
                            }
                            
                            // Modulators.
                            int modEnd = ibag[1].instModNdx;
                            int whichMod = ibag->instModNdx;
                            if (whichMod < modEnd)
                            {
                                sf2Sound.addUnsupportedOpcode("any modulator");
                            }
                        }
                    }
                    else
                    {
                        sf2Sound.addError("Instrument out of range.");
                    }
                }
                // Other generators.
                else
                {
                    addGeneratorToRegion(presetGenerator->genOper, &presetGenerator->genAmount, &presetRegion);
                }
            }
            
            // Modulators.
            int modEnd = presetZone[1].modNdx;
            int whichMod = presetZone->modNdx;
            if (whichMod < modEnd)
            {
                sf2Sound.addUnsupportedOpcode("any modulator");
            }
        }
        sf2Sound.addPreset(std::move(preset));
    }
}

std::shared_ptr<juce::AudioSampleBuffer> sfzero::SF2Reader::readSamples(double *progressVar, juce::Thread *thread)
{
    static const int bufferSize = 32768;
    
    if (fileInputStream == nullptr)
    {
        sf2Sound.addError("Couldn't open file.");
        return nullptr;
    }
    
    // Find the "sdta" chunk.
    fileInputStream->setPosition(0);
    sfzero::RIFFChunk riffChunk;
    riffChunk.readFrom(*fileInputStream);
    bool found = false;
    sfzero::RIFFChunk chunk;
    while (fileInputStream->getPosition() < riffChunk.end())
    {
        chunk.readFrom(*fileInputStream);
        if (FourCCEquals(chunk.id, "sdta"))
        {
            found = true;
            break;
        }
        chunk.seekAfter(*fileInputStream);
    }
    juce::int64 sdtaEnd = chunk.end();
    found = false;
    while (fileInputStream->getPosition() < sdtaEnd)
    {
        chunk.readFrom(*fileInputStream);
        if (FourCCEquals(chunk.id, "smpl"))
        {
            found = true;
            break;
        }
        chunk.seekAfter(*fileInputStream);
    }
    if (!found)
    {
        sf2Sound.addError("SF2 is missing its \"smpl\" chunk.");
        return nullptr;
    }
    
    // Allocate the AudioSampleBuffer.
    int numSamples = (int)chunk.size / sizeof(short);
    //juce::AudioSampleBuffer *sampleBuffer = new juce::AudioSampleBuffer(1, numSamples);
    std::shared_ptr<juce::AudioSampleBuffer> sampleBuffer = std::make_shared<juce::AudioSampleBuffer>(1, numSamples);
    
    // Read and convert.
    //short *buffer = new short[bufferSize];
    juce::HeapBlock<short> buffer(bufferSize);
    int samplesLeft = numSamples;
    float *out = sampleBuffer->getWritePointer(0);
    while (samplesLeft > 0)
    {
        // Read <= 32768 bytes at a time from the buffer
        int samplesToRead = bufferSize;
        if (samplesToRead > samplesLeft)
        {
            samplesToRead = samplesLeft;
        }
        fileInputStream->read(buffer.getData(), samplesToRead * sizeof(short));
        
        // Convert from signed 16-bit to float.
        int samplesToConvert = samplesToRead;
        short *in = buffer.getData();
        for (; samplesToConvert > 0; --samplesToConvert)
        {
            // If we ever need to compile for big-endian platforms, we'll need to
            // byte-swap here.
            *out++ = *in++ / 32767.0f;

        }
        
        samplesLeft -= samplesToRead;
        
        if (progressVar)
        {
            *progressVar = static_cast<float>(numSamples - samplesLeft) / numSamples;
        }
        if (thread && thread->threadShouldExit())
        {
            //delete[] buffer;
            //delete sampleBuffer;
            sampleBuffer.reset();
            return nullptr;
        }
    }
    //delete[] buffer;
    
    if (progressVar)
    {
        *progressVar = 1.0;
    }
    
    return sampleBuffer;
}

void sfzero::SF2Reader::addGeneratorToRegion(sfzero::word genOper, sfzero::SF2::genAmountType *amount, sfzero::Region *region)
{
    switch (genOper)
    {
        case sfzero::SF2Generator::startAddrsOffset:
            region->offset += amount->shortAmount;
            break;
            
        case sfzero::SF2Generator::endAddrsOffset:
            region->end += amount->shortAmount;
            break;
            
        case sfzero::SF2Generator::startloopAddrsOffset:
            region->loop_start += amount->shortAmount;
            break;
            
        case sfzero::SF2Generator::endloopAddrsOffset:
            region->loop_end += amount->shortAmount;
            break;
            
        case sfzero::SF2Generator::startAddrsCoarseOffset:
            region->offset += amount->shortAmount * 32768;
            break;
            
        case sfzero::SF2Generator::endAddrsCoarseOffset:
            region->end += amount->shortAmount * 32768;
            break;
            
        case sfzero::SF2Generator::pan:
            region->pan = amount->shortAmount * (2.0f / 10.0f);
            break;
            
        case sfzero::SF2Generator::delayVolEnv:
            region->ampeg.delay = amount->shortAmount;
            break;
            
        case sfzero::SF2Generator::attackVolEnv:
            region->ampeg.attack = amount->shortAmount;
            break;
            
        case sfzero::SF2Generator::holdVolEnv:
            region->ampeg.hold = amount->shortAmount;
            break;
            
        case sfzero::SF2Generator::decayVolEnv:
            region->ampeg.decay = amount->shortAmount;
            break;
            
        case sfzero::SF2Generator::sustainVolEnv:
            region->ampeg.sustain = amount->shortAmount;
            break;
            
        case sfzero::SF2Generator::releaseVolEnv:
            region->ampeg.release = amount->shortAmount;
            break;
            
        case sfzero::SF2Generator::keyRange:
            region->lokey = amount->range.lo;
            region->hikey = amount->range.hi;
            break;
            
        case sfzero::SF2Generator::velRange:
            region->lovel = amount->range.lo;
            region->hivel = amount->range.hi;
            break;
            
        case sfzero::SF2Generator::startloopAddrsCoarseOffset:
            region->loop_start += amount->shortAmount * 32768;
            break;
            
        case sfzero::SF2Generator::initialAttenuation:
            // The spec says "initialAttenuation" is in centibels.  But everyone
            // seems to treat it as millibels.
            region->volume += -amount->shortAmount / 100.0f;
            break;
            
        case sfzero::SF2Generator::endloopAddrsCoarseOffset:
            region->loop_end += amount->shortAmount * 32768;
            break;
            
        case sfzero::SF2Generator::coarseTune:
            region->transpose += amount->shortAmount;
            break;
            
        case sfzero::SF2Generator::fineTune:
            region->tune += amount->shortAmount;
            break;
            
        case sfzero::SF2Generator::sampleModes:
        {
            sfzero::Region::LoopMode loopModes[] = {sfzero::Region::no_loop, sfzero::Region::loop_continuous, sfzero::Region::no_loop,
                sfzero::Region::loop_sustain};
            region->loop_mode = loopModes[amount->wordAmount & 0x03];
        }
            break;
            
        case sfzero::SF2Generator::scaleTuning:
            region->pitch_keytrack = amount->shortAmount;
            break;
            
        case sfzero::SF2Generator::exclusiveClass:
            region->off_by = amount->wordAmount;
            region->group = static_cast<int>(region->off_by);
            break;
            
        case sfzero::SF2Generator::overridingRootKey:
            region->pitch_keycenter = amount->shortAmount;
            break;
            
        case sfzero::SF2Generator::endOper:
            // Ignore.
            break;
            
        case sfzero::SF2Generator::modLfoToPitch:
        case sfzero::SF2Generator::vibLfoToPitch:
        case sfzero::SF2Generator::modEnvToPitch:
        case sfzero::SF2Generator::initialFilterFc:
        case sfzero::SF2Generator::initialFilterQ:
        case sfzero::SF2Generator::modLfoToFilterFc:
        case sfzero::SF2Generator::modEnvToFilterFc:
        case sfzero::SF2Generator::modLfoToVolume:
        case sfzero::SF2Generator::unused1:
        case sfzero::SF2Generator::chorusEffectsSend:
        case sfzero::SF2Generator::reverbEffectsSend:
        case sfzero::SF2Generator::unused2:
        case sfzero::SF2Generator::unused3:
        case sfzero::SF2Generator::unused4:
        case sfzero::SF2Generator::delayModLFO:
        case sfzero::SF2Generator::freqModLFO:
        case sfzero::SF2Generator::delayVibLFO:
        case sfzero::SF2Generator::freqVibLFO:
        case sfzero::SF2Generator::delayModEnv:
        case sfzero::SF2Generator::attackModEnv:
        case sfzero::SF2Generator::holdModEnv:
        case sfzero::SF2Generator::decayModEnv:
        case sfzero::SF2Generator::sustainModEnv:
        case sfzero::SF2Generator::releaseModEnv:
        case sfzero::SF2Generator::keynumToModEnvHold:
        case sfzero::SF2Generator::keynumToModEnvDecay:
        case sfzero::SF2Generator::keynumToVolEnvHold:
        case sfzero::SF2Generator::keynumToVolEnvDecay:
        case sfzero::SF2Generator::instrument:
            // Only allowed in certain places, where we already special-case it.
        case sfzero::SF2Generator::reserved1:
        case sfzero::SF2Generator::keynum:
        case sfzero::SF2Generator::velocity:
        case sfzero::SF2Generator::reserved2:
        case sfzero::SF2Generator::sampleID:
            // Only allowed in certain places, where we already special-case it.
        case sfzero::SF2Generator::reserved3:
        case sfzero::SF2Generator::unused5:
        {
            const sfzero::SF2Generator *generator = sfzero::GeneratorFor(static_cast<int>(genOper));
            sf2Sound.addUnsupportedOpcode(generator->name);
        }
            break;
    }
}
