#pragma once

#include <JuceHeader.h>
#include "UIContext.h"

namespace te = tracktion;

class LevelMeterComponent : public juce::Component, private juce::Timer
{
public:
	LevelMeterComponent(te::LevelMeasurer& measurer)
		: measurer(measurer)
	{
		measurer.addClient(client);
		startTimerHz(30);
	}

	~LevelMeterComponent() override
	{
		stopTimer();
		measurer.removeClient(client);
	}

	void paint(juce::Graphics& g) override
	{
		if (numChannels == 0)
		{
			return;
		}

		auto area = getLocalBounds();

		auto width = area.getWidth() / numChannels;

		for (int ch = 0; ch < numChannels; ++ch)
		{
			auto barArea = area.removeFromLeft(width).reduced(1);

			const float normLevel = juce::jmap(juce::jlimit(minLevelDb, 0.0f, currentLevelsDb[ch]), minLevelDb, 0.0f, 0.0f, 1.0f);

			const int fillHeight = static_cast<int>(barArea.getHeight() * normLevel);
			auto fillArea = barArea.removeFromBottom(fillHeight);

			g.setColour(currentLevelsDb[ch] > -3.0f ? juce::Colours::red : juce::Colours::limegreen);
			g.fillRect(fillArea);
		}
	}

	void resized() override
	{
	}
private:
	te::LevelMeasurer& measurer;
	te::LevelMeasurer::Client client;
	int numChannels = 0;
	float minLevelDb = -100.0f;
	float fallOffRateDbPerFrame = 3.5f;
	float currentLevelsDb[2] = { minLevelDb, minLevelDb };

	void timerCallback() override
	{
		numChannels = std::min(2, client.getNumChannelsUsed());

		for (int i = 0; i < numChannels; ++i)
		{
			const float rawDb = client.getAndClearAudioLevel(i).dB;
			currentLevelsDb[i] = std::max({ rawDb, currentLevelsDb[i] - fallOffRateDbPerFrame, minLevelDb });
		}

		repaint();
	}

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LevelMeterComponent)
};

class LevelMeterPluginComponent : public juce::Component
{
public:
	LevelMeterPluginComponent(fumeUI::UIContext& context, te::LevelMeterPlugin* plugin)
		: uiContext(context), plugin(plugin), levelMeterComponent(plugin->measurer)
	{
		addAndMakeVisible(levelMeterComponent);
	}

	~LevelMeterPluginComponent() override 
	{
	}

	void paint(juce::Graphics& g) override
	{
		g.fillAll(juce::Colours::black);

		g.setColour(juce::Colours::white);
		g.drawText("LevelMeterPluginComponent", getLocalBounds(), juce::Justification::centred, true);
	}

	void resized() override 
	{
		levelMeterComponent.setBounds(juce::Rectangle<int>(20, 20, 30, 100));
	}
private:
	fumeUI::UIContext& uiContext;
	te::LevelMeterPlugin* plugin;
	LevelMeterComponent levelMeterComponent;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LevelMeterPluginComponent)
};