#pragma once

#include <JuceHeader.h>
#include "MidiClipToolState.h"

class MidiEventsEditorComponent;

namespace te = tracktion;

class MidiClipWriteToolState : public MidiClipToolState
{
public:
	MidiClipWriteToolState(MidiEventsEditorComponent* component);

	~MidiClipWriteToolState() override = default;

	void onEnterState() override;

	void onExitState() override;

	bool canHandle(const juce::KeyPress& key) override;

	bool keyPressed(const juce::KeyPress& key) override;

	bool keyStateChanged(bool isKeyDown) override;

private:
	bool isWriting = false;
	double lengthSnappingInterval = 0.0625;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MidiClipWriteToolState)
};