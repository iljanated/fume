#pragma once

#include <JuceHeader.h>
#include "ToolState.h"
#include "InputManager.h"

class MidiEventsEditorComponent;

namespace te = tracktion;

class MidiClipMoveToolState : public ToolState<MidiEventsEditorComponent>
{
public:
	MidiClipMoveToolState(MidiEventsEditorComponent* component);

	~MidiClipMoveToolState() override = default;

	void onEnterState() override;

	void onExitState() override;

	bool keyPressed(const juce::KeyPress& key) override;

	bool keyStateChanged(bool isKeyDown) override;

private:

	void moveNote(te::MidiNote* note, double deltaBeats, int deltaPitch, juce::UndoManager& undoManager);

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MidiClipMoveToolState)
};