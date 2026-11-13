#pragma once
#include <JuceHeader.h>
#include "UIContext.h"
#include "Utilities.h"

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

	void focusGained(FocusChangeType cause) override;
	bool keyPressed(const juce::KeyPress& key) override;

    std::unique_ptr<juce::ComponentTraverser> createKeyboardFocusTraverser() override
    {
        return std::make_unique<fumeUI::WrappingFocusTraverser>();
    }

private:
    //==============================================================================
    fumeUI::UIContext& uiContext;
    juce::TextButton recordButton{ "Record" };
    juce::TextButton playButton{ "Play" };
	juce::Slider tempoSlider;
	juce::Component* currentlyFocusedComponent = nullptr;

    void buttonClicked(juce::Button* button) override;

    void togglePlay();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TransportComponent)
};
