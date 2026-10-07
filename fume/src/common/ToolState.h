#pragma once

#include "InputManager.h"

class ToolStateListener
{
public:
	virtual void onToolStateFinished() = 0;
};

template <typename T>
class ToolState
{
public:
	ToolState(T& c, InputManager& i, ToolStateListener& l) : component(c), inputManager(i), listener(l) {}
	virtual ~ToolState() = default;
	virtual void onEnterState() = 0;
	virtual void onExitState() = 0;
	virtual void onInputAction(InputManagerAction action, int modifiersMask, bool isActive) = 0;
protected:
	T& component;
	InputManager& inputManager;
	ToolStateListener& listener;
};