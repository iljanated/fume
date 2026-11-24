#pragma once

#include <JuceHeader.h>
#include "UIContext.h"
#include "CardinalFocusTraverser.h"

namespace te = tracktion;

class PluginChainComponent : public juce::Component, public juce::ChangeListener, public juce::ValueTree::Listener
{
public:
	PluginChainComponent(fumeUI::UIContext& uiContext);
	~PluginChainComponent() override;
	void paint(juce::Graphics& g) override;
	void resized() override;

	void focusGained(FocusChangeType cause) override;
	bool keyPressed(const juce::KeyPress& key) override;

	void valueTreeParentChanged(juce::ValueTree& treeWhoseParentHasChanged) override;
	void changeListenerCallback(juce::ChangeBroadcaster* source) override;

private:
	fumeUI::UIContext& uiContext;
	juce::ValueTree currentTrackState;
	std::vector<std::unique_ptr<juce::Component>> pluginComponents;
	fumeUI::CardinalFocusTraverser focusTraverser;

	void clearPluginComponents();

	void populatePluginComponentsForTrack(te::Track& track);

	void focusComponent(juce::Point<int> delta);

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginChainComponent)
};