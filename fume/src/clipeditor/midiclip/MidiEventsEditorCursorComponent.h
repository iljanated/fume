#pragma once

#include <JuceHeader.h>
#include "UIMidiClipContext.h"

namespace te = tracktion;

class MidiEventsEditorComponent;

class MidiEventsEditorCursorComponent : public juce::Component, public juce::ValueTree::Listener
{
public:
	MidiEventsEditorCursorComponent(fumeUI::UIMidiClipContext& context);
	~MidiEventsEditorCursorComponent() override;

	/* Handle paint events */
	void paint(juce::Graphics& g) override;

	/* Handle key press events */
	bool keyPressed(const juce::KeyPress& key) override;

	void valueTreePropertyChanged(juce::ValueTree& treeWhosePropertyHasChanged, const juce::Identifier& property) override;

	/* Update the bounds of the cursor component based on the current cursor position */
	void updateBounds();
private:
	juce::ValueTree midiClipState;
	fumeUI::UIMidiClipContext& uiContext;

	/* Move the cursor by a delta relative to current snapping and visual trackHeight*/
	void moveCursorPosition(const juce::Point<int>& delta);
};