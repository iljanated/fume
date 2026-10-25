#pragma once

#include <JuceHeader.h>
#include <SDL3/SDL.h>

enum class InputAction : uint32_t
{
	None,
	Left,
	Right,
	Up,
	Down,
	MoveLeft,
	MoveRight,
	MoveUp,
	MoveDown,
	NextLeft,
	NextRight,
	NextUp,
	NextDown,
	Play,
	PlayFromStart,
	Write,
	Delete,
	Select,
	MultiSelect,
	Deselect,
	Undo,
	Redo,
	Zoom
};

struct InputActionHashGenerator
{
	int generateHash(InputAction key, int upperLimit) const
	{
		auto enumIntValue = static_cast<uint32_t> (key);
		return juce::DefaultHashFunctions::generateHash(enumIntValue, upperLimit);
	}
};

struct ActionBinding
{
	int keyCode;
 	juce::ModifierKeys::Flags modifiers;
};

class InputManager : private juce::Timer
{
public:
	//==============================================================================
	InputManager();

	~InputManager();

	bool isCurrentlyPressed(InputAction action);
	bool validate(int keyCode, InputAction action) const;
	bool validate(int keyCode, juce::ModifierKeys modifiers, InputAction action) const;
	ActionBinding getBinding(InputAction action) const;

private:
	juce::HashMap<InputAction, ActionBinding, InputActionHashGenerator> actionBindings;
	juce::HashMap<int, int> pressedButtons;

	void initSDL();
	void quitSDL();
	void pollSDL();
	void timerCallback() override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(InputManager)
};
