#pragma once
#include <JuceHeader.h>
#include "ClipComponent.h"
#include "Utilities.h"

namespace te = tracktion;

class MidiClipComponent : public ClipComponent
{
public:
    //==============================================================================
    MidiClipComponent(te::MidiClip& c, fumeUI::UIContext& context);

    ~MidiClipComponent() override;

    void paint(juce::Graphics& g) override;

    void resized() override;

    te::Clip& getClip() override;
    
private:
    //==============================================================================
    fumeUI::UIContext& uiContext;
    //==============================================================================
    te::MidiClip& clip;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MidiClipComponent)
};
