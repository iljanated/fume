#pragma once

#include <JuceHeader.h>
#include "ToolState.h"
#include "InputManager.h"

class TrackListComponent;

namespace te = tracktion;

class TrackListSelectToolState : public ToolState<TrackListComponent>
{
public:
	TrackListSelectToolState(TrackListComponent& c, fumeUI::UIContext& context, ToolStateListener& l);
	~TrackListSelectToolState() override;

	void onEnterState() override;

	void onExitState() override;

	void onInputAction(InputManagerActionId actionId, int inputMask, bool isActive) override;
private:
	juce::DrawableRectangle& drawableCursor;
	juce::Point<double>& cursor;
	juce::Point<double>& cursorAnchor;
	bool isDragging = false;

	void updateSelection();
	void updateDrawableCursor();
};