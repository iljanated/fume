#include "MidiClipDeleteToolState.h"
#include "MidiEventsEditorComponent.h"

MidiClipDeleteToolState::MidiClipDeleteToolState(MidiEventsEditorComponent* component)
	: MidiClipToolState(component, FumeIDs::MidiClipDeleteToolState, InputAction::Delete)
{
}

void MidiClipDeleteToolState::onEnterState()
{
	if (component)
	{
		auto& selectionBoxComponent = component->getSelectionBoxComponent();
		selectionBoxComponent.setColour(juce::Colours::yellow);
	}
}

void MidiClipDeleteToolState::onExitState()
{
}

bool MidiClipDeleteToolState::keyPressed(const juce::KeyPress& key)
{
	return false;
}

bool MidiClipDeleteToolState::keyStateChanged(bool isKeyDown)
{
	if (!component)
	{
		return false;
	}
	auto& inputManager = component->getUIContext().inputManager;

	if (!isKeyDown && !inputManager.isCurrentlyPressed(inputAction))
	{
		auto& selectedEvents = component->getSelectedEvents(true);

		auto& undoManager = component->getUIContext().extendedEdit.getUndoManager();
		auto& clip = component->getMidiClip().getClip();

		for (auto* note : selectedEvents.getSelectedNotes())
		{
			clip.getSequence().removeNote(*note, &undoManager);
			selectedEvents.removeSelectedEvent(note);
		}

		notifyToolFinished();
		
		return true;
	}
	return false;
}
