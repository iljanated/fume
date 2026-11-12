#pragma once

#include <JuceHeader.h>
#include "MidiClipToolState.h"

class MidiEventsEditorComponent;

namespace te = tracktion;

class MidiClipMoveToolState : public MidiClipToolState
{
public:
	MidiClipMoveToolState(MidiEventsEditorComponent* component);

	~MidiClipMoveToolState() override = default;

	void onEnterState() override;

	void onExitState() override;

	bool canHandle(const juce::KeyPress& key) override;

	bool keyPressed(const juce::KeyPress& key) override;

	bool keyStateChanged(bool isKeyDown) override;

private:

	void moveNotes(juce::Point<int> delta);

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MidiClipMoveToolState)
};