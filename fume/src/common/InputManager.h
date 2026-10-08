#pragma once

#include <JuceHeader.h>
#include <SDL3/SDL.h>

enum class InputManagerActionId : int
{
    UP = 1 << 0,
	DOWN = 1 << 1,
	LEFT = 1 << 2,
	RIGHT = 1 << 3,
    SELECT = 1 << 4,
    WRITE = 1 << 5,
	EDIT = 1 << 6,
	DELETE = 1 << 7,
    MOD1 = 1 << 8,
	UNDO = 1 << 9,
	LT = 1 << 10,
	RT = 1 << 11,
	LSB = 1 << 12,
	RSB = 1 << 13,
	PLAY = 1 << 14,
	OPTION = 1 << 15
};

enum InputManagerInputType : int
{
	KeyCode,
	Modifier,
    Button
};

struct InputManagerActionBinding
{
	InputManagerInputType type;
	int value;
};

struct InputManagerAction
{
	InputManagerActionId id;
	juce::Array<InputManagerActionBinding> bindings;
	bool isRepeating;
};

class InputManagerListener {
public:
	virtual void onInputAction(InputManagerActionId actionId, int inputMask, bool isActive) = 0;
};

class InputManager : public juce::LightweightListenerList<InputManagerListener>,
    public juce::KeyListener,
	private juce::Timer
{
public:
    //==============================================================================
    InputManager();

    ~InputManager();

    //==============================================================================

    bool keyPressed(const KeyPress& key, Component* originatingComponent) override;
    bool keyStateChanged(bool isKeyDown, Component* originatingComponent) override;
	bool modifierKeysChanged(int modifiers);

private:
    //==============================================================================
	juce::HashMap<int, InputManagerAction> actions;
	juce::HashMap<int, InputManagerActionId> keyCodeToActionMap;
	juce::HashMap<int, InputManagerActionId> buttonToActionMap;
	juce::HashMap<int, int> pressedButtons;
	
    int inputMask;
	
	bool buttonPressed(SDL_GamepadButton button);
	bool buttonReleased(SDL_GamepadButton button);
	void updateKeyCodeToActionMap();
	void updateButtonToActionMap();
	bool evaluateInputsStillActive();
	void initSDL();
	void quitSDL();
	void pollSDL();
	void timerCallback() override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(InputManager)
};
