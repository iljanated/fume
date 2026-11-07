#pragma once

#include "MidiEventsEditorCursorComponent.h"
#include "MidiEventsEditorComponent.h"

MidiEventsEditorCursorComponent::MidiEventsEditorCursorComponent(fumeUI::UIMidiClipContext& uiContext, MidiEventsEditorComponent& midiEventsEditorComponent)
	: uiContext(uiContext), midiEventsEditorComponent(midiEventsEditorComponent)
{
	uiContext.extendedEdit.getState().addListener(this);
	updateBounds();
}

MidiEventsEditorCursorComponent::~MidiEventsEditorCursorComponent()
{
	uiContext.extendedEdit.getState().removeListener(this);
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

void MidiEventsEditorCursorComponent::setCursorPosition(const juce::Point<double>& newPosition)
{
	cursorPosition = newPosition;
	updateBounds();
}

void MidiEventsEditorCursorComponent::valueTreePropertyChanged(juce::ValueTree& treeWhosePropertyHasChanged, const juce::Identifier& property)
{
	if (property == FumeIDs::ZoomX || property == FumeIDs::ZoomY)
	{
		updateBounds();
	}
}

void MidiEventsEditorCursorComponent::moveCursorPosition(const juce::Point<int>& delta)
{
	auto x = delta.getX();
	if (x != 0)
	{
		auto& extendedMidiClip = uiContext.extendedMidiClip;

		auto& tempoSequence = uiContext.extendedEdit.getTempoSequence();

		auto timePosition = tempoSequence.beatsToTime(te::BeatPosition::fromBeats(cursorPosition.getX()));

		auto snapType = te::TimecodeSnapType(te::TimecodeType::barsBeats, extendedMidiClip.getSnapLevel());

		auto timeOffset = te::TimeDuration::fromSeconds(0.0001);

		if (x > 0)
		{
			auto nextTime = snapType.roundTimeUp(timePosition + timeOffset, tempoSequence, extendedMidiClip.getSnapUseTriplets());
			cursorPosition.setX(tempoSequence.timeToBeats(nextTime).inBeats());
		}
		else
		{
			auto nextTime = snapType.roundTimeDown(timePosition - timeOffset, tempoSequence, extendedMidiClip.getSnapUseTriplets());
			cursorPosition.setX(tempoSequence.timeToBeats(nextTime).inBeats());
		}

		cursorPosition.setX(std::clamp(cursorPosition.getX(), 0.0, 512.0));
	}

	cursorPosition.setY(std::clamp(cursorPosition.getY() + delta.getY(), 0.0, 127.0));
	updateBounds();
}

void MidiEventsEditorCursorComponent::updateBounds()
{
	auto& extendedMidiClip = uiContext.extendedMidiClip;

	int margin = 2;

	auto rect = juce::Rectangle<int>(
		cursorPosition.getX() / extendedMidiClip.getBeatsPerPixel() - margin,
		cursorPosition.getY() * extendedMidiClip.getNoteHeight() - margin,
		margin * 2,
		extendedMidiClip.getNoteHeight() + margin * 2
	);

	auto bounds = getBounds();
	if (rect != bounds)
	{
		setBounds(rect);
		DBG(rect.toString());
	}
}
