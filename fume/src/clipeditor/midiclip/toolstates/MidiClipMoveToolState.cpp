#include "MidiClipMoveToolState.h"
#include "MidiEventsEditorComponent.h"

MidiClipMoveToolState::MidiClipMoveToolState(MidiEventsEditorComponent* component)
	: MidiClipToolState(component, FumeIDs::MidiClipMoveToolState)
{
}

void MidiClipMoveToolState::onEnterState()
{
	if (component)
	{
		auto uiContext = component->getUIContext();
		auto& undoManager = uiContext.extendedEdit.getUndoManager();
		undoManager.beginNewTransaction("Move Note");
	}
}

void MidiClipMoveToolState::onExitState()
{
}

bool MidiClipMoveToolState::canHandle(const juce::KeyPress& key)
{
	if (!component)
	{
		return false;
	}
	auto& inputManager = component->getUIContext().inputManager;
	if (inputManager.validate(key.getKeyCode(), key.getModifiers(), InputAction::MoveLeft) ||
		inputManager.validate(key.getKeyCode(), key.getModifiers(), InputAction::MoveRight) ||
		inputManager.validate(key.getKeyCode(), key.getModifiers(), InputAction::MoveUp) ||
		inputManager.validate(key.getKeyCode(), key.getModifiers(), InputAction::MoveDown))
	{
		return true;
	}
	return false;
}

bool MidiClipMoveToolState::keyPressed(const juce::KeyPress& key)
{
	if (!component)
	{
		return false;
	}
	auto& selectedNotes = component->getSelectedEvents().getSelectedNotes();
	if(selectedNotes.isEmpty())
	{
		return false;
	}

	auto keyCode = key.getKeyCode();
	auto modifiers = key.getModifiers();

	auto& uiContext = component->getUIContext();
	auto& inputManager = uiContext.inputManager;

	if (inputManager.validate(keyCode, modifiers, InputAction::MoveLeft))
	{
		for (auto* note : selectedNotes)
		{
			moveNote(note, juce::Point<int>(-1, 0));
		}
		return true;
	}
	else if (inputManager.validate(keyCode, modifiers, InputAction::MoveRight))
	{
		for (auto* note : selectedNotes)
		{
			moveNote(note, juce::Point<int>(1, 0));
		}
		return true;
	}
	else if (inputManager.validate(keyCode, modifiers, InputAction::MoveUp))
	{
		for (auto* note : selectedNotes)
		{
			moveNote(note, juce::Point<int>(0, 1));
		}
		return true;
	}
	else if (inputManager.validate(keyCode, modifiers, InputAction::MoveDown))
	{
		for (auto* note : selectedNotes)
		{
			moveNote(note, juce::Point<int>(0, -1));
		}
		return true;
	}
	
	return false;
}

bool MidiClipMoveToolState::keyStateChanged(bool isKeyDown)
{
	auto& inputManager = component->getUIContext().inputManager;

	if (!isKeyDown && !inputManager.isCurrentlyPressed(InputAction::MoveLeft) &&
		!inputManager.isCurrentlyPressed(InputAction::MoveRight) &&
		!inputManager.isCurrentlyPressed(InputAction::MoveUp) &&
		!inputManager.isCurrentlyPressed(InputAction::MoveDown))
	{
		notifyToolFinished();
		return true;
	}
	return false;
}

void MidiClipMoveToolState::moveNote(te::MidiNote* note, juce::Point<int> delta)
{
	auto uiContext = component->getUIContext();
	auto& undoManager = uiContext.extendedEdit.getUndoManager();
	if (delta.x != 0)
	{
		auto length = note->getLengthBeats();
		auto newStart = note->getStartBeat() + te::BeatDuration::fromBeats(delta.getX() * 0.125);

		auto& clip = uiContext.extendedMidiClip.getClip();
		if(newStart.inBeats() >= 0 && newStart.inBeats() < clip.getLengthInBeats().inBeats())
		{
			note->setStartAndLength(newStart, length, &undoManager);
		}
	}
	
	if (delta.getY() != 0)
	{
		auto newPitch = note->getNoteNumber() + delta.getY();
		if (newPitch >= 0 && newPitch <= 127)
		{
			note->setNoteNumber(newPitch, &undoManager);
		}
	}
}
