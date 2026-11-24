#include "PluginChainComponent.h"
#include "FourOscPluginComponent.h"
#include "VolumeAndPanPluginComponent.h"
#include "LevelMeterPluginComponent.h"

PluginChainComponent::PluginChainComponent(fumeUI::UIContext& uiContext)
	: uiContext{ uiContext }, focusTraverser{ this }
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
	auto height = localBounds.getHeight();

	int x = 0;
	
	for (auto& comp : pluginComponents)
	{
		if (comp != nullptr)
		{
			auto width = comp->getWidth();
			comp->setBounds(juce::Rectangle(x, 0, width, height));
			x += width;
		}
	}
}

void PluginChainComponent::focusGained(FocusChangeType cause)
{
	juce::ignoreUnused(cause);

	juce::MessageManager::callAsync([safeThis = juce::Component::SafePointer<PluginChainComponent>(this)]
		{
			if (safeThis == nullptr)
				return;

			if (auto* defaultComponent = safeThis->focusTraverser.getDefaultComponent(safeThis.getComponent()))
				defaultComponent->grabKeyboardFocus();
		});
}

bool PluginChainComponent::keyPressed(const juce::KeyPress& key)
{
	auto keyCode = key.getKeyCode();
	auto modifiers = key.getModifiers();
	auto& inputManager = uiContext.inputManager;

	if (inputManager.validate(keyCode, modifiers, InputAction::MoveLeft))
	{
		focusComponent({ -1, 0 });
		return true;
	}
	if (inputManager.validate(keyCode, modifiers, InputAction::MoveRight))
	{
		focusComponent({ 1, 0 });
		return true;
	}
	if (inputManager.validate(keyCode, modifiers, InputAction::MoveUp))
	{
		focusComponent({ 0, -1 });
		return true;
	}
	if (inputManager.validate(keyCode, modifiers, InputAction::MoveDown))
	{
		focusComponent({ 0, 1 });
		return true;
	}
	return false;
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
			auto component = std::make_unique<FourOscPluginComponent>(uiContext, fourOscPlugin);
			addAndMakeVisible(*component);
			pluginComponents.push_back(std::move(component));
		}
		else if (pluginName == "Volume & Pan Plugin")
		{
			auto volumeAndPanPlugin = dynamic_cast<te::VolumeAndPanPlugin*>(plugin);
			auto component = std::make_unique<VolumeAndPanPluginComponent>(uiContext, volumeAndPanPlugin);
			addAndMakeVisible(*component);
			pluginComponents.push_back(std::move(component));
		}
		else if (pluginName == "Level Meter")
		{	auto levelMeterPlugin = dynamic_cast<te::LevelMeterPlugin*>(plugin);
			auto component = std::make_unique<LevelMeterPluginComponent>(uiContext, levelMeterPlugin);
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

void PluginChainComponent::focusComponent(juce::Point<int> delta)
{
	auto* current = juce::Component::getCurrentlyFocusedComponent();

	if (current == nullptr || (!isParentOf(current) && current != this))
		current = focusTraverser.getDefaultComponent(this);

	if (current == nullptr)
		return;

	auto* next = focusTraverser.getNextComponent(current, delta);

	if (next == nullptr)
		next = focusTraverser.getDefaultComponent(this);

	if (next != nullptr)
		next->grabKeyboardFocus();
}
