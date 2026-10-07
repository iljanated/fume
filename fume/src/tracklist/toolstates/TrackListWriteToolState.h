#pragma once

#include <JuceHeader.h>
#include "ToolState.h"
#include "InputManager.h"

class TrackListComponent;

namespace te = tracktion;

class TrackListWriteToolState : public ToolState<TrackListComponent>
{
public:
	TrackListWriteToolState(TrackListComponent& c, InputManager& i, ToolStateListener& l);
	~TrackListWriteToolState() override;

	void onEnterState() override;

	void onExitState() override;

	void onInputAction(InputManagerAction action, int inputMask, bool isActive) override;
private:
	juce::DrawableRectangle& drawableCursor;
	juce::Point<double>& cursor;
	juce::Point<double>& cursorAnchor;
	te::SelectionManager& selectionManager;
	bool isDragging = false;

	void updateDrawableCursor();
};