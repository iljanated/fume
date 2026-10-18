#pragma once
#include "InputManager.h"
#include "Utilities.h"

InputManager::InputManager()
{
	// cursor

	actionBindings.set(InputAction::Left, ActionBinding{ juce::KeyPress::leftKey, juce::ModifierKeys::noModifiers });
	actionBindings.set(InputAction::Right, ActionBinding{ juce::KeyPress::rightKey, juce::ModifierKeys::noModifiers });
	actionBindings.set(InputAction::Up, ActionBinding{ juce::KeyPress::upKey, juce::ModifierKeys::noModifiers });
	actionBindings.set(InputAction::Down, ActionBinding{ juce::KeyPress::downKey, juce::ModifierKeys::noModifiers });

	// next

	actionBindings.set(InputAction::NextLeft, ActionBinding{ juce::KeyPress::leftKey, juce::ModifierKeys::ctrlModifier });
	actionBindings.set(InputAction::NextRight, ActionBinding{ juce::KeyPress::rightKey, juce::ModifierKeys::ctrlModifier });
	actionBindings.set(InputAction::NextUp, ActionBinding{ juce::KeyPress::upKey, juce::ModifierKeys::ctrlModifier });
	actionBindings.set(InputAction::NextDown, ActionBinding{ juce::KeyPress::downKey, juce::ModifierKeys::ctrlModifier });

	// actions

	actionBindings.set(InputAction::Select, ActionBinding{ 'S', juce::ModifierKeys::noModifiers});
	actionBindings.set(InputAction::Write, ActionBinding{ 'D', juce::ModifierKeys::noModifiers});
	actionBindings.set(InputAction::Delete, ActionBinding{ 'D', juce::ModifierKeys::shiftModifier });

	// undo/redo

	actionBindings.set(InputAction::Undo, ActionBinding{ 'Z', juce::ModifierKeys::noModifiers });
	actionBindings.set(InputAction::Redo, ActionBinding{ 'Z', juce::ModifierKeys::shiftModifier });

	// play

	actionBindings.set(InputAction::Play, ActionBinding{ juce::KeyPress::spaceKey, juce::ModifierKeys::noModifiers });
	actionBindings.set(InputAction::PlayFromStart, ActionBinding{ juce::KeyPress::spaceKey, juce::ModifierKeys::shiftModifier });

	initSDL();
	startTimerHz(60);

}

InputManager::~InputManager()
{
	quitSDL();
}

void InputManager::timerCallback()
{
	pollSDL();
}

bool InputManager::isCurrentlyPressed(InputAction action)
{
	auto binding = getBinding(action);
	return juce::KeyPress::isKeyCurrentlyDown(binding.keyCode) && juce::ModifierKeys::getCurrentModifiers().getRawFlags() == binding.modifiers;
}

bool InputManager::validate(int keyCode, InputAction action) const
{
	if (actionBindings.contains(action))
	{
		auto binding = actionBindings[action];
		return binding.keyCode == keyCode;
	}
	return false;
}

bool InputManager::validate(int keyCode, juce::ModifierKeys modifiers, InputAction action) const
{
	if (actionBindings.contains(action))
	{
		auto binding = actionBindings[action];
		return binding.keyCode == keyCode && binding.modifiers == modifiers.getRawFlags();
	}
	return false;
}

ActionBinding InputManager::getBinding(InputAction action) const
{
	return actionBindings[action];
}

void InputManager::initSDL()
{
	// 1. Allow the application to capture gamepad inputs while running in the background
	SDL_SetHint(SDL_HINT_JOYSTICK_ALLOW_BACKGROUND_EVENTS, "1");

	// 2. Initialize ONLY the Gamepad subsystem
	if (!SDL_InitSubSystem(SDL_INIT_GAMEPAD)) {
		DBG("Failed to init SDL Gamepad: " << SDL_GetError());
		return;
	}
}

void InputManager::quitSDL()
{
	SDL_QuitSubSystem(SDL_INIT_GAMEPAD);
}

void InputManager::pollSDL()
{
	SDL_Event event;
	// 3. Pump and poll OS events safely
	while (SDL_PollEvent(&event)) {
		switch (event.type) {
		case SDL_EVENT_GAMEPAD_ADDED: {
			// Open the device by its instance identifier
			SDL_Gamepad* gamepad = SDL_OpenGamepad(event.gdevice.which);
			if (gamepad) {
				DBG("Controller Connected: " << SDL_GetGamepadName(gamepad));
			}
			break;
		}
		case SDL_EVENT_GAMEPAD_REMOVED: {
			DBG("Controller Disconnected: " << event.gdevice.which);
			break;
		}
		case SDL_EVENT_GAMEPAD_BUTTON_DOWN: {
			if (!pressedButtons.contains(event.gbutton.button))
			{
				pressedButtons.set(event.gbutton.button, 0);
				
				//buttonPressed(static_cast<SDL_GamepadButton>(event.gbutton.button));
			}
			break;
		}
		case SDL_EVENT_GAMEPAD_BUTTON_UP: {
			if (pressedButtons.contains(event.gbutton.button))
			{
				pressedButtons.remove(event.gbutton.button);
				
				//evaluateInputsStillActive();
			}
			break;
		}
		case SDL_EVENT_GAMEPAD_AXIS_MOTION: {
			// Thumbstick axis values range from -32768 to 32767
			// You can use these to control tracks fader volumes or pan parameters
			int16_t axisValue = event.gaxis.value;
			if (event.gaxis.axis == SDL_GAMEPAD_AXIS_LEFTX && abs(axisValue) > 8000) { // Deadzone filter
				DBG("Left Stick Motion: " << axisValue);
			}
			break;
		}
		}
	}

	juce::HashMap<int, int>::Iterator i(pressedButtons);
	while (i.next())
	{
		auto time = i.getValue();
		if (time > 0 && time >= FUME_INPUT_REPEAT_DELAY_FRAMES && (time - FUME_INPUT_REPEAT_DELAY_FRAMES) % FUME_INPUT_REPEAT_FRAMES == 0)
		{
			// buttonPressed(static_cast<SDL_GamepadButton>(i.getKey()));
		}
		pressedButtons.set(i.getKey(), time + 1);
	}
}

