#pragma once

#include <JuceHeader.h>

enum class InputManagerAction : int
{
    DPAD_UP = 1 << 0,
	DPAD_DOWN = 1 << 1,
	DPAD_LEFT = 1 << 2,
	DPAD_RIGHT = 1 << 3,
    A = 1 << 4,
    B = 1 << 5,
	X = 1 << 6,
	Y = 1 << 7,
    LB = 1 << 8,
	RB = 1 << 9,
	LT = 1 << 10,
	RT = 1 << 11,
	LSB = 1 << 12,
	RSB = 1 << 13,
	Start = 1 << 14,
	Select = 1 << 15
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

class InputManagerListener {
public:
	virtual void onInputAction(InputManagerAction action, int inputMask, bool isActive) = 0;
};

class InputManager : public juce::LightweightListenerList<InputManagerListener>,
    public juce::KeyListener
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
	juce::HashMap<int, juce::Array<InputManagerActionBinding>> actionBindings;
	juce::HashMap<int, InputManagerAction> keyCodeToActionMap;
    
    int inputMask;

	void updateKeyCodeToActionMap();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(InputManager)
};
