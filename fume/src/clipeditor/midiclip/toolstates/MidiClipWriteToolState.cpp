#include "MidiClipWriteToolState.h"

#include "MidiEventsEditorComponent.h"

MidiClipWriteToolState::MidiClipWriteToolState(MidiEventsEditorComponent* component)
	: ToolState<MidiEventsEditorComponent>(component, FumeIDs::MidiClipWriteToolState, InputAction::Write)
{
}

void MidiClipWriteToolState::onEnterState()
{
}

void MidiClipWriteToolState::onExitState()
{
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

	if (inputManager.validate(keyCode, modifiers, inputAction) && !isWriting)
	{
		isWriting = true;
		auto& cursorComponent = component->getCursorComponent();
		cursorComponent.setVisible(false);
		auto cursorPosition = cursorComponent.getCursorPosition();

		auto startBeat = te::BeatPosition::fromBeats(cursorPosition.x);
		auto duration = te::BeatDuration::fromBeats(0.125);
		auto noteNumber = 127 - (int)cursorPosition.y;
		int velocity = 127;

		auto& sequence = component->getMidiClip().getClip().getSequence();

		auto* note = sequence.addNote(noteNumber, startBeat, duration, velocity, 0, &uiContext.edit.getUndoManager());

		auto& selectedEvents = component->clearSelectedEvents();
		selectedEvents.addSelectedEvent(note, true);

		return true;
	}
	else if (isWriting && inputManager.validate(keyCode, modifiers, InputAction::Left))
	{
		auto snappingInterval = component->getSnappingInterval();
		auto selectedNotes = component->getSelectedEvents().getSelectedNotes();

		auto& undoManager = uiContext.edit.getUndoManager();

		for (auto* note : selectedNotes)
		{
			if (note->getLengthBeats().inBeats() <= snappingInterval)
			{
				DBG("Cannot shrink note below snapping interval");
				continue;
			}
			auto startBeat = note->getStartBeat();
			auto duration = note->getLengthBeats();
			duration = duration - te::BeatDuration::fromBeats(snappingInterval);
			note->setStartAndLength(startBeat, duration, &undoManager);
			return true;
		}
	}
	else if (isWriting && inputManager.validate(keyCode, modifiers, InputAction::Right))
	{
		auto snappingInterval = component->getSnappingInterval();
		auto selectedNotes = component->getSelectedEvents().getSelectedNotes();

		auto& undoManager = uiContext.edit.getUndoManager();

		for (auto* note : selectedNotes)
		{
			auto startBeat = note->getStartBeat();
			auto duration = note->getLengthBeats();
			duration = duration + te::BeatDuration::fromBeats(snappingInterval);
			note->setStartAndLength(startBeat, duration, &undoManager);
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

	if (!isKeyDown && isWriting && !inputManager.isCurrentlyPressed(inputAction))
	{
		isWriting = false;
		auto& cursorComponent = component->getCursorComponent();
		auto selectedNotes = component->getSelectedEvents().getSelectedNotes();
		if (selectedNotes.size() > 0)
		{
			auto* note = selectedNotes.getFirst();
			auto endBeat = note->getEndBeat().inBeats() + component->getSnappingInterval();
			endBeat = endBeat - std::fmod(endBeat, component->getSnappingInterval());
			cursorComponent.setCursorPosition({ endBeat, cursorComponent.getCursorPosition().y });
		}
		cursorComponent.setVisible(true);
		notifyToolFinished();
		return true;
	}
	return false;
}