#include "TrackListDeleteToolState.h"
#include "TrackListComponent.h"

TrackListDeleteToolState::TrackListDeleteToolState(TrackListComponent* component)
	: TrackListToolState(component, FumeIDs::TrackListDeleteToolState)
{
}

void TrackListDeleteToolState::onEnterState()
{
}

void TrackListDeleteToolState::onExitState()
{
}

bool TrackListDeleteToolState::canHandle(const juce::KeyPress& key)
{
	if(!component)
	{
		return false;
	}
	auto& inputManager = component->getUIContext().inputManager;
	return inputManager.validate(key.getKeyCode(), key.getModifiers(), InputAction::Delete);
}

bool TrackListDeleteToolState::keyPressed(const juce::KeyPress& key)
{
	if (!component)
	{
		return false;
	}
	auto& inputManager = component->getUIContext().inputManager;
	return inputManager.validate(key.getKeyCode(), key.getModifiers(), InputAction::Delete);
}

bool TrackListDeleteToolState::keyStateChanged(bool isKeyDown)
{
	if (!component)
	{
		return false;
	}
	auto& inputManager = component->getUIContext().inputManager;

	if (!isKeyDown && !inputManager.isCurrentlyPressed(InputAction::Delete))
	{
		auto& uiContext = component->getUIContext();
		auto& selectionManager = uiContext.selectionManager;
		selectionManager.deleteSelected();
		notifyToolFinished();
		return true;
	}
	return false;
}
