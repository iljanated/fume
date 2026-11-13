#pragma once

#include "MidiEventsEditorCursorComponent.h"

MidiEventsEditorCursorComponent::MidiEventsEditorCursorComponent(fumeUI::UIMidiClipContext& uiContext)
	: uiContext(uiContext), midiClipState(uiContext.extendedMidiClip.getState())
{
	midiClipState.addListener(this);
	updateBounds();
}

MidiEventsEditorCursorComponent::~MidiEventsEditorCursorComponent()
{
	midiClipState.removeListener(this);
}

void MidiEventsEditorCursorComponent::paint(juce::Graphics& g)
{
	g.setColour(juce::Colours::white);
	g.fillRect(getLocalBounds());
}

bool MidiEventsEditorCursorComponent::keyPressed(const juce::KeyPress& key)
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

void MidiEventsEditorCursorComponent::valueTreePropertyChanged(juce::ValueTree& treeWhosePropertyHasChanged, const juce::Identifier& property)
{
	if (property == FumeIDs::ZoomX || property == FumeIDs::ZoomY || property == FumeIDs::CursorX || property == FumeIDs::CursorY)
	{
		updateBounds();
	}
}

void MidiEventsEditorCursorComponent::moveCursorPosition(const juce::Point<int>& delta)
{
	auto& extendedMidiClip = uiContext.extendedMidiClip;

	auto x = delta.getX();
	if (x != 0)
	{
		auto& tempoSequence = uiContext.extendedEdit.getTempoSequence();
		auto snapType = te::TimecodeSnapType(te::TimecodeType::barsBeats, extendedMidiClip.getCursorSnapLevel());
		auto timePosition = te::TimePosition::fromSeconds(extendedMidiClip.getCursorX());
		
		auto timeOffset = te::TimeDuration::fromSeconds(0.0001);
		
		if (x > 0)
		{
			auto nextTime = snapType.roundTimeUp(timePosition + timeOffset, tempoSequence);
			extendedMidiClip.setCursorX(nextTime.inSeconds());
		}
		else
		{
			auto previousTime = snapType.roundTimeDown(timePosition - timeOffset, tempoSequence);
			extendedMidiClip.setCursorX(previousTime.inSeconds());
		}
	}

	extendedMidiClip.setCursorY(std::clamp(extendedMidiClip.getCursorY() + delta.getY(), 0.0, 127.0));
}

void MidiEventsEditorCursorComponent::updateBounds()
{
	auto& extendedMidiClip = uiContext.extendedMidiClip;

	int margin = 2;

	auto rect = juce::Rectangle<int>(
		extendedMidiClip.getCursorX() / extendedMidiClip.getSecondsPerPixel() - margin,
		extendedMidiClip.getCursorY() * extendedMidiClip.getNoteHeight() - margin,
		margin * 2,
		extendedMidiClip.getNoteHeight() + margin * 2
	);

	auto bounds = getBounds();
	if (rect != bounds)
	{
		setBounds(rect);
	}
}
