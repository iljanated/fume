#pragma once

#include "ToolState.h"

class TrackListComponent;

class TrackListToolState : public ToolState
{
public:
	TrackListToolState(TrackListComponent* component, const juce::Identifier identifier, InputAction inputAction)
		: ToolState(identifier, inputAction), component(component) {
	}

	~TrackListToolState() override = default;


protected:
	TrackListComponent* component;
};