#pragma once

#include <JuceHeader.h>
#include "InputManager.h"


template <typename T>
class ToolState
{
public:
	ToolState(T* component, const juce::Identifier identifier, InputAction inputAction) :
		component(component), identifier(identifier), inputAction(inputAction) {
	}
	virtual ~ToolState() = default;

	virtual void onEnterState() = 0;
	virtual void onExitState() = 0;

	virtual bool keyPressed(const juce::KeyPress& key) = 0;
	virtual bool keyStateChanged(bool isKeyDown) = 0;

	juce::Identifier getIdentifier() const { return identifier; }

	InputAction getInputAction() const { return inputAction; }

protected:
	T* component = nullptr;
	juce::Identifier identifier;
	InputAction inputAction;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ToolState)
};

/// <summary>
/// ToolStateManager manages toolstates.
/// </summary>
/// <typeparam name="T"></typeparam>
template <typename T>
class ToolStateManager
{
public:
	ToolStateManager(InputManager& inputManager) : inputManager(inputManager)
	{
	}

	~ToolStateManager() = default;

	/** Handles key presses and delegates them to the current tool state if applicable */
	bool keyPressed(const juce::KeyPress& key)
	{
		auto modifiers = key.getModifiers();
		auto keyCode = key.getKeyCode();
		
		for (auto& toolState : toolStates)
		{
			if (toolState.get() != currentToolState && inputManager.validate(keyCode, modifiers, toolState.get()->getInputAction()))
			{
				activateToolState(toolState.get());
				break;
			}
		}
		
		if (currentToolState && currentToolState->keyPressed(key))
		{
			DBG("ToolState handled it");
			return true;
		}

		return false;
	}
	bool keyStateChanged(bool isKeyDown)
	{
		if (currentToolState)
		{
			return currentToolState->keyStateChanged(isKeyDown);
		}
		return false;
	}

	/** Adds a new tool state to the manager */
	void addToolState(std::unique_ptr<class ToolState<T>> toolState)
	{
		toolStates.push_back(std::move(toolState));
	}

	void activateToolStateByIdentifier(const juce::Identifier& identifier)
	{
		for (auto& toolState : toolStates)
		{
			if (toolState->getIdentifier() == identifier)
			{
				activateToolState(toolState.get());
				return;
			}
		}
	}
private:
	InputManager& inputManager;
	std::vector<std::unique_ptr<class ToolState<T>>> toolStates;
	ToolState<T>* currentToolState{ nullptr };

	/** Activates the given tool state, deactivating the current one if necessary */
	void activateToolState(ToolState<T>* newState)
	{
		if (currentToolState == newState)
		{
			return;
		}
		if (currentToolState)
		{
			currentToolState->onExitState();
		}
		currentToolState = newState;
		if (currentToolState)
		{
			DBG("Activating tool state: " << currentToolState->getIdentifier().toString());
			currentToolState->onEnterState();
		}
	}

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ToolStateManager)
};
