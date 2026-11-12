#pragma once
#include "TransportComponent.h"

namespace te = tracktion;

TransportComponent::TransportComponent(fumeUI::UIContext& c)
    : uiContext(c)
{
	addAndMakeVisible(recordButton);
	recordButton.addListener(this); 
	recordButton.setHasFocusOutline(true);
	addAndMakeVisible(playButton);
	playButton.addListener(this);
	playButton.setHasFocusOutline(true);

	addAndMakeVisible(tempoSlider);
	tempoSlider.setSliderStyle(juce::Slider::SliderStyle::LinearBarVertical);
	tempoSlider.setRange(20.0, 300.0, 1.0);
	tempoSlider.setHasFocusOutline(true);
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
	tempoSlider.grabKeyboardFocus();

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