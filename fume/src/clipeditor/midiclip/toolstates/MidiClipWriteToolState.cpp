#include "MidiClipWriteToolState.h"
#include "MidiEventsEditorComponent.h"

MidiClipWriteToolState::MidiClipWriteToolState(MidiEventsEditorComponent* component)
	: MidiClipToolState(component, FumeIDs::MidiClipWriteToolState)
{
}

void MidiClipWriteToolState::onEnterState()
{
}

void MidiClipWriteToolState::onExitState()
{
}

bool MidiClipWriteToolState::canHandle(const juce::KeyPress& key)
{
	if(!component)
	{
		return false;
	}
	auto& inputManager = component->getUIContext().inputManager;
	return inputManager.validate(key.getKeyCode(), key.getModifiers(), InputAction::Write);
}

bool MidiClipWriteToolState::keyPressed(const juce::KeyPress& key)
{
	if (!component)
	{
		return false;
	}

	auto& uiContext = component->getUIContext();
	auto& inputManager = uiContext.inputManager;
	auto modifiers = key.getModifiers();
	auto keyCode = key.getKeyCode();

	auto& selectedEvents = component->getSelectedEvents();
	auto& extendedMidiClip = uiContext.extendedMidiClip;

	auto& undoManager = uiContext.extendedEdit.getEdit().getUndoManager();

	if (inputManager.validate(keyCode, modifiers, InputAction::Write) && !isWriting)
	{
		auto clipStartBeat = extendedMidiClip.getClip().getStartBeat();
		auto& tempoSequence = uiContext.extendedEdit.getTempoSequence();

		auto startBeat = te::BeatPosition::fromBeats(tempoSequence.timeToBeats(te::TimePosition::fromSeconds(extendedMidiClip.getCursorX())).inBeats() - clipStartBeat.inBeats());

		if (startBeat.inBeats() < 0 || startBeat.inBeats() > extendedMidiClip.getClip().getLengthInBeats().inBeats())
		{
			DBG("Cannot write note before start of clip");
			return false;
		}

		isWriting = true;
		
		component->showCursor(false);
		
		auto duration = te::BeatDuration::fromBeats(extendedMidiClip.getNoteLength());
		auto noteNumber = 127 - static_cast<int>(extendedMidiClip.getCursorY());
		int velocity = extendedMidiClip.getNoteVelocity();

		auto& sequence = extendedMidiClip.getClip().getSequence();

		undoManager.beginNewTransaction("Add Note");

		auto* note = sequence.addNote(noteNumber, startBeat, duration, velocity, 0, &undoManager);

		auto& selectedEvents = component->clearSelectedEvents();
		selectedEvents.addSelectedEvent(note, true);

		return true;
	}
	else if (isWriting && inputManager.validate(keyCode, modifiers, InputAction::Left))
	{
		auto selectedNotes = component->getSelectedEvents().getSelectedNotes();
		
		for (auto* note : selectedNotes)
		{
			auto startBeat = note->getStartBeat();
			auto endBeat = note->getEndBeat();
			if (note->getLengthBeats().inBeats() <= lengthSnappingInterval)
			{
				DBG("Cannot shrink note below snapping interval");
				continue;
			}
			auto newEndBeat = te::BeatPosition::fromBeats(endBeat.inBeats() - lengthSnappingInterval);
			auto newLength = newEndBeat - startBeat;
			note->setStartAndLength(startBeat, newLength, &undoManager);
			extendedMidiClip.setNoteLength(newLength.inBeats());
			
		}
		return true;
	}
	else if (isWriting && inputManager.validate(keyCode, modifiers, InputAction::Right))
	{
		auto selectedNotes = component->getSelectedEvents().getSelectedNotes();

		for (auto* note : selectedNotes)
		{
			auto startBeat = note->getStartBeat();
			auto endBeat = note->getEndBeat();
			auto newEndBeat = te::BeatPosition::fromBeats(endBeat.inBeats() + lengthSnappingInterval);
			auto newLength = newEndBeat - startBeat;
			note->setStartAndLength(startBeat, newLength, &undoManager);
			extendedMidiClip.setNoteLength(newLength.inBeats());
		}
		return true;
	}

	return false;
}

bool MidiClipWriteToolState::keyStateChanged(bool isKeyDown)
{
	if (!component)
	{
		return false;
	}

	auto& inputManager = component->getUIContext().inputManager;

	if (!isKeyDown && isWriting && !inputManager.isCurrentlyPressed(InputAction::Write))
	{
		isWriting = false;
		auto selectedNotes = component->getSelectedEvents().getSelectedNotes();
		if (selectedNotes.size() > 0)
		{
			auto& uiContext = component->getUIContext();
			auto& extendedMidiClip = uiContext.extendedMidiClip;

			auto* note = selectedNotes.getFirst();
			// move cursor to end of note
		}
		component->showCursor(true);
		notifyToolFinished();
		return true;
	}
	return false;
}