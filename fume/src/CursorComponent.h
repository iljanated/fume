#pragma once

#include <JuceHeader.h>
#include "Utilities.h"

class CursorComponent : public juce::Component
{
public:
	CursorComponent(fumeUI::Grid& grid, int margin, InputManager& i);


	bool keyPressed(const juce::KeyPress& key);
	void paint(juce::Graphics& g) override;
	void setCursorPosition(const juce::Point<double>& currentPoint);
	juce::Point<double> getCursorPosition() const { return cursorPosition; }
	void setColour(juce::Colour newColour);

private:
	InputManager& inputManager;
	fumeUI::Grid& grid;
	juce::Colour colour = juce::Colours::white;
	int margin = 2;
	juce::Point<double> cursorPosition;

	void updateBounds();
};