#pragma once

#include <JuceHeader.h>
#include "ToolState.h"
#include "InputManager.h"

class MidiEventsEditorComponent;

namespace te = tracktion;

class MidiClipWriteToolState : public ToolState<MidiEventsEditorComponent>
{
public:
	MidiClipWriteToolState(MidiEventsEditorComponent* component);

	~MidiClipWriteToolState() override = default;

	void onEnterState() override;

	void onExitState() override;

	bool keyPressed(const juce::KeyPress& key) override;

	bool keyStateChanged(bool isKeyDown) override;

private:
	bool isWriting = false;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MidiClipWriteToolState)
};