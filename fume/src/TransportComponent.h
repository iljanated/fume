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
    TransportComponent(te::Edit& e, InputManager& i);

    ~TransportComponent() override;

    //==============================================================================
    void paint(juce::Graphics& g) override;

    void resized() override;

	void onInputAction(InputManagerActionId actionId, int inputMask, bool isActive) override;

private:
    //==============================================================================
    te::Edit& edit;
    InputManager& inputManager;

	juce::TextButton playButton{ "Play" };

    void buttonClicked(juce::Button* button) override;

	void startPlay(bool fromStart);
    void togglePlay();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TransportComponent)
};
