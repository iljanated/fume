#include "CursorComponent.h"

CursorComponent::CursorComponent(fumeUI::Grid& grid, int margin, InputManager& i)
	: grid(grid), margin(margin), inputManager(i)
{
}

void CursorComponent::paint(juce::Graphics& g)
{
	g.setColour(colour);
	g.fillRect(getLocalBounds());
}

bool CursorComponent::keyPressed(const juce::KeyPress& key)
{
	if (!isVisible())
	{
		return false;
	}

	auto keyCode = key.getKeyCode();
	auto modifiers = key.getModifiers();

	if (inputManager.validate(keyCode, modifiers, InputAction::Left))
	{
		setCursorPosition(cursorPosition + juce::Point<double>(-grid.getSnappingInterval(), 0));
		return true;
	}
	else if (inputManager.validate(keyCode, modifiers, InputAction::Right))
	{
		setCursorPosition(cursorPosition + juce::Point<double>(grid.getSnappingInterval(), 0));
		return true;
	}
	else if (inputManager.validate(keyCode, modifiers, InputAction::Up))
	{
		setCursorPosition(cursorPosition + juce::Point<double>(0, -1.0));
		return true;
	}
	else if (inputManager.validate(keyCode, modifiers, InputAction::Down))
	{
		setCursorPosition(cursorPosition + juce::Point<double>(0, 1.0));
		return true;
	}
	return false;
}

void CursorComponent::setCursorPosition(const juce::Point<double>& currentPoint)
{
	auto gridSize = grid.getGridSize();
	if (currentPoint.x < 0 || currentPoint.x >= gridSize.x || currentPoint.y < 0 || currentPoint.y >= gridSize.y)
	{
		return;
	}
	cursorPosition = currentPoint;
	updateBounds();
}

void CursorComponent::setColour(juce::Colour newColour)
{
	colour = newColour;
	repaint();
}

void CursorComponent::updateBounds()
{
	auto unitSize = grid.getUnitSize();

	auto x = cursorPosition.x * unitSize.x - margin;
	auto y = cursorPosition.y * unitSize.y - margin;
	auto width = 2 * margin;
	auto height = unitSize.y + 2 * margin;
	setBounds(x, y, width, height);
}
