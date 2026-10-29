#pragma once
#include <JuceHeader.h>
#include "UIContext.h"

namespace te = tracktion;

class TransportComponent : public juce::Component,
	public juce::Button::Listener
{
public:
    //==============================================================================
    TransportComponent(fumeUI::UIContext& c);

    ~TransportComponent() override;

    //==============================================================================
    void paint(juce::Graphics& g) override;

    void resized() override;

private:
    //==============================================================================
    fumeUI::UIContext& uiContext;
	juce::TextButton playButton{ "Play" };

    void buttonClicked(juce::Button* button) override;

    void togglePlay();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TransportComponent)
};
