#pragma once

#include <JuceHeader.h>
#include "ToolState.h"
#include "InputManager.h"

class MidiEventsEditorComponent;

namespace te = tracktion;

class MidiClipDeleteToolState : public ToolState<MidiEventsEditorComponent>
{
public:
	MidiClipDeleteToolState(MidiEventsEditorComponent* component);

	~MidiClipDeleteToolState() override = default;

	void onEnterState() override;

	void onExitState() override;

	bool keyPressed(const juce::KeyPress& key) override;

	bool keyStateChanged(bool isKeyDown) override;

private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MidiClipDeleteToolState)
};