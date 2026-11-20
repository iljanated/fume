#pragma once

#include <JuceHeader.h>
#include "InputManager.h"
#include "EditManager.h"

namespace te = tracktion;

class OptionsComponent : public juce::Component
{
public:
	OptionsComponent(te::Engine& engine, InputManager& inputManager, EditManager& editManager);
	~OptionsComponent() override;
	void paint(juce::Graphics&) override;
	void resized() override;
	bool keyPressed(const juce::KeyPress& key) override;
private:
	te::Engine& engine;
	InputManager& inputManager;
	EditManager& editManager;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OptionsComponent)
};