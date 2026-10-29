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
		auto& extendedEdit = uiContext.extendedEdit;

		auto modifiers = key.getModifiers();
		auto keyCode = key.getKeyCode();

		if (!isLassoActive && inputManager.validate(keyCode, modifiers, inputAction))
		{
			isLassoActive = true;
			component->getCursorComponent().setVisible(false);
			component->addAndMakeVisible(lassoComponent);

			dragPosition = component->getCursorComponent().getCursorPosition();
			beginLasso(modifiers);
			//dragLasso(juce::Point<int>(1, 1), modifiers);
			return true;
		}
		else if (isLassoActive && inputManager.validate(keyCode, InputAction::Left))
		{
			dragLasso(juce::Point<int>(-1, 0), modifiers);
			return true;
		}
		else if (isLassoActive && inputManager.validate(keyCode, InputAction::Right))
		{
			dragLasso(juce::Point<int>(1, 0), modifiers);
			return true;
		}
		else if (isLassoActive && inputManager.validate(keyCode, InputAction::Up))
		{
			dragLasso(juce::Point<int>(0, -1), modifiers);
			return true;
		}
		else if (isLassoActive && inputManager.validate(keyCode, InputAction::Down))
		{
			dragLasso(juce::Point<int>(0, 1), modifiers);
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
	juce::Point<double> dragPosition;
	bool isLassoActive = false;
	SelectedItemSet<te::Clip*> selectedItemSet;
	ManualLassoComponent<te::Clip*> lassoComponent;

	void beginLasso(juce::ModifierKeys modifiers)
	{
		auto& uiContext = component->getUIContext();
		auto& extendedEdit = uiContext.extendedEdit;

		auto secondsPerPixel = extendedEdit.getSecondsPerPixel();
		auto trackHeight =	extendedEdit.getTrackHeight();

		auto pixelPosition = juce::Point<int>(static_cast<int>(dragPosition.getX() / secondsPerPixel), static_cast<int>(dragPosition.getY() * trackHeight));
		lassoComponent.setRowHeight(trackHeight);
		lassoComponent.setMargin(2);
		lassoComponent.beginLasso(pixelPosition, modifiers, this);
	}

	void dragLasso(juce::Point<int> delta, juce::ModifierKeys modifiers)
	{
		auto& uiContext = component->getUIContext();
		auto& extendedEdit = uiContext.extendedEdit;

		auto secondsPerPixel = extendedEdit.getSecondsPerPixel();
		auto trackHeight =	extendedEdit.getTrackHeight();

		auto deltaX = delta.getX();

		if (deltaX != 0)
		{
			auto timePosition = te::TimePosition::fromSeconds(dragPosition.getX());
			auto timeOffset = te::TimeDuration::fromSeconds(0.0001);

			auto& snapType = component->getSnapType();
			auto& tempoSequence = extendedEdit.getTempoSequence();

			if (deltaX > 0)
			{
				dragPosition.setX(snapType.roundTimeUp(timePosition + timeOffset, tempoSequence).inSeconds());
			}
			else
			{
				dragPosition.setX(snapType.roundTimeDown(timePosition - timeOffset, tempoSequence).inSeconds());
			}

			dragPosition.setX(std::clamp(dragPosition.getX(), 0.0, extendedEdit.getEdit().getMaximumLength().inSeconds()));

		}
		
		dragPosition.setY(std::clamp(dragPosition.getY() + delta.getY(), 0.0, static_cast<double>(component->getTrackCount())));

		auto pixelPosition = juce::Point<int>(static_cast<int>(dragPosition.getX() / secondsPerPixel), static_cast<int>(dragPosition.getY() * trackHeight));
		lassoComponent.dragLasso(pixelPosition, modifiers);
	}

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TrackListSelectToolState)
};

