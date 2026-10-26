#include "MidiClipMoveToolState.h"
#include "MidiEventsEditorComponent.h"

MidiClipMoveToolState::MidiClipMoveToolState(MidiEventsEditorComponent* component)
	: MidiClipToolState(component, FumeIDs::MidiClipMoveToolState, InputAction::None)
{
}

void MidiClipMoveToolState::onEnterState()
{
	if (component)
	{
	}
}

void MidiClipMoveToolState::onExitState()
{
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
		auto& undoManager = uiContext.edit.getUndoManager();
		double delta = -component->getSnappingInterval();
		for (auto* note : selectedNotes)
		{
			moveNote(note, delta, 0, undoManager);
		}
		return true;
	}
	else if (inputManager.validate(keyCode, modifiers, InputAction::MoveRight))
	{
		auto& undoManager = uiContext.edit.getUndoManager();
		double delta = component->getSnappingInterval();
		for (auto* note : selectedNotes)
		{
			moveNote(note, delta, 0, undoManager);
		}
		return true;
	}
	else if (inputManager.validate(keyCode, modifiers, InputAction::MoveUp))
	{
		auto& undoManager = uiContext.edit.getUndoManager();
		int delta = 1;
		for (auto* note : selectedNotes)
		{
			moveNote(note, 0, delta, undoManager);
		}
		return true;
	}
	else if (inputManager.validate(keyCode, modifiers, InputAction::MoveDown))
	{
		auto& undoManager = uiContext.edit.getUndoManager();
		int delta = -1;
		for (auto* note : selectedNotes)
		{
			moveNote(note, 0, delta, undoManager);
		}
		return true;
	}

	return false;
}

bool MidiClipMoveToolState::keyStateChanged(bool isKeyDown)
{
	return false;
}

void MidiClipMoveToolState::moveNote(te::MidiNote* note, double deltaBeats, int deltaPitch, juce::UndoManager& undoManager)
{
	if (component && deltaBeats != 0.0)
	{
		auto length = note->getLengthBeats();
		auto newStart = note->getStartBeat() + te::BeatDuration::fromBeats(deltaBeats);

		auto& clip = component->getMidiClip().getClip();
		if(newStart.inBeats() >= 0 && newStart.inBeats() < clip.getLengthInBeats().inBeats())
		{
			note->setStartAndLength(newStart, length, &undoManager);
		}
	}
	
	if (deltaPitch != 0)
	{
		auto newPitch = note->getNoteNumber() + deltaPitch;
		if (newPitch >= 0 && newPitch <= 127)
		{
			note->setNoteNumber(newPitch, &undoManager);
		}
	}
}
