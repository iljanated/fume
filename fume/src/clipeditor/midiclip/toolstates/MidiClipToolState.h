#pragma once

#include "ToolState.h"

class MidiEventsEditorComponent;

class MidiClipToolState : public ToolState
{
public:
	MidiClipToolState(MidiEventsEditorComponent* component, const juce::Identifier identifier, InputAction inputAction)
		: ToolState(identifier, inputAction), component(component) {
	}

	~MidiClipToolState() override = default;


protected:
	MidiEventsEditorComponent* component;
};