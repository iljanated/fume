#pragma once

#include <JuceHeader.h>
#include "ToolState.h"
#include "InputManager.h"

class TrackListComponent;

namespace te = tracktion;

class TrackListDeleteToolState : public ToolState<TrackListComponent>
{
public:
	TrackListDeleteToolState(TrackListComponent& c, InputManager& i, ToolStateListener& l);
	~TrackListDeleteToolState() override;

	void onEnterState() override;

	void onExitState() override;

	void onInputAction(InputManagerActionId actionId, int inputMask, bool isActive) override;
private:
	juce::DrawableRectangle& drawableCursor;
	juce::Point<double>& cursor;
	juce::Point<double>& cursorAnchor;
	te::SelectionManager& selectionManager;
	bool isDragging = false;

	void updateSelection();
	void updateDrawableCursor();
};