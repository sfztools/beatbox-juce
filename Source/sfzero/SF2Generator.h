/*************************************************************************************
 * Original code copyright (C) 2012 Steve Folta
 * Converted to Juce module (C) 2016 Leo Olivers
 * Forked from https://github.com/stevefolta/SFZero
 * For license info please see the LICENSE file distributed with this source code
 *************************************************************************************/
#ifndef SF2GENERATOR_H_INCLUDED
#define SF2GENERATOR_H_INCLUDED

#include "SFZCommon.h"

#define SF2GeneratorValue(name, type) name

namespace sfzero
{
    
    struct SF2Generator
    {
        enum Type
        {
            Word,
            Short,
            Range
        };
        
        const char *name;
        Type type;
        
        enum
        {
#include "sf2-chunks/generators.h"
        };
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SF2Generator)
    };
    
    const SF2Generator *GeneratorFor(int index);
    
#undef SF2GeneratorValue
}

#endif // SF2GENERATOR_H_INCLUDED
