#pragma once

#include <JuceHeader.h>
#include "UIMidiClipContext.h"

namespace te = tracktion;

class MidiEventsEditorComponent;

class MidiEventsEditorCursorComponent : public juce::Component, public juce::ValueTree::Listener
{
public:
	MidiEventsEditorCursorComponent(fumeUI::UIMidiClipContext& context, MidiEventsEditorComponent& midiEventsEditorComponent);
	~MidiEventsEditorCursorComponent() override;

	/* Handle paint events */
	void paint(juce::Graphics& g) override;

	/* Handle key press events */
	bool keyPressed(const juce::KeyPress& key) override;

	/* Get the current cursor position */
	juce::Point<double> getCursorPosition() const { return cursorPosition; }

	/* Set the cursor position */
	void setCursorPosition(const juce::Point<double>& newPosition);

	void valueTreePropertyChanged(juce::ValueTree& treeWhosePropertyHasChanged, const juce::Identifier& property) override;

	/* Update the bounds of the cursor component based on the current cursor position */
	void updateBounds();
private:
	fumeUI::UIMidiClipContext& uiContext;

	/* The current cursor position in seconds and tracks */
	juce::Point<double> cursorPosition{ 0.0, 0.0 };
	MidiEventsEditorComponent& midiEventsEditorComponent;

	/* Move the cursor by a delta relative to current snapping and visual trackHeight*/
	void moveCursorPosition(const juce::Point<int>& delta);
};