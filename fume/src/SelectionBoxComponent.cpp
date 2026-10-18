#pragma once

#include "SelectionBoxComponent.h"

SelectionBoxComponent::SelectionBoxComponent(fumeUI::Grid& grid, int margin, SelectionBoxSource* c, InputManager& i)
	: grid(grid), margin(margin), source(c), inputManager(i)
{
	setVisible(false);
}

void SelectionBoxComponent::paint(juce::Graphics& g)
{
	g.setColour(colour.withAlpha(0.3f));
	g.fillRect(getLocalBounds());
	g.setColour(colour);
	g.drawRect(getLocalBounds(), 1);
}

bool SelectionBoxComponent::keyPressed(const juce::KeyPress& key)
{
	if (!isVisible() || !isSelecting)
	{
		return false;
	}

	auto keyCode = key.getKeyCode();

	if (inputManager.validate(keyCode, InputAction::Left))
	{
		updateSelection(selectionEnd + juce::Point<double>(-grid.getSnappingInterval(), 0));
		return true;
	}
	else if (inputManager.validate(keyCode, InputAction::Right))
	{
		updateSelection(selectionEnd + juce::Point<double>(grid.getSnappingInterval(), 0));
		return true;
	}
	else if (inputManager.validate(keyCode, InputAction::Up))
	{
		updateSelection(selectionEnd + juce::Point<double>(0, -1.0));
		return true;
	}
	else if (inputManager.validate(keyCode, InputAction::Down))
	{
		updateSelection(selectionEnd + juce::Point<double>(0, 1.0));
		return true;
	}
	return false;
}

void SelectionBoxComponent::startSelection(const juce::Point<double>& startPoint)
{
	selectionStart = startPoint;
	selectionEnd = startPoint;
	isSelecting = true;
	if (source)
	{
		source->selectItemsInBox(getSelectionBox());
	}
	updateBounds();
	setVisible(true);
}

void SelectionBoxComponent::updateSelection(const juce::Point<double>& currentPoint)
{
	if (isSelecting)
	{
		auto gridSize = grid.getGridSize();
		if (currentPoint.x < 0 || currentPoint.x >= gridSize.x || currentPoint.y < 0 || currentPoint.y >= gridSize.y)
		{
			return;
		}
		selectionEnd = currentPoint;
		if (source)
		{
			source->selectItemsInBox(getSelectionBox());
		}
		updateBounds();
	}
}

void SelectionBoxComponent::endSelection()
{
	isSelecting = false;
	setVisible(false);
}

juce::Rectangle<double> SelectionBoxComponent::getSelectionBox() const
{
	return juce::Rectangle<double>::leftTopRightBottom(
		std::min(selectionStart.x, selectionEnd.x),
		std::min(selectionStart.y, selectionEnd.y),
		std::max(selectionStart.x, selectionEnd.x),
		std::max(selectionStart.y, selectionEnd.y)
	);
}

void SelectionBoxComponent::setColour(juce::Colour newColour)
{
	colour = newColour;
	repaint();
}

void SelectionBoxComponent::updateBounds()
{
	auto rect = getSelectionBox();
	auto unitSize = grid.getUnitSize();
	setBounds(
		rect.getX() * unitSize.x - margin,
		rect.getY() * unitSize.y - margin,
		rect.getWidth() * unitSize.x + 2 * margin,
		(rect.getHeight() + 1) * unitSize.y + 2 * margin);
}
