#pragma once

#include "TimelineComponent.h"

TimelineComponent::TimelineComponent(fumeUI::UIContext& context, fumeUI::Grid& grid)
	: uiContext(context), grid(grid)
{
}

void TimelineComponent::paint(juce::Graphics& g)
{
	g.fillAll(juce::Colours::lightgrey);
	auto unitX = grid.getUnitSize().getX();

	auto stepWidth = unitX * grid.getCellSize().getX();

	auto startX = std::fmod(offset, stepWidth);
	DBG(startX);
	for (auto x = startX; x < getWidth(); x += stepWidth)
	{
		g.setColour(juce::Colours::darkgrey);
		g.drawLine(x, 0, x, getHeight(),1.0f);
		auto beatNumber = static_cast<int>((-offset + x) / unitX);
		g.drawText(std::to_string(beatNumber), x + 2, 2, 30, 20, juce::Justification::left);
	}
}

void TimelineComponent::resized()
{
}
