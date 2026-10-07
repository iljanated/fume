#include "TrackListWriteToolState.h"
#include "TrackListComponent.h"

namespace te = tracktion;

TrackListWriteToolState::TrackListWriteToolState(TrackListComponent& c, InputManager& i, ToolStateListener& l)
	: ToolState<TrackListComponent>(c, i, l),
	drawableCursor(component.getDrawableCursor()),
	cursor(component.getCursor()),
	cursorAnchor(component.getCursorAnchor()),
	selectionManager(component.getSelectionManager())
{
}


TrackListWriteToolState::~TrackListWriteToolState()
{
}


void TrackListWriteToolState::onEnterState()
{
	drawableCursor.setFill(juce::FillType(juce::Colours::indianred.withAlpha(0.5f)));
	drawableCursor.setStrokeFill(juce::Colours::orangered);
	drawableCursor.setStrokeThickness(2.0f);
}

void TrackListWriteToolState::onExitState()
{
}

void TrackListWriteToolState::onInputAction(InputManagerAction action, int inputMask, bool isActive)
{
	if (action == InputManagerAction::DPAD_LEFT && isActive)
	{
		if (isDragging)
		{
			cursor.setX(std::max(cursor.getX() - 1.0, 0.0));
			updateDrawableCursor();
		}
		else
		{
			cursor.setX(std::max(cursor.getX() - 1.0, 0.0));
			cursorAnchor.setX(cursor.getX());
			updateDrawableCursor();
		}
	}
	else if (action == InputManagerAction::DPAD_RIGHT && isActive)
	{
		if (isDragging)
		{
			cursor.setX(cursor.getX() + 1.0);
			updateDrawableCursor();
		}
		else
		{
			cursor.setX(cursor.getX() + 1.0);
			cursorAnchor.setX(cursor.getX());
			updateDrawableCursor();
		}
	}
	else if (action == InputManagerAction::DPAD_UP && isActive)
	{
		cursor.setY(std::max(cursor.getY() - 1.0, 0.0));
		cursorAnchor.setY(cursor.getY());
		updateDrawableCursor();
	}
	else if (action == InputManagerAction::DPAD_DOWN && isActive)
	{
		double trackCount = (double)component.getTrackCount();
		cursor.setY(std::min(cursor.getY() + 1.0, trackCount - 1.0));
		cursorAnchor.setY(cursor.getY());
		updateDrawableCursor();
	}
	else if (action == InputManagerAction::B)
	{
		if (isActive)
		{
			selectionManager.deselectAll();
			cursorAnchor.setXY(cursor.getX(), cursor.getY());
			updateDrawableCursor();
			isDragging = true;
		}
		else
		{
			auto trackIndex = (int)cursor.getY();
			auto track = component.getTrackAt(trackIndex);

			if (std::abs(cursor.getX() - cursorAnchor.getX()) > 0.01)
			{
				auto startBeat = te::BeatPosition::fromBeats(std::min(cursor.getX(), cursorAnchor.getX()));
				auto endBeat = te::BeatPosition::fromBeats(std::max(cursor.getX(), cursorAnchor.getX()));

				te::TempoSequence& tempoSequence = track->edit.tempoSequence;
				auto midiClip = track->insertMIDIClip(tempoSequence.toTime({ startBeat, endBeat }), nullptr);

				selectionManager.addToSelection(midiClip);

				cursor.setX(std::max(cursor.getX(), cursorAnchor.getX()));
				cursorAnchor.setXY(cursor.getX(), cursor.getY());
				updateDrawableCursor();

			}
			isDragging = false;
		}
	}
}

void TrackListWriteToolState::updateDrawableCursor()
{
	auto gridWidth = std::max(std::abs(cursor.getX() - cursorAnchor.getX()) * FUME_BEAT_WIDTH, 0.01);
	auto gridHeight = FUME_TRACK_HEIGHT;
	auto x = std::min(cursor.getX(), cursorAnchor.getX()) * FUME_BEAT_WIDTH;
	auto y = std::min(cursor.getY(), cursorAnchor.getY()) * FUME_TRACK_HEIGHT;

	drawableCursor.setRectangle(Rectangle<float>(x, y, gridWidth, gridHeight));
}