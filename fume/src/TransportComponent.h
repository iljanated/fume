#pragma once
#include <JuceHeader.h>
#include "Utilities.h"

namespace te = tracktion;

class TransportComponent : public juce::Component,
	public juce::Button::Listener,
    public InputManagerListener
{
public:
    //==============================================================================
    TransportComponent(fumeUI::UIContext& c);

    ~TransportComponent() override;

    //==============================================================================
    void paint(juce::Graphics& g) override;

    void resized() override;

	void onInputAction(InputManagerActionId actionId, int inputMask, bool isActive) override;

private:
    //==============================================================================
    fumeUI::UIContext& uiContext;
	juce::TextButton playButton{ "Play" };

    void buttonClicked(juce::Button* button) override;

	void startPlay(bool fromStart);
    void togglePlay();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TransportComponent)
};
