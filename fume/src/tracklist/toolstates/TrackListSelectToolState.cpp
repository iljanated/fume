#include "TrackListSelectToolState.h"
#include "Utilities.h"
#include "TrackListComponent.h"

TrackListSelectToolState::TrackListSelectToolState(TrackListComponent* component)
	: TrackListToolState(component, FumeIDs::TrackListSelectToolState)
{
	selectedItemSet.addChangeListener(this);
}

TrackListSelectToolState::~TrackListSelectToolState()
{
	selectedItemSet.removeChangeListener(this);
}

void TrackListSelectToolState::changeListenerCallback(ChangeBroadcaster* broadcaster)
{
	if (broadcaster == &selectedItemSet)
	{
		if (component != nullptr)
		{
			component->lassoSelectionChanged(selectedItemSet);
		}
	}
}

void TrackListSelectToolState::findLassoItemsInArea(Array<te::Clip*>& itemsFound, const Rectangle<int>& area)
{
	if (component != nullptr)
	{
		component->findLassoItemsInArea(itemsFound, area);
	}
}

bool TrackListSelectToolState::canHandle(const juce::KeyPress& key)
{
	if (!component)
	{
		return false;
	}
	auto& inputManager = component->getUIContext().inputManager;
	return inputManager.validate(key.getKeyCode(), key.getModifiers(), InputAction::Select);
}

bool TrackListSelectToolState::keyPressed(const juce::KeyPress& key)
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

	if (!isLassoActive && inputManager.validate(keyCode, modifiers, InputAction::Select))
	{
		isLassoActive = true;
		component->showCursor(false);
		component->addAndMakeVisible(lassoComponent);

		dragPosition = juce::Point<double>(extendedEdit.getCursorX(), extendedEdit.getCursorY());
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

bool TrackListSelectToolState::keyStateChanged(bool isKeyDown)
{
	if (!component)
	{
		return false;
	}
	auto& inputManager = component->getUIContext().inputManager;

	if (!isKeyDown && isLassoActive && !inputManager.isCurrentlyPressed(InputAction::Select))
	{
		isLassoActive = false;
		lassoComponent.endLasso();
		component->removeChildComponent(&lassoComponent);
		component->showCursor(true);
		notifyToolFinished();
		return true;
	}
	return false;
}

void TrackListSelectToolState::beginLasso(juce::ModifierKeys modifiers)
{
	auto& uiContext = component->getUIContext();
	auto& extendedEdit = uiContext.extendedEdit;

	auto secondsPerPixel = extendedEdit.getSecondsPerPixel();
	auto trackHeight = extendedEdit.getTrackHeight();

	auto pixelPosition = juce::Point<int>(static_cast<int>(dragPosition.getX() / secondsPerPixel), static_cast<int>(dragPosition.getY() * trackHeight));
	DBG("pixelPosition: " << pixelPosition.getX() << ", " << pixelPosition.getY());
	lassoComponent.setRowHeight(trackHeight);
	lassoComponent.setMargin(2);
	lassoComponent.beginLasso(pixelPosition, modifiers, this);
}

void TrackListSelectToolState::dragLasso(juce::Point<int> delta, juce::ModifierKeys modifiers)
{
	auto& uiContext = component->getUIContext();
	auto& extendedEdit = uiContext.extendedEdit;

	auto secondsPerPixel = extendedEdit.getSecondsPerPixel();
	auto trackHeight = extendedEdit.getTrackHeight();

	auto deltaX = delta.getX();

	if (deltaX != 0)
	{
		auto timePosition = te::TimePosition::fromSeconds(dragPosition.getX());
		auto timeOffset = te::TimeDuration::fromSeconds(0.0001);

		auto& snapType = component->getSnapType();
		auto& tempoSequence = extendedEdit.getTempoSequence();

		auto newX = dragPosition.getX();

		if (deltaX > 0)
		{
			newX = snapType.roundTimeUp(timePosition + timeOffset, tempoSequence).inSeconds();
		}
		else
		{
			newX = snapType.roundTimeDown(timePosition - timeOffset, tempoSequence).inSeconds();
		}

		dragPosition.setX(std::clamp(newX, 0.0, extendedEdit.getEdit().getMaximumLength().inSeconds()));

	}

	dragPosition.setY(std::clamp(dragPosition.getY() + delta.getY(), 0.0, static_cast<double>(component->getTrackCount())));

	auto pixelPosition = juce::Point<int>(static_cast<int>(dragPosition.getX() / secondsPerPixel), static_cast<int>(dragPosition.getY() * trackHeight));
	lassoComponent.dragLasso(pixelPosition, modifiers);
}
