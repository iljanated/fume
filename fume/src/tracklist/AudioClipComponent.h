#pragma once
#include <JuceHeader.h>
#include "ClipComponent.h"

namespace te = tracktion;

class AudioClipComponent : public ClipComponent
{
public:
    //==============================================================================
    AudioClipComponent(te::WaveAudioClip& c);

    ~AudioClipComponent() override;

    te::Clip& getClip() override;

private:
    //==============================================================================
    te::WaveAudioClip& clip;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioClipComponent)
};
