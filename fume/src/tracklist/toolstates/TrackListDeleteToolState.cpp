#pragma once

#include "TrackListDeleteToolState.h"
#include "TrackListComponent.h"


TrackListDeleteToolState::TrackListDeleteToolState(TrackListComponent& c, InputManager& i, ToolStateListener& l) : ToolState<TrackListComponent>(c, i, l),
	selectionManager(component.getSelectionManager())
{
}


TrackListDeleteToolState::~TrackListDeleteToolState()
{
}


void TrackListDeleteToolState::onEnterState()
{
}

void TrackListDeleteToolState::onExitState()
{
}

void TrackListDeleteToolState::onInputAction(InputManagerAction action, int inputMask, bool isActive)
{
	if (action == InputManagerAction::Y)
	{
		if (isActive)
		{
			selectionManager.deleteSelected();
			component.onToolStateFinished();
		}
	}
}