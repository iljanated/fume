#include "MidiClipDeleteToolState.h"
#include "MidiEventsEditorComponent.h"

MidiClipDeleteToolState::MidiClipDeleteToolState(MidiEventsEditorComponent* component)
	: MidiClipToolState(component, FumeIDs::MidiClipDeleteToolState)
{
}

void MidiClipDeleteToolState::onEnterState()
{
	if (component)
	{
	}
}

void MidiClipDeleteToolState::onExitState()
{
}

bool MidiClipDeleteToolState::canHandle(const juce::KeyPress& key)
{
	if (!component)
	{
		return false;
	}
	auto& inputManager = component->getUIContext().inputManager;
	return inputManager.validate(key.getKeyCode(), key.getModifiers(), InputAction::Delete);
}

bool MidiClipDeleteToolState::keyPressed(const juce::KeyPress& key)
{
	if (!component)
	{
		return false;
	}
	auto& inputManager = component->getUIContext().inputManager;
	return inputManager.validate(key.getKeyCode(), key.getModifiers(), InputAction::Delete);
}

bool MidiClipDeleteToolState::keyStateChanged(bool isKeyDown)
{
	if (!component)
	{
		return false;
	}
	
	auto& inputManager = component->getUIContext().inputManager;

	if (!isKeyDown && !inputManager.isCurrentlyPressed(InputAction::Delete))
	{
		auto& selectedEvents = component->getSelectedEvents(true);

		auto& uiContext = component->getUIContext();
		auto& undoManager = uiContext.extendedEdit.getUndoManager();
		
		auto& clip = uiContext.extendedMidiClip.getClip();

		auto notesToDelete = selectedEvents.getSelectedNotes();

		for (auto* note : notesToDelete)
		{
			clip.getSequence().removeNote(*note, &undoManager);
		}

		component->clearSelectedEvents();
		
		notifyToolFinished();
		
		return true;
	}
	
	return false;
}
