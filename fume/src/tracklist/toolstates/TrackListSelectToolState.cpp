#pragma once

#include "TrackListSelectToolState.h"
#include "TrackListComponent.h"


TrackListSelectToolState::TrackListSelectToolState(TrackListComponent& c, fumeUI::UIContext& context, ToolStateListener& l) : ToolState<TrackListComponent>(c, context, l),
drawableCursor(component.getDrawableCursor()),
cursor(component.getCursor()),
cursorAnchor(component.getCursorAnchor())
{
}


TrackListSelectToolState::~TrackListSelectToolState()
{
}


void TrackListSelectToolState::onEnterState()
{
	drawableCursor.setFill(juce::FillType(juce::Colours::aliceblue.withAlpha(0.5f)));
	drawableCursor.setStrokeFill(juce::Colours::white);
	drawableCursor.setStrokeThickness(2.0f);
}

void TrackListSelectToolState::onExitState()
{
}

void TrackListSelectToolState::onInputAction(InputManagerActionId actionId, int inputMask, bool isActive)
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
	else if (actionId == InputManagerActionId::SELECT)
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
			cursorAnchor.setXY(cursor.getX(), cursor.getY());
			updateDrawableCursor();
			isDragging = false;
		}
	}
}

void TrackListSelectToolState::updateSelection()
{
	auto& selectionManager = uiContext.selectionManager;

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
				int trackIndex = fumeHelpers::getTrackIndex(uiContext.edit, track);
				if (trackIndex < yMin || trackIndex > yMax || clip->getEndBeat().inBeats() < xMin || clip->getStartBeat().inBeats() > xMax)
				{
					DBG("Deselecting clip: " << clip->getName() << " at track index: " << trackIndex);
					DBG("minX: " << xMin << ", maxX: " << xMax << ", minY: " << yMin << ", maxY: " << yMax);
					selectionManager.deselect(clip);
				}
			}
			else
			{
				DBG("Deselecting clip: " << clip->getName() << " because it has no track");
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
					if(!selectionManager.isSelected(*clip))
					{
						selectionManager.addToSelection(*clip);
					}
				}
			}
		}
	}
}

void TrackListSelectToolState::updateDrawableCursor()
{
	auto gridWidth = std::max(std::abs(cursor.getX() - cursorAnchor.getX()) * FUME_BEAT_WIDTH, 0.01);
	auto gridHeight = (std::abs(cursor.getY() - cursorAnchor.getY()) + 1.0) * FUME_TRACK_HEIGHT;
	auto x = std::min(cursor.getX(), cursorAnchor.getX()) * FUME_BEAT_WIDTH;
	auto y = std::min(cursor.getY(), cursorAnchor.getY()) * FUME_TRACK_HEIGHT;

	drawableCursor.setRectangle(Rectangle<float>(x, y, gridWidth, gridHeight));
}