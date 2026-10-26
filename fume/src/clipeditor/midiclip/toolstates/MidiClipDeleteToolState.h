#pragma once

#include <JuceHeader.h>
#include "MidiClipToolState.h"

namespace te = tracktion;

class MidiClipDeleteToolState : public MidiClipToolState
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