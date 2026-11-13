#pragma once
#include "TransportComponent.h"

namespace te = tracktion;

TransportComponent::TransportComponent(fumeUI::UIContext& c)
	: uiContext(c)
{
	setFocusContainerType(FocusContainerType::focusContainer);
	setWantsKeyboardFocus(true);

	addAndMakeVisible(recordButton);
	recordButton.addListener(this); 
	recordButton.setHasFocusOutline(true);
	addAndMakeVisible(playButton);
	playButton.addListener(this);
	playButton.setHasFocusOutline(true);

	addAndMakeVisible(tempoSlider);
	tempoSlider.setSliderStyle(juce::Slider::SliderStyle::LinearBarVertical);
	tempoSlider.setTextBoxIsEditable(false);
	tempoSlider.setWantsKeyboardFocus(true);
	tempoSlider.setRange(20.0, 300.0, 1.0);
	tempoSlider.setHasFocusOutline(true);

	currentlyFocusedComponent = &playButton;
}

TransportComponent::~TransportComponent()
{
}

void TransportComponent::paint(juce::Graphics& g)
{
}

void TransportComponent::resized()
{
	auto bounds = getLocalBounds();
	recordButton.setBounds(bounds.removeFromLeft(100));
	playButton.setBounds(bounds.removeFromLeft(100));
	tempoSlider.setBounds(bounds.removeFromLeft(100));
}

void TransportComponent::focusGained(FocusChangeType cause)
{
	juce::ignoreUnused(cause);

	juce::MessageManager::callAsync([safeThis = juce::Component::SafePointer<TransportComponent>(this)]
	{
		if (safeThis == nullptr || safeThis->currentlyFocusedComponent == nullptr)
			return;

		safeThis->currentlyFocusedComponent->grabKeyboardFocus();
	});
}

bool TransportComponent::keyPressed(const juce::KeyPress& key)
{
	auto& extendedEdit = uiContext.extendedEdit;
	auto& edit = extendedEdit.getEdit();
	auto keyCode = key.getKeyCode();
	auto modifiers = key.getModifiers();
	auto& inputManager = uiContext.inputManager;
	if (inputManager.validate(keyCode, modifiers, InputAction::Write))
	{
		playButton.triggerClick();
		return true;
	}
	if (inputManager.validate(keyCode, modifiers, InputAction::MoveLeft))
	{
		auto* focused = getCurrentlyFocusedComponent();
		if (focused)
		{
			focused->moveKeyboardFocusToSibling(false);
		}
		return true;
	}
	if (inputManager.validate(keyCode, modifiers, InputAction::MoveRight))
	{
		auto* focused = getCurrentlyFocusedComponent();
		if (focused)
		{
			focused->moveKeyboardFocusToSibling(true);
		}
		return true;
	}
	return false;
}

void TransportComponent::buttonClicked(juce::Button* button)
{
	if (button == &playButton)
	{
		togglePlay();
	}
}

void TransportComponent::togglePlay()
{
	auto& transport = uiContext.extendedEdit.getEdit().getTransport();

	if (transport.isPlaying())
	{
		transport.stop(false, false);
	}
	else
	{
		transport.play(false);
	}
}