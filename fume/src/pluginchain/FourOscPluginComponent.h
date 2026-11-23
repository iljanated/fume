#pragma once

#include <JuceHeader.h>
#include "InputComponents.h"

namespace te = tracktion;

class FourOscPluginComponent : public juce::Component
{
public:
	FourOscPluginComponent(te::FourOscPlugin* plugin)
	{
		AmpAttackSlider.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
		AmpAttackSlider.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
		addAndMakeVisible(AmpAttackSlider);

		AmpDecaySlider.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
		addAndMakeVisible(AmpDecaySlider);

		AmpSustainSlider.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
		addAndMakeVisible(AmpSustainSlider);

		AmpReleaseSlider.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
		addAndMakeVisible(AmpReleaseSlider);
	}

	~FourOscPluginComponent() override
	{
	}

	void paint(juce::Graphics& g) override
	{
		g.fillAll(juce::Colours::black);
		g.setColour(juce::Colours::white);
		g.drawText("FourOscPluginComponent", getLocalBounds(), juce::Justification::centred, true);
	}

	void resized() override
	{
		juce::FlexBox fb;
		fb.flexDirection = juce::FlexBox::Direction::row;
		fb.items.add(juce::FlexItem(AmpAttackSlider).withFlex(1.0f));
		fb.items.add(juce::FlexItem(AmpDecaySlider).withFlex(1.0f));
		fb.items.add(juce::FlexItem(AmpSustainSlider).withFlex(1.0f));
		fb.items.add(juce::FlexItem(AmpReleaseSlider).withFlex(1.0f));
		fb.performLayout(getLocalBounds());
	}

private:
	fumeUI::KeyboardSlider AmpAttackSlider{ "Amp Attack", nullptr };
	fumeUI::KeyboardSlider AmpDecaySlider{ "Amp Decay", nullptr };
	fumeUI::KeyboardSlider AmpSustainSlider{ "Amp Sustain", nullptr };
	fumeUI::KeyboardSlider AmpReleaseSlider{ "Amp Release", nullptr };

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FourOscPluginComponent)
};