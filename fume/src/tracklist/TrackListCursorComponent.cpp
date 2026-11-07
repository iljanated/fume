#pragma once

#include "TrackListCursorComponent.h"
#include "TrackListComponent.h"

TrackListCursorComponent::TrackListCursorComponent(fumeUI::UIContext& uiContext, TrackListComponent& trackListComponent)
	: uiContext(uiContext), trackListComponent(trackListComponent)
{
	uiContext.extendedEdit.getState().addListener(this);
	updateBounds();
}

TrackListCursorComponent::~TrackListCursorComponent()
{
	uiContext.extendedEdit.getState().removeListener(this);
}

void TrackListCursorComponent::paint(juce::Graphics& g)
{
	g.setColour(juce::Colours::white);
	g.fillRect(getLocalBounds());
}

bool TrackListCursorComponent::keyPressed(const juce::KeyPress& key)
{
	if (!isVisible())
	{
		return false;
	}

	auto keyCode = key.getKeyCode();
	auto modifiers = key.getModifiers();
	auto& inputManager = uiContext.inputManager;

	if (inputManager.validate(keyCode, modifiers, InputAction::Left))
	{
		moveCursorPosition(juce::Point<int>(-1, 0));
		return true;
	}
	else if (inputManager.validate(keyCode, modifiers, InputAction::Right))
	{
		moveCursorPosition(juce::Point<int>(1, 0));
		return true;
	}
	else if (inputManager.validate(keyCode, modifiers, InputAction::Up))
	{
		moveCursorPosition(juce::Point<int>(0, -1));
		return true;
	}
	else if (inputManager.validate(keyCode, modifiers, InputAction::Down))
	{
		moveCursorPosition(juce::Point<int>(0, 1));
		return true;
	}
	return false;
}

void TrackListCursorComponent::setCursorPosition(const juce::Point<double>& newPosition)
{
	cursorPosition = newPosition;
	updateBounds();
}

void TrackListCursorComponent::valueTreePropertyChanged(juce::ValueTree& treeWhosePropertyHasChanged, const juce::Identifier& property)
{
	if (property == FumeIDs::ZoomX || property == FumeIDs::ZoomY)
	{
		updateBounds();
	}
}

void TrackListCursorComponent::moveCursorPosition(const juce::Point<int>& delta)
{
	auto x = delta.getX();
	if (x != 0)
	{
		auto timePosition = te::TimePosition::fromSeconds(cursorPosition.getX());
		auto timeOffset = te::TimeDuration::fromSeconds(0.0001);

		auto& snapType = trackListComponent.getSnapType();
		auto& tempoSequence = uiContext.extendedEdit.getTempoSequence();

		if (x > 0)
		{

			cursorPosition.setX(snapType.roundTimeUp(timePosition + timeOffset, tempoSequence, false).inSeconds());
		}
		else
		{
			cursorPosition.setX(snapType.roundTimeDown(timePosition - timeOffset, tempoSequence, false).inSeconds());
		}

		cursorPosition.setX(std::clamp(cursorPosition.getX(), 0.0, uiContext.extendedEdit.getEdit().getMaximumEditEnd().inSeconds()));
	}

	cursorPosition.setY(std::clamp(cursorPosition.getY() + delta.getY(), 0.0, (double)trackListComponent.getTrackCount() - 1));
	updateBounds();
}

void TrackListCursorComponent::updateBounds()
{
	auto& extendedEdit = uiContext.extendedEdit;

	int margin = 2;
	
	auto rect = juce::Rectangle<int>(
		cursorPosition.getX() / extendedEdit.getSecondsPerPixel() - margin,
		cursorPosition.getY() * extendedEdit.getTrackHeight() - margin,
		margin * 2,
		extendedEdit.getTrackHeight() + margin * 2
	);
	
	auto bounds = getBounds();
	if (rect != bounds)
	{
		setBounds(rect);
	}
}
