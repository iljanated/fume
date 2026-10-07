#pragma once
#include <JuceHeader.h>
#include "ClipComponent.h"

namespace te = tracktion;

class MidiClipComponent : public ClipComponent
{
public:
    //==============================================================================
    MidiClipComponent(te::MidiClip& c);

    ~MidiClipComponent() override;

    void paint(juce::Graphics& g) override;

    void resized() override;

    te::Clip& getClip() override;
    
private:
    //==============================================================================
    te::MidiClip& clip;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MidiClipComponent)
};
