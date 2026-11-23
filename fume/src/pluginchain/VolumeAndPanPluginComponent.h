#pragma once

#include <JuceHeader.h>

namespace te = tracktion;

class VolumeAndPanPluginComponent : public juce::Component
{
public:
	VolumeAndPanPluginComponent() {}
	~VolumeAndPanPluginComponent() override {}
	void paint(juce::Graphics& g) override
	{
		g.fillAll(juce::Colours::black);

		g.setColour(juce::Colours::white);
		g.drawText("VolumeAndPanPluginComponent", getLocalBounds(), juce::Justification::centred, true);
	}
	void resized() override {}
private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(VolumeAndPanPluginComponent)
};