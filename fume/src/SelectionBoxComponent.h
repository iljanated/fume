#pragma once

#include <JuceHeader.h>
#include "Utilities.h"

class SelectionBoxSource
{
public:
	virtual ~SelectionBoxSource() = default;
	virtual void selectItemsInBox(const juce::Rectangle<double>& selectionBox) = 0;
};


class SelectionBoxComponent : public juce::Component
{
public:
	SelectionBoxComponent(fumeUI::Grid& grid, int margin, SelectionBoxSource* c, InputManager& i);

	~SelectionBoxComponent() override = default;
	
	bool keyPressed(const juce::KeyPress& key);

	void paint(juce::Graphics& g) override;

	void startSelection(const juce::Point<double>& startPoint);

	void updateSelection(const juce::Point<double>& currentPoint);

	void endSelection();

	juce::Rectangle<double> getSelectionBox() const;

	void setColour(juce::Colour newColour);

	bool getIsSelecting() const { return isSelecting; }
	juce::Point<double> getSelectionStart() const { return selectionStart; }
	juce::Point<double> getSelectionEnd() const { return selectionEnd; }
private:
	fumeUI::Grid& grid;
	InputManager& inputManager;
	juce::Colour colour = juce::Colours::blue.withAlpha(0.3f);
	int margin = 2;
	juce::Point<double> selectionStart;
	juce::Point<double> selectionEnd;
	bool isSelecting = false;
	SelectionBoxSource* source = nullptr;

	void updateBounds();
};