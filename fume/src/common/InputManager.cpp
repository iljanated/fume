#pragma once
#include "InputManager.h"
#include "Utilities.h"


InputManager::InputManager()
{
	actions.set(static_cast<int>(InputManagerActionId::LEFT),
		{
			InputManagerActionId::LEFT,
			{
				InputManagerActionBinding{ InputManagerInputType::KeyCode, KeyPress::leftKey },
				InputManagerActionBinding{ InputManagerInputType::Button, SDL_GamepadButton::SDL_GAMEPAD_BUTTON_DPAD_LEFT }
			},
			true
		});
	actions.set(static_cast<int>(InputManagerActionId::RIGHT),
		{
			InputManagerActionId::RIGHT,
			{
				InputManagerActionBinding{ InputManagerInputType::KeyCode, KeyPress::rightKey },
				InputManagerActionBinding{ InputManagerInputType::Button, SDL_GamepadButton::SDL_GAMEPAD_BUTTON_DPAD_RIGHT }
			},
			true
		});
	actions.set(static_cast<int>(InputManagerActionId::UP),
		{
			InputManagerActionId::UP,
			{
				InputManagerActionBinding{ InputManagerInputType::KeyCode, KeyPress::upKey },
				InputManagerActionBinding{ InputManagerInputType::Button, SDL_GamepadButton::SDL_GAMEPAD_BUTTON_DPAD_UP }
			},
			true
		});
	actions.set(static_cast<int>(InputManagerActionId::DOWN),
		{
			InputManagerActionId::DOWN,
			{
				InputManagerActionBinding{ InputManagerInputType::KeyCode, KeyPress::downKey },
				InputManagerActionBinding{ InputManagerInputType::Button, SDL_GamepadButton::SDL_GAMEPAD_BUTTON_DPAD_DOWN }
			},
			true
		});
	actions.set(static_cast<int>(InputManagerActionId::PLAY),
		{
			InputManagerActionId::PLAY,
			{
				InputManagerActionBinding{ InputManagerInputType::KeyCode, KeyPress::spaceKey },
				InputManagerActionBinding{ InputManagerInputType::Button, SDL_GamepadButton::SDL_GAMEPAD_BUTTON_START }
			},
			false
		});
	actions.set(static_cast<int>(InputManagerActionId::SELECT),
		{
			InputManagerActionId::SELECT,
			{
				InputManagerActionBinding{ InputManagerInputType::KeyCode, 'S'},
				InputManagerActionBinding{ InputManagerInputType::Button, SDL_GamepadButton::SDL_GAMEPAD_BUTTON_SOUTH }
			},
			false
		});
	actions.set(static_cast<int>(InputManagerActionId::WRITE),
		{
			InputManagerActionId::WRITE,
			{
				InputManagerActionBinding{ InputManagerInputType::KeyCode, 'D' },
				InputManagerActionBinding{ InputManagerInputType::Button, SDL_GamepadButton::SDL_GAMEPAD_BUTTON_EAST }
			},
			false
		});
	actions.set(static_cast<int>(InputManagerActionId::EDIT),
		{
			InputManagerActionId::EDIT,
			{
				InputManagerActionBinding{ InputManagerInputType::KeyCode, 'Q' },
				InputManagerActionBinding{ InputManagerInputType::KeyCode, 'A' },
				InputManagerActionBinding{ InputManagerInputType::Button, SDL_GamepadButton::SDL_GAMEPAD_BUTTON_WEST }
			},
			false
		});
	actions.set(static_cast<int>(InputManagerActionId::DELETE),
		{
			InputManagerActionId::DELETE,
			{
				InputManagerActionBinding{ InputManagerInputType::KeyCode, 'Z' },
				InputManagerActionBinding{ InputManagerInputType::KeyCode, 'W' },
				InputManagerActionBinding{ InputManagerInputType::Button, SDL_GamepadButton::SDL_GAMEPAD_BUTTON_NORTH }
			},
			false
		});
	actions.set(static_cast<int>(InputManagerActionId::MOD1),
		{
			InputManagerActionId::MOD1,
			{
				InputManagerActionBinding{ InputManagerInputType::Modifier, ModifierKeys::shiftModifier },
				InputManagerActionBinding{ InputManagerInputType::Button, SDL_GamepadButton::SDL_GAMEPAD_BUTTON_LEFT_SHOULDER }
			},
			false
		});
	actions.set(static_cast<int>(InputManagerActionId::UNDO),
		{
			InputManagerActionId::UNDO,
			{
				InputManagerActionBinding{ InputManagerInputType::KeyCode, 'E'},
				InputManagerActionBinding{ InputManagerInputType::Button, SDL_GamepadButton::SDL_GAMEPAD_BUTTON_RIGHT_SHOULDER }
			},
			false
		});

	updateKeyCodeToActionMap();
	updateButtonToActionMap();

	initSDL();
	startTimerHz(60);

}

InputManager::~InputManager()
{
	quitSDL();
}

