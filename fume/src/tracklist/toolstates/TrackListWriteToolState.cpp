#include "TrackListWriteToolState.h"
#include "TrackListComponent.h"

TrackListWriteToolState::TrackListWriteToolState(TrackListComponent* component)
	: TrackListToolState(component, FumeIDs::TrackListWriteToolState)
{
}

void TrackListWriteToolState::onEnterState()
{
}

void TrackListWriteToolState::onExitState()
{
}

bool TrackListWriteToolState::canHandle(const juce::KeyPress& key)
{
	if (!component)
	{
		return false;
	}
	auto& inputManager = component->getUIContext().inputManager;
	return inputManager.validate(key.getKeyCode(), key.getModifiers(), InputAction::Write);
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

	if (inputManager.validate(keyCode, modifiers, InputAction::Write) && !isWriting)
	{
		isWriting = true;
		component->showCursor(false);
		
		auto& extendedEdit = uiContext.extendedEdit;
		auto& tempoSequence = extendedEdit.getTempoSequence();

		auto startBeat = tempoSequence.timeToBeats(te::TimePosition::fromSeconds(extendedEdit.getCursorX()));
		auto endBeat = te::BeatPosition::fromBeats(startBeat.inBeats() + 4.0);

		auto trackIndex = (int)extendedEdit.getCursorY();
		auto track = component->getTrackAt(trackIndex);
		auto& selectionManager = component->getUIContext().selectionManager;

		auto midiClip = track->insertMIDIClip(tempoSequence.toTime({ startBeat, endBeat }), &selectionManager);

		extendedEdit.setCursorX(tempoSequence.beatsToTime(endBeat).inSeconds());
		
		return true;
	}
	else if (isWriting && inputManager.validate(keyCode, modifiers, InputAction::Left))
	{
		auto& extendedEdit = uiContext.extendedEdit;
		auto& tempoSequence = extendedEdit.getTempoSequence();
		auto snapType = component->getSnapType();

		auto& selectionManager = component->getUIContext().selectionManager;

		for (auto* selectable : selectionManager.getSelectedObjects())
		{
			if (auto* midiClip = dynamic_cast<tracktion_engine::MidiClip*> (selectable))
			{
				auto startTime = tempoSequence.beatsToTime(midiClip->getStartBeat());
				auto endTime = tempoSequence.beatsToTime(midiClip->getEndBeat());

				auto newEndTime = snapType.roundTimeDown(endTime - te::TimeDuration::fromSeconds(0.0001), tempoSequence);

				if (newEndTime <= startTime)
				{
					DBG("Cannot shrink clip below snapping interval");
					continue;
				}

				midiClip->setEnd(newEndTime,false);

				extendedEdit.setCursorX(newEndTime.inSeconds());
			}
		}
	}
	else if (isWriting && inputManager.validate(keyCode, modifiers, InputAction::Right))
	{
		auto& extendedEdit = uiContext.extendedEdit;
		auto& tempoSequence = extendedEdit.getTempoSequence();
		auto snapType = component->getSnapType();


		auto& selectionManager = component->getUIContext().selectionManager;

		for (auto* selectable : selectionManager.getSelectedObjects())
		{
			if (auto* midiClip = dynamic_cast<tracktion_engine::MidiClip*> (selectable))
			{
				auto endTime = tempoSequence.beatsToTime(midiClip->getEndBeat());

				auto newEndTime = snapType.roundTimeUp(endTime + te::TimeDuration::fromSeconds(0.0001), tempoSequence);

				midiClip->setEnd(newEndTime, false);

				extendedEdit.setCursorX(newEndTime.inSeconds());
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
	
	auto& inputManager = component->getUIContext().inputManager;

	if (!isKeyDown && isWriting && !inputManager.isCurrentlyPressed(InputAction::Write))
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

		if (endBeat >= 0.0)
		{
			auto uiContext = component->getUIContext();
			auto& extendedEdit = uiContext.extendedEdit;
			auto& tempoSequence = extendedEdit.getTempoSequence();
			auto snapType = component->getSnapType();

			auto endTimePosition = tempoSequence.toTime(te::BeatPosition::fromBeats(endBeat));
			auto nextTimePosition = snapType.roundTimeUp(endTimePosition, tempoSequence);

			extendedEdit.setCursorX(nextTimePosition.inSeconds());
		}
		component->showCursor(true);
		notifyToolFinished();
		return true;
	}
	
	return false;	
}