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

	void onInputAction(InputManagerAction action, int inputMask, bool isActive) override;
private:
	te::SelectionManager& selectionManager;
};