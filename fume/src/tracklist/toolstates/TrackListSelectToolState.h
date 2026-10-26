#pragma once

#include <JuceHeader.h>
#include "TrackListToolState.h"
#include "InputManager.h"
#include "Utilities.h"
#include "ManualLassoComponent.h"

namespace te = tracktion;


class TrackListSelectToolState : public TrackListToolState, public LassoSource<te::Clip*>, public	juce::ChangeListener
{
public:
	TrackListSelectToolState(TrackListComponent* component)
		: TrackListToolState(component, FumeIDs::TrackListSelectToolState, InputAction::Select)
	{
		selectedItemSet.addChangeListener(this);
	}

	~TrackListSelectToolState() override
	{
		selectedItemSet.removeChangeListener(this);
	}

	void onEnterState() override {}

	void onExitState() override {}

	void changeListenerCallback(ChangeBroadcaster* broadcaster) override
	{
		if (broadcaster == &selectedItemSet)
		{
			if (component != nullptr)
			{
				component->lassoSelectionChanged(selectedItemSet);
			}
		}
	}

	virtual void findLassoItemsInArea(Array<te::Clip*>& itemsFound, const Rectangle<int>& area) override
	{
		if (component != nullptr)
		{
			component->findLassoItemsInArea(itemsFound, area);
		}
	}

	bool keyPressed(const juce::KeyPress& key) override
	{
		if (!component)
		{
			return false;
		}
		auto& uiContext = component->getUIContext();
		auto& inputManager = uiContext.inputManager;

		auto modifiers = key.getModifiers();
		auto keyCode = key.getKeyCode();

		if (!isLassoActive && inputManager.validate(keyCode, modifiers, inputAction))
		{
			isLassoActive = true;
			component->getCursorComponent().setVisible(false);
			component->addAndMakeVisible(lassoComponent);

			dragPosition = juce::Point<int>(20, 20);
			lassoComponent.beginLasso(dragPosition, modifiers, this);
			dragPosition += juce::Point<int>(20, 20);
			lassoComponent.dragLasso(dragPosition, modifiers);
			return true;
		}
		else if (isLassoActive && inputManager.validate(keyCode, InputAction::Left))
		{
			dragPosition.x -= 20;
			lassoComponent.dragLasso(dragPosition, modifiers);
			return true;
		}
		else if (isLassoActive && inputManager.validate(keyCode, InputAction::Right))
		{
			dragPosition.x += 20;
			lassoComponent.dragLasso(dragPosition, modifiers);
			return true;
		}
		else if (isLassoActive && inputManager.validate(keyCode, InputAction::Up))
		{
			auto unitSize = component->getUnitSize();
			dragPosition.y -= unitSize.getY();
			lassoComponent.dragLasso(dragPosition, modifiers);
			return true;
		}
		else if (isLassoActive && inputManager.validate(keyCode, InputAction::Down))
		{
			auto unitSize = component->getUnitSize();
			dragPosition.y += unitSize.getY();
			lassoComponent.dragLasso(dragPosition, modifiers);
			return true;
		}
		return false;
	}

	bool keyStateChanged(bool isKeyDown) override
	{
		if (!component)
		{
			return false;
		}
		auto& inputManager = component->getUIContext().inputManager;

		if (!isKeyDown && isLassoActive && !inputManager.isCurrentlyPressed(inputAction))
		{
			isLassoActive = false;
			lassoComponent.endLasso();
			component->removeChildComponent(&lassoComponent);
			component->getCursorComponent().setVisible(true);

			return true;
		}
		return false;
	}

	SelectedItemSet<te::Clip*>& getLassoSelection() override { return selectedItemSet; }


private:
	juce::Point<int> dragPosition;
	bool isLassoActive = false;
	SelectedItemSet<te::Clip*> selectedItemSet;
	ManualLassoComponent<te::Clip*> lassoComponent;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TrackListSelectToolState)
};

