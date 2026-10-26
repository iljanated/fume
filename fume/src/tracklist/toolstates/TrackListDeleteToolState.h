#pragma once

#include <JuceHeader.h>
#include "TrackListToolState.h"

class TrackListComponent;

namespace te = tracktion;

class TrackListDeleteToolState : public TrackListToolState
{
public:
	TrackListDeleteToolState(TrackListComponent* component);

	~TrackListDeleteToolState() override = default;

	void onEnterState() override;

	void onExitState() override;

	bool keyPressed(const juce::KeyPress& key) override;

	bool keyStateChanged(bool isKeyDown) override;

private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TrackListDeleteToolState)
};