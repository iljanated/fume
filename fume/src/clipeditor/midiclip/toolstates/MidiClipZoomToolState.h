#pragma once

#include <JuceHeader.h>
#include "MidiClipToolState.h"

namespace te = tracktion;

class MidiClipZoomToolState : public MidiClipToolState
{
public:
	MidiClipZoomToolState(MidiEventsEditorComponent* component);

	~MidiClipZoomToolState() override = default;

	void onEnterState() override;

	void onExitState() override;

	bool canHandle(const juce::KeyPress& key) override;

	bool keyPressed(const juce::KeyPress& key) override;

	bool keyStateChanged(bool isKeyDown) override;

private:
	bool isZooming = false;
	void zoom(float x, float y);

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MidiClipZoomToolState)
};