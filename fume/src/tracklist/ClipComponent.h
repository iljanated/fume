#pragma once
#include <JuceHeader.h>

namespace te = tracktion;

class ClipComponent : public juce::Component
{
public:
    //==============================================================================
    ClipComponent();

    ~ClipComponent() override;

    //==============================================================================

    virtual te::Clip& getClip() = 0;

private:
    //==============================================================================

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ClipComponent)
};
