#pragma once

#include "TrackListDeleteToolState.h"
#include "TrackListComponent.h"


TrackListDeleteToolState::TrackListDeleteToolState(TrackListComponent& c, InputManager& i, ToolStateListener& l) : ToolState<TrackListComponent>(c, i, l),
drawableCursor(component.getDrawableCursor()),
cursor(component.getCursor()),
cursorAnchor(component.getCursorAnchor()),
selectionManager(component.getSelectionManager())
{
}


TrackListDeleteToolState::~TrackListDeleteToolState()
{
}


void TrackListDeleteToolState::onEnterState()
{
	drawableCursor.setFill(juce::FillType(juce::Colours::yellow.withAlpha(0.5f)));
	drawableCursor.setStrokeFill(juce::Colours::yellowgreen);
	drawableCursor.setStrokeThickness(2.0f);

	selectionManager.deleteSelected();
}

void TrackListDeleteToolState::onExitState()
{
}

void TrackListDeleteToolState::onInputAction(InputManagerActionId actionId, int inputMask, bool isActive)
{
	if (actionId == InputManagerActionId::LEFT && isActive)
	{
		if (isDragging)
		{
			cursor.setX(std::max(cursor.getX() - 1.0, 0.0));
			updateSelection();
			updateDrawableCursor();
		}
		else
		{
			cursor.setX(std::max(cursor.getX() - 1.0, 0.0));
			cursorAnchor.setX(cursor.getX());
			updateDrawableCursor();
		}
	}
	else if (actionId == InputManagerActionId::RIGHT && isActive)
	{
		if (isDragging)
		{
			cursor.setX(cursor.getX() + 1.0);
			updateSelection();
			updateDrawableCursor();
		}
		else
		{
			cursor.setX(cursor.getX() + 1.0);
			cursorAnchor.setX(cursor.getX());
			updateDrawableCursor();
		}
	}
	else if (actionId == InputManagerActionId::UP && isActive)
	{
		if (isDragging)
		{
			cursor.setY(std::max(cursor.getY() - 1.0, 0.0));
			updateSelection();
			updateDrawableCursor();
		}
		else
		{
			cursor.setY(std::max(cursor.getY() - 1.0, 0.0));
			cursorAnchor.setY(cursor.getY());
			updateDrawableCursor();
		}
	}
	else if (actionId == InputManagerActionId::DOWN && isActive)
	{
		if (isDragging)
		{
			cursor.setY(std::min(cursor.getY() + 1.0, (double)component.getTrackCount() - 1.0));
			updateSelection();
			updateDrawableCursor();
		}
		else
		{
			cursor.setY(std::min(cursor.getY() + 1.0, (double)component.getTrackCount() - 1.0));
			cursorAnchor.setY(cursor.getY());
			updateDrawableCursor();
		}
	}
	else if (actionId == InputManagerActionId::DELETE)
	{
		if (isActive)
		{
			if (!isDragging)
			{
				cursorAnchor.setXY(cursor.getX(), cursor.getY());
				updateSelection();
				updateDrawableCursor();
				isDragging = true;
			}
		}
		else
		{
			if (isDragging)
			{
				selectionManager.deleteSelected();
			}
			cursorAnchor.setXY(cursor.getX(), cursor.getY());
			updateDrawableCursor();
			isDragging = false;
		}
	}
}

void TrackListDeleteToolState::updateSelection()
{
	auto xMin = std::min(cursor.getX(), cursorAnchor.getX());
	auto xMax = std::max(cursor.getX(), cursorAnchor.getX());
	auto yMin = (int)std::min(cursor.getY(), cursorAnchor.getY());
	auto yMax = (int)std::max(cursor.getY(), cursorAnchor.getY());

	for (auto* selectable : selectionManager.getSelectedObjects())
	{
		if (te::Clip* clip = dynamic_cast<te::Clip*> (selectable))
		{
			if (auto* track = clip->getTrack())
			{
				int trackIndex = track->getIndexInEditTrackList();
				if (trackIndex < yMin || trackIndex > yMax || clip->getEndBeat().inBeats() < xMin || clip->getStartBeat().inBeats() > xMax)
				{
					selectionManager.deselect(clip);
				}
			}
			else
			{
				selectionManager.deselect(clip);
			}
		}
	}

	for (int i = yMin; i <= yMax; ++i)
	{
		if (auto track = component.getTrackAt(i))
		{
			for (auto clip : track->getClips())
			{
				if (clip->getStartBeat().inBeats() <= xMax && clip->getEndBeat().inBeats() >= xMin)
				{
					selectionManager.addToSelection(*clip);
				}
			}
		}
	}
}

void TrackListDeleteToolState::updateDrawableCursor()
{
	auto gridWidth = std::max(std::abs(cursor.getX() - cursorAnchor.getX()) * FUME_BEAT_WIDTH, 0.01);
	auto gridHeight = (std::abs(cursor.getY() - cursorAnchor.getY()) + 1.0) * FUME_TRACK_HEIGHT;
	auto x = std::min(cursor.getX(), cursorAnchor.getX()) * FUME_BEAT_WIDTH;
	auto y = std::min(cursor.getY(), cursorAnchor.getY()) * FUME_TRACK_HEIGHT;

	drawableCursor.setRectangle(Rectangle<float>(x, y, gridWidth, gridHeight));
}