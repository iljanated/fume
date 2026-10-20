#pragma once
#include <JuceHeader.h>  

class ClipEditorImplementationComponent : public juce::Component
{
public:
    virtual te::Clip* getClip() = 0;
};