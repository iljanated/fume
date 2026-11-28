#include "PluginChainComponent.h"
#include "FourOscPluginComponent.h"
#include "VolumeAndPanPluginComponent.h"
#include "LevelMeterPluginComponent.h"

PluginChainComponent::PluginChainComponent(fumeUI::UIContext& uiContext)
	: uiContext{ uiContext }, focusTraverser{ uiContext, this }
{
	uiContext.selectionManager.addChangeListener(this);
	addAndMakeVisible(viewport);
	viewport.setViewedComponent(&containerComponent, false);
	juce::Desktop::getInstance().addFocusChangeListener(this);
}

PluginChainComponent::~PluginChainComponent()
{
	currentTrackState.removeListener(this);
	uiContext.selectionManager.removeChangeListener(this);
}

void PluginChainComponent::paint(juce::Graphics& g)
{
	g.fillAll(juce::Colours::black);
}

void PluginChainComponent::resized()
{
	auto localBounds = getLocalBounds();

	viewport.setBounds(localBounds);

	auto height = localBounds.getHeight() - viewport.getScrollBarThickness();
	int width = 0;
	for (auto& comp : pluginComponents)
	{
		if (comp != nullptr)
		{
			auto compWidth = comp->getMinimumWidth();
			comp->setBounds(width, 0, compWidth, height);
			width += compWidth;
		}
	}

	containerComponent.setBounds(0, 0, width, height);
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

	if (inputManager.validate(keyCode, modifiers, InputAction::Write))
	{
		auto* currentlyFocusedComponent = dynamic_cast<juce::Button*>(focusTraverser.getCurrentComponent());
		if (currentlyFocusedComponent != nullptr && currentlyFocusedComponent->isEnabled())
		{
			currentlyFocusedComponent->triggerClick();
		}
		return true;
	}

	if (focusTraverser.keyPressed(key))
	{
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

void PluginChainComponent::valueTreeChildAdded(juce::ValueTree& parentTree, juce::ValueTree& childWhichHasBeenAdded)
{
	if (parentTree == currentTrackState)
	{
		auto* track = uiContext.extendedEdit.getEdit().getTrackList().getTrackFor(currentTrackState);

		if (track != nullptr)
		{
			populatePluginComponentsForTrack(track);
			resized();
			if(isShowing())
			{
			juce::MessageManager::callAsync([safeThis = juce::Component::SafePointer<PluginChainComponent>(this)]
				{
					if (safeThis == nullptr)
						return;

					safeThis->grabKeyboardFocus();
				});
			}
		}
	}
}

void PluginChainComponent::valueTreeChildRemoved(juce::ValueTree& parentTree, juce::ValueTree& childWhichHasBeenRemoved, int indexFromWhichChildWasRemoved)
{
	if (parentTree == currentTrackState)
	{
		auto* track = uiContext.extendedEdit.getEdit().getTrackList().getTrackFor(currentTrackState);
		if (track != nullptr)
		{
			populatePluginComponentsForTrack(track);
			resized();
			if (isShowing())
			{
			juce::MessageManager::callAsync([safeThis = juce::Component::SafePointer<PluginChainComponent>(this)]
				{
					if (safeThis == nullptr)
						return;

					safeThis->grabKeyboardFocus();
				});
			}
		}
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
				populatePluginComponentsForTrack(track);
				resized();
				return;
			}
		}
	}
}

void PluginChainComponent::clearPluginComponents()
{
}

void PluginChainComponent::populatePluginComponentsForTrack(te::Track* track)
{
	pluginComponents.clear();

	auto plugins = track->getAllPlugins();

	for (auto plugin : plugins)
	{
		DBG("Plugin: " << plugin->getName());
		auto pluginName = plugin->getName();

		if (pluginName == "4OSC")
		{
			addPlusComponent(track);

			auto fourOscPlugin = dynamic_cast<te::FourOscPlugin*>(plugin);
			auto component = std::make_unique<FourOscPluginComponent>(fourOscPlugin, uiContext);
			auto wrapper = std::make_unique<PluginWrapperComponent>(std::move(component));
			containerComponent.addAndMakeVisible(*wrapper);
			pluginComponents.push_back(std::move(wrapper));
		}
		else if (pluginName == "Volume & Pan Plugin")
		{
			addPlusComponent(track);

			auto volumeAndPanPlugin = dynamic_cast<te::VolumeAndPanPlugin*>(plugin);
			auto component = std::make_unique<VolumeAndPanPluginComponent>(volumeAndPanPlugin, uiContext);
			auto wrapper = std::make_unique<PluginWrapperComponent>(std::move(component));
			containerComponent.addAndMakeVisible(*wrapper);
			pluginComponents.push_back(std::move(wrapper));
		}
		else if (pluginName == "Level Meter")
		{	auto levelMeterPlugin = dynamic_cast<te::LevelMeterPlugin*>(plugin);
			auto component = std::make_unique<LevelMeterPluginComponent>(levelMeterPlugin, uiContext);
			auto wrapper = std::make_unique<PluginWrapperComponent>(std::move(component));
			containerComponent.addAndMakeVisible(*wrapper);
			pluginComponents.push_back(std::move(wrapper));
		}
		else
		{
			DBG("Unknown plugin: " << pluginName);
			continue;
		}
	}
}

void PluginChainComponent::addPlusComponent(te::Track* track)
{
	auto plusComponent = std::make_unique<PlusComponent>(uiContext, track);
	containerComponent.addAndMakeVisible(*plusComponent);
	pluginComponents.push_back(std::move(plusComponent));
}

void PluginChainComponent::globalFocusChanged(juce::Component* focusedComponent)
{
	if (focusedComponent != nullptr && isParentOf(focusedComponent))
	{
		juce::MessageManager::callAsync([this, focusedComponent]() {
			fumeUI::makeComponentVisibleInViewport(viewport, *focusedComponent, fumeUI::scrollPadding);
			});
	}
}
