#include "TrackListWriteToolState.h"
#include "TrackListComponent.h"

TrackListWriteToolState::TrackListWriteToolState(TrackListComponent* component)
	: TrackListToolState(component, FumeIDs::TrackListWriteToolState, InputAction::Write)
{
}

void TrackListWriteToolState::onEnterState()
{
}

void TrackListWriteToolState::onExitState()
{
}

bool TrackListWriteToolState::keyPressed(const juce::KeyPress& key)
{
	if (!component)
	{
		return false;
	}
	auto& uiContext = component->getUIContext();
	auto& inputManager = uiContext.inputManager;

	auto modifiers = key.getModifiers();
	auto keyCode = key.getKeyCode();

	auto& selectionBoxComponent = component->getSelectionBoxComponent();

	if (inputManager.validate(keyCode, modifiers, inputAction) && !isWriting)
	{
		isWriting = true;
		auto& cursorComponent = component->getCursorComponent();
		cursorComponent.setVisible(false);
		auto cursorPosition = cursorComponent.getCursorPosition();

		auto& edit = component->getUIContext().edit;

		auto startBeat = te::BeatPosition::fromBeats(cursorPosition.x);
		auto endBeat = te::BeatPosition::fromBeats(cursorPosition.x + 4.0);

		auto trackIndex = (int)cursorPosition.y;
		auto track = component->getTrackAt(trackIndex);
		te::TempoSequence& tempoSequence = edit.tempoSequence;
		auto midiClip = track->insertMIDIClip(tempoSequence.toTime({ startBeat, endBeat }), nullptr);

		auto& selectionManager = component->getUIContext().selectionManager;
		selectionManager.deselectAll();
		selectionManager.addToSelection(midiClip);
		
		return true;
	}
	else if (isWriting && inputManager.validate(keyCode, modifiers, InputAction::Left))
	{
		auto& edit = component->getUIContext().edit;

		auto snappingInterval = component->getSnappingInterval();
		auto& selectionManager = component->getUIContext().selectionManager;

		for (auto* selectable : selectionManager.getSelectedObjects())
		{
			if (auto* midiClip = dynamic_cast<tracktion_engine::MidiClip*> (selectable))
			{
				if (midiClip->getLengthInBeats().inBeats() <= snappingInterval)
				{
					DBG("Cannot shrink clip below snapping interval");
					continue;
				}
				auto endBeat = midiClip->getEndBeat();
				auto newEndBeat = te::BeatPosition::fromBeats(endBeat.inBeats() - snappingInterval);
				auto newEndTime = edit.tempoSequence.toTime(newEndBeat);
				midiClip->setEnd(newEndTime,false);
			}
		}
	}
	else if (isWriting && inputManager.validate(keyCode, modifiers, InputAction::Right))
	{
		auto& edit = component->getUIContext().edit;

		auto snappingInterval = component->getSnappingInterval();
		auto& selectionManager = component->getUIContext().selectionManager;

		for (auto* selectable : selectionManager.getSelectedObjects())
		{
			if (auto* midiClip = dynamic_cast<tracktion_engine::MidiClip*> (selectable))
			{
				auto endBeat = midiClip->getEndBeat();
				auto newEndBeat = te::BeatPosition::fromBeats(endBeat.inBeats() + snappingInterval);
				auto newEndTime = edit.tempoSequence.toTime(newEndBeat);
				midiClip->setEnd(newEndTime, false);
				DBG("Extended clip to " << newEndBeat.inBeats() << " beats");
			}
		}
	}
	return false;
}

bool TrackListWriteToolState::keyStateChanged(bool isKeyDown)
{
	if (!component)
	{
		return false;
	}
	auto& selectionBoxComponent = component->getSelectionBoxComponent();
	auto& inputManager = component->getUIContext().inputManager;

	if (!isKeyDown && isWriting && !inputManager.isCurrentlyPressed(inputAction))
	{
		isWriting = false;

		double endBeat = -1.0;

		auto& selectionManager = component->getUIContext().selectionManager;
		for (auto* selectable : selectionManager.getSelectedObjects())
		{
			if (auto* midiClip = dynamic_cast<tracktion_engine::MidiClip*> (selectable))
			{
				endBeat = midiClip->getEndBeat().inBeats();
				break;
			}
		}

		auto& cursorComponent = component->getCursorComponent();

		if (endBeat >= 0.0)
		{
			cursorComponent.setCursorPosition({ endBeat, cursorComponent.getCursorPosition().y });
		}
		cursorComponent.setVisible(true);
		return true;
	}
	return false;	
}