#pragma once
#include <JuceHeader.h>
#include "InputManager.h"

namespace te = tracktion;

class TransportComponent : public juce::Component,
	public juce::Button::Listener,
    public InputManagerListener
{
public:
    //==============================================================================
    TransportComponent(te::TransportControl& t, InputManager& i);

    ~TransportComponent() override;

    //==============================================================================
    void paint(juce::Graphics& g) override;

    void resized() override;

	void onInputAction(InputManagerAction action, int inputMask, bool isActive) override;

private:
    //==============================================================================
    te::TransportControl& transport;
    InputManager& inputManager;

	juce::TextButton playButton{ "Play" };

    void buttonClicked(juce::Button* button) override;

    void togglePlay(bool fromStart);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TransportComponent)
};
