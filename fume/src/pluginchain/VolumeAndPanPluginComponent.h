#pragma once

#include <JuceHeader.h>
#include "UIContext.h"

namespace te = tracktion;

class VolumeAndPanPluginComponent : public TypedPluginComponent<te::VolumeAndPanPlugin>
{
public:
	VolumeAndPanPluginComponent(te::VolumeAndPanPlugin* plugin, fumeUI::UIContext& context)
		: TypedPluginComponent(plugin, context, 100)
	{
		setSize(100, 300);
	}
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