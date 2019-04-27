/*************************************************************************************
 * Original code copyright (C) 2012 Steve Folta
 * Converted to Juce module (C) 2016 Leo Olivers
 * Forked from https://github.com/stevefolta/SFZero
 * For license info please see the LICENSE file distributed with this source code
 *************************************************************************************/
#ifndef SF2READER_H_INCLUDED
#define SF2READER_H_INCLUDED

#include "SF2.h"
#include <memory>
namespace sfzero
{
    class SF2Sound;
    class Sample;
    struct Region;
    
    class SF2Reader
    {
    public:
        SF2Reader(SF2Sound& sound, const juce::File &file_);
        virtual ~SF2Reader() {}
        
        ///reads the file that was passed in 
        void read();
        //juce::AudioSampleBuffer* readSamples(double *progressVar = nullptr, juce::Thread *thread = nullptr);
        std::shared_ptr<juce::AudioSampleBuffer> readSamples(double* progressVar = nullptr, juce::Thread* thread = nullptr);
        
    private:
        SF2Sound& sf2Sound;
        //juce::FileInputStream *file_;
        juce::ScopedPointer<juce::FileInputStream> fileInputStream;
        
        void addGeneratorToRegion(word genOper, SF2::genAmountType *amount, Region *region);
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SF2Reader)
    };
}

#endif // SF2READER_H_INCLUDED
