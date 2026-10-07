#pragma once
#include "InputManager.h"

InputManager::InputManager()
{
	actionBindings.set(static_cast<int>(InputManagerAction::DPAD_LEFT), { InputManagerActionBinding{ InputManagerInputType::KeyCode, KeyPress::leftKey } });
	actionBindings.set(static_cast<int>(InputManagerAction::DPAD_RIGHT), { InputManagerActionBinding{ InputManagerInputType::KeyCode, KeyPress::rightKey } });
	actionBindings.set(static_cast<int>(InputManagerAction::DPAD_UP), { InputManagerActionBinding{ InputManagerInputType::KeyCode, KeyPress::upKey } });
	actionBindings.set(static_cast<int>(InputManagerAction::DPAD_DOWN), { InputManagerActionBinding{ InputManagerInputType::KeyCode, KeyPress::downKey } });
	actionBindings.set(static_cast<int>(InputManagerAction::Start), { InputManagerActionBinding{ InputManagerInputType::KeyCode, KeyPress::returnKey } });
	actionBindings.set(static_cast<int>(InputManagerAction::A), { InputManagerActionBinding{ InputManagerInputType::KeyCode, KeyPress::spaceKey } });
	actionBindings.set(static_cast<int>(InputManagerAction::B), { InputManagerActionBinding{ InputManagerInputType::KeyCode, 'C' } });
	actionBindings.set(static_cast<int>(InputManagerAction::X), { InputManagerActionBinding{ InputManagerInputType::KeyCode, 'X' } });
	actionBindings.set(static_cast<int>(InputManagerAction::Y), { InputManagerActionBinding{ InputManagerInputType::KeyCode, 'D' } });
	actionBindings.set(static_cast<int>(InputManagerAction::LB), { InputManagerActionBinding{ InputManagerInputType::Modifier, ModifierKeys::shiftModifier } });
	actionBindings.set(static_cast<int>(InputManagerAction::RB), { InputManagerActionBinding{ InputManagerInputType::Modifier, ModifierKeys::ctrlModifier } });

	updateKeyCodeToActionMap();
}

InputManager::~InputManager()
{
}

bool InputManager::keyPressed(const KeyPress& key, Component* originatingComponent)
{
	auto keyCode = key.getKeyCode();

	if(keyCodeToActionMap.contains(keyCode))
	{
		auto action = keyCodeToActionMap[keyCode];
		inputMask |= static_cast<int>(action);
		call(&InputManagerListener::onInputAction, static_cast<InputManagerAction>(action), inputMask, true);
		return true;
	}return false;
}

bool InputManager::keyStateChanged(bool isKeyDown, Component* originatingComponent)
{
	bool result = false;
	if (!isKeyDown)
	{
		juce::HashMap<int, juce::Array<InputManagerActionBinding>>::Iterator i(actionBindings);
		while (i.next())
		{
			auto action = i.getKey();
			if(inputMask & static_cast<int>(action))
			{
				auto bindings = i.getValue();
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
				}

				if (allKeysReleased)
				{
					inputMask &= ~static_cast<int>(action);
					call(&InputManagerListener::onInputAction, static_cast<InputManagerAction>(action), inputMask, false);
					result = true;
				}
			}
		}
	}
	return result;
}

bool InputManager::modifierKeysChanged(int modifiers)
{
	juce::HashMap<int, juce::Array<InputManagerActionBinding>>::Iterator i(actionBindings);
	while (i.next())
	{
		auto action = i.getKey();
		auto bindings = i.getValue();

		for (InputManagerActionBinding& binding : bindings)
		{
			if (binding.type == InputManagerInputType::Modifier)
			{
				bool isModifierActive = juce::ModifierKeys::getCurrentModifiersRealtime().testFlags(binding.value);
				bool wasModifierActive = (inputMask & static_cast<int>(action)) != 0;

				if (isModifierActive && !wasModifierActive)
				{
					inputMask |= static_cast<int>(action);
					call(&InputManagerListener::onInputAction, static_cast<InputManagerAction>(action), inputMask, true);
					return true;
				}
				else if (!isModifierActive && wasModifierActive)
				{
					inputMask &= ~static_cast<int>(action);
					call(&InputManagerListener::onInputAction, static_cast<InputManagerAction>(action), inputMask, false);
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
	juce::HashMap<int, juce::Array<InputManagerActionBinding>>::Iterator i(actionBindings);
	while (i.next())
	{
		auto action = i.getKey();
		auto bindings = i.getValue();

		for (InputManagerActionBinding& binding : bindings)
		{
			if (binding.type == InputManagerInputType::KeyCode)
			{
				keyCodeToActionMap.set(binding.value, static_cast<InputManagerAction>(action));
			}
		}
	}
}