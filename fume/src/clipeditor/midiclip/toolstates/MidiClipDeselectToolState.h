#pragma once

#include <JuceHeader.h>
#include "MidiClipToolState.h"

class MidiEventsEditorComponent;

namespace te = tracktion;

class MidiClipDeselectToolState : public MidiClipToolState
{
public:
	MidiClipDeselectToolState(MidiEventsEditorComponent* component)
		: MidiClipToolState(component, FumeIDs::MidiClipDeselectToolState, InputAction::Deselect)
	{
	}

	~MidiClipDeselectToolState() override = default;

	void onEnterState() override
	{
	}

	void onExitState() override
	{
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

		if (inputManager.validate(keyCode, modifiers, inputAction))
		{
			component->clearSelectedEvents();
			notifyToolFinished();
			return true;
		}
		return false;
	}

	bool keyStateChanged(bool isKeyDown) override
	{
		return false;
	}

private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MidiClipDeselectToolState)
};