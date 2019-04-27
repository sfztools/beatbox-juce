/*************************************************************************************
 * Original code copyright (C) 2012 Steve Folta
 * Converted to Juce module (C) 2016 Leo Olivers
 * Forked from https://github.com/stevefolta/SFZero
 * For license info please see the LICENSE file distributed with this source code
 *************************************************************************************/
#ifndef SF2_H_INCLUDED
#define SF2_H_INCLUDED

#include "SF2WinTypes.h"
#include <vector>

namespace sfzero
{
    namespace SF2
    {
        struct rangesType
        {
            byte lo, hi;
        };
        
        union genAmountType {
            rangesType range;
            short shortAmount;
            word wordAmount;
        };
        
        struct iver
        {
            word major;
            word minor;
            void readFrom(juce::InputStream& file);
            JUCE_LEAK_DETECTOR(iver)
        };
        
        struct phdr
        {
            char20 presetName;
            word preset;
            word bank;
            word presetBagNdx;
            dword library;
            dword genre;
            dword morphology;
            void readFrom(juce::InputStream& file);
            
            static const int sizeInFile = 38;
            JUCE_LEAK_DETECTOR(phdr)
        };
        
        struct pbag
        {
            word genNdx;
            word modNdx;
            void readFrom(juce::InputStream& file);
            
            static const int sizeInFile = 4;
            JUCE_LEAK_DETECTOR(pbag)
        };
        
        struct pmod
        {
            word modSrcOper;
            word modDestOper;
            short modAmount;
            word modAmtSrcOper;
            word modTransOper;
            void readFrom(juce::InputStream& file);
            
            static const int sizeInFile = 10;
            JUCE_LEAK_DETECTOR(pmod)
        };
        
        struct pgen
        {
            word genOper;
            genAmountType genAmount;

            void readFrom(juce::InputStream& file);
            
            static const int sizeInFile = 4;
            JUCE_LEAK_DETECTOR(pgen)
        };
        
        struct inst
        {
            char20 instName;
            word instBagNdx;
            void readFrom(juce::InputStream& file);
            
            static const int sizeInFile = 22;
            JUCE_LEAK_DETECTOR(inst)
        };
        
        struct ibag
        {
            word instGenNdx;
            word instModNdx;

            void readFrom(juce::InputStream& file);
            
            static const int sizeInFile = 4;
            JUCE_LEAK_DETECTOR(ibag)
        };
        
        struct imod
        {
            word modSrcOper;
            word modDestOper;
            short modAmount;
            word modAmtSrcOper;
            word modTransOper;

            void readFrom(juce::InputStream& file);
            
            static const int sizeInFile = 10;
            JUCE_LEAK_DETECTOR(imod)
        };
        
        struct igen
        {
            word genOper;
            genAmountType genAmount;
            void readFrom(juce::InputStream& file);
            
            static const int sizeInFile = 4;
            JUCE_LEAK_DETECTOR(igen)
        };
        
        struct shdr
        {
            char20 sampleName;
            dword start;
            dword end;
            dword startLoop;
            dword endLoop;
            dword sampleRate;
            byte originalPitch;
            char pitchCorrection;
            word sampleLink;
            word sampleType;

            void readFrom(juce::InputStream& file);
            
            static const int sizeInFile = 46;
            JUCE_LEAK_DETECTOR(shdr)
        };
        
        struct Hydra
        {
            std::vector<phdr> presetHeaderList;
            std::vector<pbag> pbagItems;
            std::vector<pmod> pmodItems;
            std::vector<pgen> pgenItems;
            std::vector<inst> instItems;
            std::vector<ibag> ibagItems;
            std::vector<imod> imodItems;
            std::vector<igen> igenItems;
            std::vector<shdr> shdrItems;
            
            template<typename T>
            void readHelper(const sfzero::RIFFChunk& chunk,
                            std::vector<T>& chunkItems,
                            juce::InputStream& file)
            {
                int numItems = (int)chunk.size / T::sizeInFile;
                for( int i = 0; i < numItems; ++i )
                {
                    T t;
                    t.readFrom(file);
                    chunkItems.push_back(t);
                }
            }
            
            
            Hydra() {}
            ~Hydra() {}
            
            void readFrom(juce::InputStream& file, juce::int64 pdtaChunkEnd);
            bool isComplete();
            JUCE_LEAK_DETECTOR(Hydra)
        };
    }
}

#undef SF2Field

#endif // SF2_H_INCLUDED
