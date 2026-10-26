#pragma once

#include <JuceHeader.h>
#include "MidiClipToolState.h"

class MidiEventsEditorComponent;

namespace te = tracktion;

class MidiClipSelectToolState : public MidiClipToolState
{
public:
	MidiClipSelectToolState(MidiEventsEditorComponent* component);

	~MidiClipSelectToolState() override = default;

	void onEnterState() override;

	void onExitState() override;

	bool keyPressed(const juce::KeyPress& key) override;

	bool keyStateChanged(bool isKeyDown) override;

private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MidiClipSelectToolState)
};