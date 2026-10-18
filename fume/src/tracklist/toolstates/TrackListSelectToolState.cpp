#include "TrackListSelectToolState.h"

#include "TrackListComponent.h"

TrackListSelectToolState::TrackListSelectToolState(TrackListComponent* component)
	: ToolState<TrackListComponent>(component, FumeIDs::TrackListSelectToolState, InputAction::Select)
{
}

void TrackListSelectToolState::onEnterState()
{
	if (component)
	{
		auto& selectionBoxComponent = component->getSelectionBoxComponent();
		selectionBoxComponent.setColour(juce::Colours::white);
	}
}

void TrackListSelectToolState::onExitState()
{
}

bool TrackListSelectToolState::keyPressed(const juce::KeyPress& key)
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

bool TrackListSelectToolState::keyStateChanged(bool isKeyDown)
{
	if (!component)
	{
		return false;
	}
	auto& selectionBoxComponent = component->getSelectionBoxComponent();
	auto& inputManager = component->getUIContext().inputManager;

	if (!isKeyDown && selectionBoxComponent.getIsSelecting() && !inputManager.isCurrentlyPressed(inputAction))
	{

		auto& cursorComponent = component->getCursorComponent();
		cursorComponent.setCursorPosition(selectionBoxComponent.getSelectionEnd());
		selectionBoxComponent.endSelection();
		cursorComponent.setVisible(true);
		return true;
	}
	return false;
}