bool InputManager::buttonPressed(SDL_GamepadButton button)
{
	if (buttonToActionMap.contains(button))
	{
		auto actionId = buttonToActionMap[button];
		int actionIdAsInt = static_cast<int>(actionId);

		auto action = actions[actionIdAsInt];
		if ((inputMask & actionIdAsInt) == 0 || action.isRepeating)
		{
			inputMask |= actionIdAsInt;
			call(&InputManagerListener::onInputAction, action.id, inputMask, true);
			return true;
		}
	}
	return false;
}

bool InputManager::keyPressed(const KeyPress& key, Component* originatingComponent)
{
	auto keyCode = key.getKeyCode();

	if (keyCodeToActionMap.contains(keyCode))
	{
		auto actionId = keyCodeToActionMap[keyCode];
		int actionIdAsInt = static_cast<int>(actionId);
		auto action = actions[actionIdAsInt];
		if ((inputMask & actionIdAsInt) == 0 || action.isRepeating)
		{
			inputMask |= actionIdAsInt;
			call(&InputManagerListener::onInputAction, actionId, inputMask, true);
			return true;
		}
	}
	return false;
}


bool InputManager::keyStateChanged(bool isKeyDown, Component* originatingComponent)
{
	bool result = false;
	if (!isKeyDown)
	{
		result = evaluateInputsStillActive();
	}
	return result;
}

bool InputManager::evaluateInputsStillActive()
{
	bool result = false;
	juce::HashMap<int, InputManagerAction>::Iterator i(actions);
	while (i.next())
	{
		auto action = i.getValue();
		int actionIdAsInt = static_cast<int>(action.id);
		if (inputMask & actionIdAsInt)
		{
			auto bindings = action.bindings;
			bool allKeysReleased = true;

			for (InputManagerActionBinding& binding : bindings)
			{
				if (binding.type == InputManagerInputType::KeyCode)
				{
					if (KeyPress::isKeyCurrentlyDown(binding.value))
					{
						allKeysReleased = false;
						break;
					}
				}
				else if (binding.type == InputManagerInputType::Button)
				{
					if (pressedButtons.contains(binding.value))
					{
						allKeysReleased = false;
						break;
					}
				}
			}

			if (allKeysReleased)
			{
				inputMask &= ~actionIdAsInt;
				call(&InputManagerListener::onInputAction, action.id, inputMask, false);
				result = true;
			}
		}
	}
	return result;
}

bool InputManager::modifierKeysChanged(int modifiers)
{
	juce::HashMap<int, InputManagerAction>::Iterator i(actions);
	while (i.next())
	{
		auto action = i.getValue();
		int actionIdAsInt = static_cast<int>(action.id);
		auto bindings = action.bindings;

		for (InputManagerActionBinding& binding : bindings)
		{
			if (binding.type == InputManagerInputType::Modifier)
			{
				bool isModifierActive = juce::ModifierKeys::getCurrentModifiersRealtime().testFlags(binding.value);
				bool wasModifierActive = (inputMask & actionIdAsInt) != 0;

				if (isModifierActive && !wasModifierActive)
				{
					inputMask |= actionIdAsInt;
					call(&InputManagerListener::onInputAction, action.id, inputMask, true);
					return true;
				}
				else if (!isModifierActive && wasModifierActive)
				{
					inputMask &= ~actionIdAsInt;
					call(&InputManagerListener::onInputAction, action.id, inputMask, false);
					return true;
				}
			}
		}
	}
	return false;
}

void InputManager::updateKeyCodeToActionMap()
{
	keyCodeToActionMap.clear();
	juce::HashMap<int, InputManagerAction>::Iterator i(actions);
	while (i.next())
	{
		InputManagerAction action = i.getValue();
		auto bindings = action.bindings;

		for (InputManagerActionBinding& binding : bindings)
		{
			if (binding.type == InputManagerInputType::KeyCode)
			{
				keyCodeToActionMap.set(binding.value, action.id);
			}
		}
	}
}

void InputManager::updateButtonToActionMap()
{
	buttonToActionMap.clear();
	juce::HashMap<int, InputManagerAction>::Iterator i(actions);
	while (i.next())
	{
		auto action = i.getValue();
		auto bindings = action.bindings;

		for (InputManagerActionBinding& binding : bindings)
		{
			if (binding.type == InputManagerInputType::Button)
			{
				buttonToActionMap.set(binding.value, action.id);
			}
		}
	}
}

void InputManager::timerCallback()
{
	pollSDL();
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
				buttonPressed(static_cast<SDL_GamepadButton>(event.gbutton.button));
			}
			break;
		}
		case SDL_EVENT_GAMEPAD_BUTTON_UP: {
			if (pressedButtons.contains(event.gbutton.button))
			{
				pressedButtons.remove(event.gbutton.button);
				evaluateInputsStillActive();
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
			buttonPressed(static_cast<SDL_GamepadButton>(i.getKey()));
		}
		pressedButtons.set(i.getKey(), time + 1);
	}
}

