#pragma once

#include <JuceHeader.h>
#include "TrackListToolState.h"

class TrackListComponent;

namespace te = tracktion;

class TrackListWriteToolState : public TrackListToolState
{
public:
	TrackListWriteToolState(TrackListComponent* component);

	~TrackListWriteToolState() override = default;

	void onEnterState() override;

	void onExitState() override;

	bool keyPressed(const juce::KeyPress& key) override;

	bool keyStateChanged(bool isKeyDown) override;
private:
	bool isWriting = false;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TrackListWriteToolState)
};