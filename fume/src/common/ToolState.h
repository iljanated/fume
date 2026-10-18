#pragma once

#include <JuceHeader.h>

/// <summary>
/// ToolStateManager manages toolstates.
/// </summary>
/// <typeparam name="T"></typeparam>
template <typename T>
class ToolStateManager
{
public:
	ToolStateManager(fumeUI::UIContext& context) : context(context)
	{
	}

	/** Handles key presses and delegates them to the current tool state if applicable */
	bool keyPressed(const juce::KeyPress& key)
	{
		auto modifiers = key.getModifiers();
		auto keyCode = key.getKeyCode();
		auto& inputManager = context.inputManager;

		for (ToolState& toolState : toolStates)
		{
			if (&toolState != currentToolState && inputManager.validate(keyCode, modifiers, toolState.inputAction))
			{
				activateToolState(&toolState);
				break;
			}
		}
		
		if (currentToolState && currentToolState->keyPressed(key))
		{
			DBG("ToolState handled it");
			return true;
		}

		return false:
	}
	void keyStateChanged(bool isKeyDown)
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
private:
	fumeUI::UIContext& context;
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
			DBG("Activating tool state: " << typeid(*currentToolState).name());
			currentToolState->onEnterState();
		}
	}

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ToolStateManager)
};

template <typename T>
class ToolState
{
public:
	ToolState(T* component, const juce::String& name, fume::InputAction inputAction) : 
		component(component), name(name), inputAction(inputAction) {}
	virtual ~ToolState() = default;

	virtual void onEnterState() = 0;
	virtual void onExitState() = 0;

	virtual bool keyPressed(const juce::KeyPress& key) = 0;
	virtual bool keyStateChanged(bool isKeyDown) = 0;

	juce::String getName() const { return name; }

protected:
	T* component = nullptr;
	juce::String name;
	fume::InputAction inputAction;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ToolState)
};