#include "TrackListZoomToolState.h"
#include "TrackListComponent.h"
#include "Utilities.h"

TrackListZoomToolState::TrackListZoomToolState(TrackListComponent* component)
	: TrackListToolState(component, FumeIDs::TrackListZoomToolState, InputAction::Zoom)
{
}

void TrackListZoomToolState::onEnterState()
{
	if (component)
	{
	}
}

void TrackListZoomToolState::onExitState()
{
}

bool TrackListZoomToolState::keyPressed(const juce::KeyPress& key)
{
	if (!component)
	{
		return false;
	}
	
	auto keyCode = key.getKeyCode();
	auto modifiers = key.getModifiers();

	auto& uiContext = component->getUIContext();
	auto& inputManager = uiContext.inputManager;

	if (inputManager.validate(keyCode, modifiers, InputAction::Zoom))
	{
		isZooming = true;
		return true;
	}
	else if (isZooming &&inputManager.validate(keyCode, InputAction::Left))
	{
		zoom(-1.0f, 0.0f);
		return true;
	}
	else if (isZooming && inputManager.validate(keyCode, InputAction::Right))
	{
		zoom(1.0f, 0.0f);
		return true;
	}
	else if (isZooming && inputManager.validate(keyCode, InputAction::Up))
	{
		zoom(0.0f, -1.0f);
		return true;
	}
	else if (isZooming && inputManager.validate(keyCode, InputAction::Down))
	{
		zoom(0.0f, 1.0f);
		return true;
	}
	return false;
}

bool TrackListZoomToolState::keyStateChanged(bool isKeyDown)
{
	if (!component)
	{
		return false;
	}
	auto& inputManager = component->getUIContext().inputManager;

	if (!isKeyDown && !inputManager.isCurrentlyPressed(inputAction))
	{
		isZooming = false;
		notifyToolFinished();
		return true;
	}
	return false;
}

void TrackListZoomToolState::zoom(float x, float y)
{
	if (component)
	{
		auto& extendedEdit = component->getExtendedEdit();

		auto zoomX = extendedEdit.zoomX.get();
		auto zoomY = extendedEdit.zoomY.get();

		auto zoomStepSize = fumeUI::zoomStepSize;

		zoomX = std::clamp(zoomX + x * zoomStepSize, 1.0f - zoomStepSize * 5, 1.0f + zoomStepSize * 10.0f);
		zoomY = std::clamp(zoomY + y * zoomStepSize, 1.0f - zoomStepSize * 5, 1.0f + zoomStepSize * 10.0f);

		extendedEdit.zoomX.setValue(zoomX, nullptr);
		extendedEdit.zoomY.setValue(zoomY, nullptr);
	}
}