#include "EditContainerComponent.h"
#include "ExtendedEdit.h"

namespace te = tracktion;
using namespace te::literals;

EditContainerComponent::EditContainerComponent(te::Engine& engine, te::Edit& edit, te::SelectionManager& selectionManager, InputManager& inputManager)
	: extendedEdit{ edit },  uiContext {engine, extendedEdit, selectionManager, inputManager }
{
	addAndMakeVisible(transportComponent);
	
	addAndMakeVisible(carrouselComponent);
	carrouselComponent.addComponent(&trackListContainerComponent);
	carrouselComponent.addComponent(&clipEditorComponent);
	carrouselComponent.addComponent(&pluginChainComponent);
	carrouselComponent.showComponent(0);
	
	// Start de transport direct
	edit.getTransport().play(false);
}

EditContainerComponent::~EditContainerComponent()
{
}

void EditContainerComponent::visibilityChanged()
{
	if (isShowing())
	{
		trackListContainerComponent.grabKeyboardFocus();
	}
}	

void EditContainerComponent::resized()
{
	auto localBounds = getLocalBounds();
	transportComponent.setBounds(localBounds.removeFromBottom(30));
	carrouselComponent.setBounds(localBounds);
}

bool EditContainerComponent::keyPressed(const juce::KeyPress& key)
{
	auto& edit = extendedEdit.getEdit();
	auto keyCode = key.getKeyCode();
	auto modifiers = key.getModifiers();
	auto& inputManager = uiContext.inputManager;
	if (inputManager.validate(keyCode, modifiers, InputAction::Undo))
	{
		edit.getUndoManager().undo();
		return true;
	}
	else if (inputManager.validate(keyCode, modifiers, InputAction::Redo))
	{
		edit.getUndoManager().redo();
		return true;
	}
	else if (inputManager.validate(keyCode, modifiers, InputAction::Play))
	{
		if (edit.getTransport().isPlaying())
		{
			edit.getTransport().stop(false, false);
		}
		else
		{
			edit.getTransport().play(false);
		}
		return true;
	}
	else if (inputManager.validate(keyCode, modifiers, InputAction::PlayFromStart))
	{
		edit.getTransport().playFromStart(false);
		return true;
	}
	else if (inputManager.validate(keyCode, modifiers, InputAction::NextLeft))
	{
		carrouselComponent.showPreviousComponent();
		return true;
	}
	else if (inputManager.validate(keyCode, modifiers, InputAction::NextRight))
	{
		carrouselComponent.showNextComponent();
		return true;
	}
	else if (inputManager.validate(keyCode, modifiers, InputAction::NextUp))
	{
		carrouselComponent.grabKeyboardFocus();
		return true;
	}
	else if (inputManager.validate(keyCode, modifiers, InputAction::NextDown))
	{
		transportComponent.grabKeyboardFocus();
		return true;
	}
	DBG("HOME");
	return false;
}