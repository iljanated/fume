#pragma once

#include <JuceHeader.h>
#include "UIContext.h"

namespace te = tracktion;

class PluginChainComponent : public juce::Component, public juce::ChangeListener, public juce::ValueTree::Listener
{
public:
	PluginChainComponent(fumeUI::UIContext& uiContext);
	~PluginChainComponent() override;
	void paint(juce::Graphics& g) override;
	void resized() override;

	void focusGained(FocusChangeType cause) override;

	void valueTreeParentChanged(juce::ValueTree& treeWhoseParentHasChanged) override;
	void changeListenerCallback(juce::ChangeBroadcaster* source) override;

private:
	fumeUI::UIContext& uiContext;
	juce::ValueTree currentTrackState;
	std::vector<std::unique_ptr<juce::Component>> pluginComponents;

	void clearPluginComponents();

	void populatePluginComponentsForTrack(te::Track& track);

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginChainComponent)
};