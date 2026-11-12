#include "MidiClipSelectToolState.h"
#include "Utilities.h"
#include "MidiEventsEditorComponent.h"

MidiClipSelectToolState::MidiClipSelectToolState(MidiEventsEditorComponent* component)
	: MidiClipToolState(component, FumeIDs::MidiClipSelectToolState)
{
	selectedItemSet.addChangeListener(this);
}

MidiClipSelectToolState::~MidiClipSelectToolState()
{
	selectedItemSet.removeChangeListener(this);
}

void MidiClipSelectToolState::changeListenerCallback(ChangeBroadcaster* broadcaster)
{
	if (broadcaster == &selectedItemSet)
	{
		if (component != nullptr)
		{
			component->lassoSelectionChanged(selectedItemSet);
		}
	}
}

void MidiClipSelectToolState::findLassoItemsInArea(Array<te::MidiNote*>& itemsFound, const Rectangle<int>& area)
{
	if (component != nullptr)
	{
		component->findLassoItemsInArea(itemsFound, area);
	}
}

bool MidiClipSelectToolState::canHandle(const juce::KeyPress& key)
{
	if (!component)
	{
		return false;
	}
	auto& inputManager = component->getUIContext().inputManager;
	return inputManager.validate(key.getKeyCode(), key.getModifiers(), InputAction::Select);
}

bool MidiClipSelectToolState::keyPressed(const juce::KeyPress& key)
{
	if (!component)
	{
		return false;
	}
	auto& uiContext = component->getUIContext();
	auto& inputManager = uiContext.inputManager;
	auto& extendedMidiClip = uiContext.extendedMidiClip;

	auto modifiers = key.getModifiers();
	auto keyCode = key.getKeyCode();

	if (!isLassoActive && inputManager.validate(keyCode, modifiers, InputAction::Select))
	{
		isLassoActive = true;
		component->showCursor(false);
		component->addAndMakeVisible(lassoComponent);

		dragPosition = juce::Point<double>(extendedMidiClip.getCursorX(), extendedMidiClip.getCursorY());
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

bool MidiClipSelectToolState::keyStateChanged(bool isKeyDown)
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

void MidiClipSelectToolState::beginLasso(juce::ModifierKeys modifiers)
{
	auto& uiContext = component->getUIContext();
	auto& extendedMidiClip = uiContext.extendedMidiClip;

	auto beatsPerPixel = extendedMidiClip.getBeatsPerPixel();
	auto noteHeight = extendedMidiClip.getNoteHeight();

	auto pixelPosition = juce::Point<int>(static_cast<int>(dragPosition.getX() / beatsPerPixel), static_cast<int>(dragPosition.getY() * noteHeight));
	lassoComponent.setRowHeight(noteHeight);
	lassoComponent.setMargin(2);
	lassoComponent.beginLasso(pixelPosition, modifiers, this);
}

void MidiClipSelectToolState::dragLasso(juce::Point<int> delta, juce::ModifierKeys modifiers)
{
	auto& uiContext = component->getUIContext();
	auto& extendedMidiClip = uiContext.extendedMidiClip;

	auto beatsPerPixel = extendedMidiClip.getBeatsPerPixel();
	auto noteHeight = extendedMidiClip.getNoteHeight();

	auto deltaX = delta.getX();

	if (deltaX != 0)
	{
		auto beatPosition = te::BeatPosition::fromBeats(dragPosition.getX());
		auto quantisationType = extendedMidiClip.getCursorQuantisation();

		auto newX = dragPosition.getX();

		if (deltaX > 0)
		{
			auto beatOffset = te::BeatDuration::fromBeats(0.0001);
			newX = quantisationType.roundBeatUp(beatPosition + beatOffset).inBeats();
		}
		else
		{
			newX = quantisationType.roundBeatDown(beatPosition).inBeats();
		}

		dragPosition.setX(newX);

	}

	dragPosition.setY(std::clamp(dragPosition.getY() + delta.getY(), 0.0, 127.0));

	auto pixelPosition = juce::Point<int>(static_cast<int>(dragPosition.getX() / beatsPerPixel), static_cast<int>(dragPosition.getY() * noteHeight));
	lassoComponent.dragLasso(pixelPosition, modifiers);
}
