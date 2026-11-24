#pragma once

#include <JuceHeader.h>
#include "UIContext.h"

namespace te = tracktion;

class VolumeAndPanPluginComponent : public juce::Component
{
public:
	VolumeAndPanPluginComponent(fumeUI::UIContext& context, te::VolumeAndPanPlugin* plugin)
		: uiContext(context), plugin(plugin)
	{
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
	fumeUI::UIContext& uiContext;
	te::VolumeAndPanPlugin* plugin;
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(VolumeAndPanPluginComponent)
};