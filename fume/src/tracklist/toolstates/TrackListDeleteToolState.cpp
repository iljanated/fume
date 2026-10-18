#include "TrackListDeleteToolState.h"
#include "TrackListDeleteToolState.h"
#include "TrackListDeleteToolState.h"

#include "TrackListComponent.h"

TrackListDeleteToolState::TrackListDeleteToolState(TrackListComponent* component)
	: ToolState<TrackListComponent>(component, "Delete", InputAction::Delete)
{
}

void TrackListDeleteToolState::onEnterState()
{
	if (component)
	{
		auto& selectionBoxComponent = component->getSelectionBoxComponent();
		selectionBoxComponent.setColour(juce::Colours::yellow);
	}
}

void TrackListDeleteToolState::onExitState()
{
}

bool TrackListDeleteToolState::keyPressed(const juce::KeyPress& key)
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

	if (inputManager.validate(keyCode, modifiers, inputAction) && !selectionBoxComponent.getIsSelecting())
	{
		auto& cursorComponent = component->getCursorComponent();
		cursorComponent.setVisible(false);

		selectionBoxComponent.startSelection(cursorComponent.getCursorPosition());
		return true;
	}
	return false;
}

bool TrackListDeleteToolState::keyStateChanged(bool isKeyDown)
{
	if (!component)
	{
		return false;
	}
	auto& selectionBoxComponent = component->getSelectionBoxComponent();
	auto& inputManager = component->getUIContext().inputManager;

	if (!isKeyDown && selectionBoxComponent.getIsSelecting() && !inputManager.isCurrentlyPressed(inputAction))
	{
		auto& uiContext = component->getUIContext();
		auto& selectionManager = uiContext.selectionManager;
		selectionManager.deleteSelected();

		auto& cursorComponent = component->getCursorComponent();
		cursorComponent.setCursorPosition(selectionBoxComponent.getSelectionEnd());
		selectionBoxComponent.endSelection();
		cursorComponent.setVisible(true);
		return true;
	}
	return false;
}
