#pragma once

#include <JuceHeader.h>
#include "ToolState.h"
#include "InputManager.h"

class TrackListComponent;

namespace te = tracktion;

class TrackListZoomToolState : public ToolState<TrackListComponent>
{
public:
	TrackListZoomToolState(TrackListComponent* component);

	~TrackListZoomToolState() override = default;

	void onEnterState() override;

	void onExitState() override;

	bool keyPressed(const juce::KeyPress& key) override;

	bool keyStateChanged(bool isKeyDown) override;

private:
	bool isZooming = false;
	void zoom(float x, float y);

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TrackListZoomToolState)
};