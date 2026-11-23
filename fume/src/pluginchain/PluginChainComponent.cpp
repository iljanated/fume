#include "PluginChainComponent.h"
#include "FourOscPluginComponent.h"
#include "VolumeAndPanPluginComponent.h"
#include "LevelMeterPluginComponent.h"

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
	auto localBounds = getLocalBounds();
	
	for (auto& comp : pluginComponents)
	{
		if (comp != nullptr)
		{
			comp->setBounds(localBounds.removeFromLeft(300));
		}
	}
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
	pluginComponents.clear();

	auto plugins = track.getAllPlugins();

	for (auto plugin : plugins)
	{
		DBG("Plugin: " << plugin->getName());
		auto pluginName = plugin->getName();

		if (pluginName == "4OSC")
		{
			auto fourOscPlugin = dynamic_cast<te::FourOscPlugin*>(plugin);
			auto component = std::make_unique<FourOscPluginComponent>(fourOscPlugin);
			addAndMakeVisible(*component);
			pluginComponents.push_back(std::move(component));
		}
		else if (pluginName == "Volume & Pan Plugin")
		{
			auto component = std::make_unique<VolumeAndPanPluginComponent>();
			addAndMakeVisible(*component);
			pluginComponents.push_back(std::move(component));
		}
		else if (pluginName == "Level Meter")
		{
			auto levelMeterPlugin = dynamic_cast<te::LevelMeterPlugin*>(plugin);
			auto component = std::make_unique<LevelMeterPluginComponent>(levelMeterPlugin);
			addAndMakeVisible(*component);
			pluginComponents.push_back(std::move(component));
		}
		else
		{
			DBG("Unknown plugin: " << pluginName);
			continue;
		}
	}
}
