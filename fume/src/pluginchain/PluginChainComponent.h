#pragma once

#include <JuceHeader.h>
#include "UIContext.h"
#include "CardinalFocusTraverser.h"
#include "Utilities.h"
#include "ManualViewport.h"

namespace te = tracktion;

class PlusComponent : public fumeUI::FlexibleComponent, public juce::Button::Listener
{
public:
	PlusComponent(fumeUI::UIContext& context, te::Track *track)
		: uiContext(context), track(track)
	{
		addButton.setHasFocusOutline(true);
		addAndMakeVisible(addButton);
		addButton.addListener(this);
	}

	~PlusComponent() override = default;

	void resized() override
	{
		addButton.setBounds(getLocalBounds().reduced(10));
	}

	void buttonClicked(juce::Button* button) override
	{
		if (button == &addButton)
		{
			if (track != nullptr)
			{
				auto& edit = uiContext.extendedEdit.getEdit();
				auto synthPlugin = dynamic_cast<te::FourOscPlugin*> (edit.getPluginCache().createNewPlugin(te::FourOscPlugin::xmlTypeName, {}).get());
				auto track = te::getAudioTracks(edit)[4];
				track->pluginList.insertPlugin(synthPlugin, 0, nullptr);
			}
		}
	}

	int getMinimumWidth() const override
	{
		return 100;
	}

	int getMinimumHeight() const override
	{
		return 100;
	}

private:
	fumeUI::UIContext& uiContext;
	juce::TextButton addButton{ "+" };
	te::Track* track;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlusComponent)
};

class PluginChainComponent : public juce::Component, public juce::ChangeListener, public juce::ValueTree::Listener, private juce::FocusChangeListener
{
public:
	PluginChainComponent(fumeUI::UIContext& uiContext);
	~PluginChainComponent() override;
	void paint(juce::Graphics& g) override;
	void resized() override;

	void focusGained(FocusChangeType cause) override;
	bool keyPressed(const juce::KeyPress& key) override;

	void valueTreeParentChanged(juce::ValueTree& treeWhoseParentHasChanged) override;
	void valueTreeChildAdded(juce::ValueTree& parentTree, juce::ValueTree& childWhichHasBeenAdded) override;
	void valueTreeChildRemoved(juce::ValueTree& parentTree, juce::ValueTree& childWhichHasBeenRemoved, int indexFromWhichChildWasRemoved) override;
	void changeListenerCallback(juce::ChangeBroadcaster* source) override;

private:
	fumeUI::UIContext& uiContext;
	juce::ValueTree currentTrackState;
	ManualViewport viewport;
	juce::Component containerComponent;
	std::vector<std::unique_ptr<fumeUI::FlexibleComponent>> pluginComponents;
	fumeUI::CardinalFocusTraverser focusTraverser;

	void clearPluginComponents();

	void populatePluginComponentsForTrack(te::Track* track);

	void addPlusComponent(te::Track* track);

	void globalFocusChanged(juce::Component* focusedComponent) override;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginChainComponent)
};