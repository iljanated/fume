#pragma once

#include <JuceHeader.h>
#include "UIContext.h"

namespace te = tracktion;

class TrackListComponent;

class TrackListCursorComponent : public juce::Component, public juce::ValueTree::Listener
{
public:
	TrackListCursorComponent(fumeUI::UIContext& context, TrackListComponent& trackListComponent);
	~TrackListCursorComponent() override;

	/* Handle paint events */
	void paint(juce::Graphics& g) override;

	/* Handle key press events */
	bool keyPressed(const juce::KeyPress& key) override;

	/* Get the current cursor position */
	juce::Point<double> getCursorPosition() const { return cursorPosition; }

	/* Move the cursor to a position close to the target pixel position*/
	void alignCursorPosition(const juce::Point<int>& targetPosition);

	void valueTreePropertyChanged(juce::ValueTree& treeWhosePropertyHasChanged, const juce::Identifier& property) override;

	/* Update the bounds of the cursor component based on the current cursor position */
	void updateBounds();
private:
	fumeUI::UIContext& uiContext;

	/* The current cursor position in seconds and tracks */
	juce::Point<double> cursorPosition{ 0.0, 0.0 };
	TrackListComponent& trackListComponent;

	/* Move the cursor by a delta relative to current snapping and visual trackHeight*/
	void moveCursorPosition(const juce::Point<int>& delta);
};