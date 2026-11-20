#include "PluginChainComponent.h"

PluginChainComponent::PluginChainComponent(fumeUI::UIContext& uiContext)
	: uiContext{ uiContext }
{
	uiContext.selectionManager.addChangeListener(this);
}

PluginChainComponent::~PluginChainComponent()
{
	currentTrackState.removeListener(this);
	uiContext.selectionManager.removeChangeListener(this);
}

void PluginChainComponent::paint(juce::Graphics& g)
{
	g.fillAll(juce::Colours::darkgrey);
	g.setColour(juce::Colours::white);
	g.drawText("PluginChain", getLocalBounds(), juce::Justification::centred);
}

void PluginChainComponent::resized()
{
}

void PluginChainComponent::focusGained(FocusChangeType cause)
{
	// if some plugin editor is open, we should let it grab keyboard focus.
}

void PluginChainComponent::valueTreeParentChanged(juce::ValueTree& treeWhoseParentHasChanged)
{
	if (!currentTrackState.getParent().isValid())
	{
		// If the current track state has been removed from the ValueTree, reset the plugin components
		currentTrackState.removeListener(this);
		clearPluginComponents();
	}
}

void PluginChainComponent::changeListenerCallback(juce::ChangeBroadcaster* source)
{
	auto& selectionManager = uiContext.selectionManager;

	if (source == &selectionManager)
	{
		for (auto* selectable : selectionManager.getSelectedObjects())
		{
			if (te::MidiClip* clip = dynamic_cast<te::MidiClip*> (selectable))
			{
				auto* track = clip->getTrack();
				currentTrackState = track->state;
				currentTrackState.addListener(this);
				populatePluginComponentsForTrack(*track);
				resized();
				return;
			}
		}
	}
}

void PluginChainComponent::clearPluginComponents()
{
}

void PluginChainComponent::populatePluginComponentsForTrack(te::Track& track)
{
	auto plugins = track.getAllPlugins();

	for (auto plugin : plugins)
	{
		DBG("Plugin: " << plugin->getName());
	}
}
