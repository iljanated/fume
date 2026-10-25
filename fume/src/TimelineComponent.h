#pragma once

#include <JuceHeader.h>
#include "Utilities.h"

class TimelineComponent : public juce::Component
{
public:
	TimelineComponent(fumeUI::UIContext& context, fumeUI::Grid& grid);

	void paint(juce::Graphics& g) override;

	void resized() override;
	void setOffset(int newOffset) { offset = newOffset; repaint(); }
private:
	fumeUI::UIContext& uiContext;
	fumeUI::Grid& grid;
	int offset = 0;
};	