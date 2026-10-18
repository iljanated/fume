#pragma once

#include <JuceHeader.h>
#include "ToolState.h"
#include "InputManager.h"

class TrackListComponent;

namespace te = tracktion;

class TrackListSelectToolState : public ToolState<TrackListComponent>
{
public:
	TrackListSelectToolState(TrackListComponent* component);

	~TrackListSelectToolState() override = default;

	void onEnterState() override;

	void onExitState() override;

	bool keyPressed(const juce::KeyPress& key) override;

	bool keyStateChanged(bool isKeyDown) override;

private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TrackListSelectToolState)
};