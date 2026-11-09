#include "MidiClipZoomToolState.h"
#include "MidiEventsEditorComponent.h"
#include "Utilities.h"

MidiClipZoomToolState::MidiClipZoomToolState(MidiEventsEditorComponent* component)
	: MidiClipToolState(component, FumeIDs::MidiClipZoomToolState, InputAction::Zoom)
{
}

void MidiClipZoomToolState::onEnterState()
{
	if (component)
	{
	}
}

void MidiClipZoomToolState::onExitState()
{
}

bool MidiClipZoomToolState::keyPressed(const juce::KeyPress& key)
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
	else if (isZooming && inputManager.validate(keyCode, InputAction::Left))
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

bool MidiClipZoomToolState::keyStateChanged(bool isKeyDown)
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

void MidiClipZoomToolState::zoom(float x, float y)
{
	if (component)
	{
		auto& extendedMidiClip = component->getUIContext().extendedMidiClip;

		auto zoomX = extendedMidiClip.getZoomX();
		auto zoomY = extendedMidiClip.getZoomY();

		auto zoomStepSize = fumeUI::zoomStepSize;

		zoomX = std::clamp(zoomX + x * zoomStepSize, 1.0f - zoomStepSize * 9.0f, 1.0f + zoomStepSize * 10.0f);
		zoomY = std::clamp(zoomY + y * zoomStepSize, 1.0f - zoomStepSize * 2, 1.0f + zoomStepSize * 20.0f);

		extendedMidiClip.setZoomX(zoomX);
		extendedMidiClip.setZoomY(zoomY);
	}
}