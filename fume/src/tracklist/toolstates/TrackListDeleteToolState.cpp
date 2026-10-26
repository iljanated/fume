#include "TrackListDeleteToolState.h"
#include "TrackListComponent.h"

TrackListDeleteToolState::TrackListDeleteToolState(TrackListComponent* component)
	: TrackListToolState(component, FumeIDs::TrackListDeleteToolState, InputAction::Delete)
{
}

void TrackListDeleteToolState::onEnterState()
{
}

void TrackListDeleteToolState::onExitState()
{
}

bool TrackListDeleteToolState::keyPressed(const juce::KeyPress& key)
{
	return false;
}

bool TrackListDeleteToolState::keyStateChanged(bool isKeyDown)
{
	if (!component)
	{
		return false;
	}
	auto& inputManager = component->getUIContext().inputManager;

	if (!isKeyDown && !inputManager.isCurrentlyPressed(inputAction))
	{
		auto& uiContext = component->getUIContext();
		auto& selectionManager = uiContext.selectionManager;
		selectionManager.deleteSelected();
		notifyToolFinished();
		return true;
	}
	return false;
}
