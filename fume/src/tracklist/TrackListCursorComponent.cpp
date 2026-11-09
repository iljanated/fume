#pragma once

#include "TrackListCursorComponent.h"
#include "TrackListComponent.h"

TrackListCursorComponent::TrackListCursorComponent(fumeUI::UIContext& uiContext, TrackListComponent& trackListComponent)
	: uiContext(uiContext), editState(uiContext.extendedEdit.getState()), trackListComponent(trackListComponent)
{
	editState.addListener(this);
	updateBounds();
}

TrackListCursorComponent::~TrackListCursorComponent()
{
	editState.removeListener(this);
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

void TrackListCursorComponent::valueTreePropertyChanged(juce::ValueTree& treeWhosePropertyHasChanged, const juce::Identifier& property)
{
	if (property == FumeIDs::ZoomX || property == FumeIDs::ZoomY || property == FumeIDs::CursorX || property == FumeIDs::CursorY)
	{
		updateBounds();
	}
}

void TrackListCursorComponent::moveCursorPosition(const juce::Point<int>& delta)
{
	auto& extendedEdit = uiContext.extendedEdit;

	auto x = delta.getX();
	if (x != 0)
	{
		auto timePosition = te::TimePosition::fromSeconds(extendedEdit.getCursorX());
		auto timeOffset = te::TimeDuration::fromSeconds(0.0001);

		auto& snapType = trackListComponent.getSnapType();
		auto& tempoSequence = uiContext.extendedEdit.getTempoSequence();

		auto newX = extendedEdit.getCursorX();

		if (x > 0)
		{

			newX = snapType.roundTimeUp(timePosition + timeOffset, tempoSequence, false).inSeconds();
		}
		else
		{
			newX = snapType.roundTimeDown(timePosition - timeOffset, tempoSequence, false).inSeconds();
		}

		extendedEdit.setCursorX(std::clamp(newX, 0.0, uiContext.extendedEdit.getEdit().getMaximumEditEnd().inSeconds()));
	}

	extendedEdit.setCursorY(std::clamp(extendedEdit.getCursorY() + delta.getY(), 0.0, (double)trackListComponent.getTrackCount() - 1));
	updateBounds();
}

void TrackListCursorComponent::updateBounds()
{
	auto& extendedEdit = uiContext.extendedEdit;

	int margin = 2;
	
	auto rect = juce::Rectangle<int>(
		extendedEdit.getCursorX() / extendedEdit.getSecondsPerPixel() - margin,
		extendedEdit.getCursorY() * extendedEdit.getTrackHeight() - margin,
		margin * 2,
		extendedEdit.getTrackHeight() + margin * 2
	);
	
	auto bounds = getBounds();
	if (rect != bounds)
	{
		setBounds(rect);
	}
}
