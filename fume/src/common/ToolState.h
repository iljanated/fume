#pragma once

#include "Utilities.h"

class ToolStateListener
{
public:
	virtual void onToolStateFinished() = 0;
};

template <typename T>
class ToolState
{
public:
	ToolState(T& c, fumeUI::UIContext& context, ToolStateListener& l) : component(c), uiContext(context), listener(l) {}
	virtual ~ToolState() = default;
	virtual void onEnterState() = 0;
	virtual void onExitState() = 0;
	virtual void onInputAction(InputManagerActionId actionId, int inputMask, bool isActive) = 0;
protected:
	T& component;
	fumeUI::UIContext& uiContext;
	ToolStateListener& listener;
};