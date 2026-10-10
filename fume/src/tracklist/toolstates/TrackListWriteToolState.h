#pragma once

#include <JuceHeader.h>
#include "ToolState.h"
#include "InputManager.h"

class TrackListComponent;

namespace te = tracktion;

class TrackListWriteToolState : public ToolState<TrackListComponent>
{
public:
	TrackListWriteToolState(TrackListComponent& c, fumeUI::UIContext& context, ToolStateListener& l);
	~TrackListWriteToolState() override;

	void onEnterState() override;

	void onExitState() override;

	void onInputAction(InputManagerActionId actionId, int inputMask, bool isActive) override;
private:
	juce::DrawableRectangle& drawableCursor;
	juce::Point<double>& cursor;
	juce::Point<double>& cursorAnchor;
	bool isDragging = false;

	void updateDrawableCursor();
};