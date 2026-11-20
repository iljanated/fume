#include "OptionsComponent.h"

OptionsComponent::OptionsComponent(te::Engine& engine, InputManager& inputManager, EditManager& editManager)
	: engine(engine), inputManager(inputManager), editManager(editManager)
{
	setWantsKeyboardFocus(true);
}

OptionsComponent::~OptionsComponent()
{
}

void OptionsComponent::paint(juce::Graphics& g)
{
	g.fillAll(juce::Colours::black);
}

void OptionsComponent::resized()
{
}

bool OptionsComponent::keyPressed(const juce::KeyPress& key)
{
	return false;
}
