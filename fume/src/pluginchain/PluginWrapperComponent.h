#pragma once

#include <JuceHeader.h>
#include "UIContext.h"
#include "Utilities.h"

namespace te = tracktion;

class PluginComponent : public juce::Component
{
public:
	PluginComponent(te::Plugin* plugin, fumeUI::UIContext& context, int minimumWidth)
		: plugin(plugin), uiContext(context), minimumWidth(minimumWidth)
	{
	}

	~PluginComponent() override = default;

	te::Plugin* getBasePlugin() const { return plugin; }
	fumeUI::UIContext& getUIContext() const { return uiContext; }

	int getMinimumWidth() const { return minimumWidth; }
	
private:
	te::Plugin* plugin;
	fumeUI::UIContext& uiContext;
	int minimumWidth;
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginComponent)
};

template <typename PluginType>
requires std::derived_from<PluginType, te::Plugin>
class TypedPluginComponent : public PluginComponent
{
public:
	explicit TypedPluginComponent(PluginType* plugin, fumeUI::UIContext& context, int desiredWidth)
		: PluginComponent(plugin, context, desiredWidth)
	{
	}

	~TypedPluginComponent() override = default;

	PluginType* getPlugin() const { return static_cast<PluginType*>(getBasePlugin()); }

private:

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TypedPluginComponent)
};

class PluginWrapperComponent : public fumeUI::FlexibleComponent
{
public:
	PluginWrapperComponent(std::unique_ptr<PluginComponent> pluginComponent)
		: pluginComponent(std::move(pluginComponent))
	{
		addAndMakeVisible(*this->pluginComponent);
	}

	~PluginWrapperComponent() override = default;

	void paint(juce::Graphics& g) override
	{
		g.setColour(juce::Colours::darkgrey);
		g.fillRoundedRectangle(getLocalBounds().toFloat(), 10.0f);
		g.setColour(juce::Colours::white);
		g.drawRoundedRectangle(getLocalBounds().toFloat(), 10.0f, 2.0f);
	}

	void resized() override {
		auto localBounds = getLocalBounds();
		localBounds.removeFromTop(20);
		if (pluginComponent)
		{
			pluginComponent->setBounds(localBounds.reduced(10));
		}
	}

	int getMinimumWidth() const override
	{
		return pluginComponent ? pluginComponent->getMinimumWidth() : 0;
	}

	int getMinimumHeight() const override
	{
		return pluginComponent ? pluginComponent->getHeight() : 0;
	}

private:
	std::unique_ptr<PluginComponent> pluginComponent;
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginWrapperComponent)
};