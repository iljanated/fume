#pragma once
#include "TransportComponent.h"

namespace te = tracktion;

TransportComponent::TransportComponent(te::TransportControl& t, InputManager& i)
    : transport(t),
      inputManager(i)
{
    addAndMakeVisible(playButton);
	playButton.addListener(this);
	inputManager.add(this);

	auto focusTraverser = Component::createKeyboardFocusTraverser();
	auto comps = focusTraverser->getAllComponents(this);
	for (auto& comp : comps) {
		comp->setWantsKeyboardFocus(false);
		comp->setMouseClickGrabsKeyboardFocus(false);
	}
}

TransportComponent::~TransportComponent()
{
	inputManager.remove(this);
}

void TransportComponent::paint(juce::Graphics& g)
{
}

void TransportComponent::onInputAction(InputManagerAction action, int inputMask, bool isActive)
{
	if (action == InputManagerAction::Start && isActive)
	{
		bool fromStart = inputMask & static_cast<int>(InputManagerAction::LT);
		togglePlay(fromStart);
	}
}

void TransportComponent::resized()
{
    playButton.setBounds(getLocalBounds().removeFromLeft(100));
}

void TransportComponent::buttonClicked(juce::Button* button)
{
	if (button == &playButton)
	{
		togglePlay(false);
	}
}

void TransportComponent::togglePlay(bool fromStart)
{
	if (transport.isPlaying())
	{
		transport.stop(false, false);
	}
	else
	{
		if (fromStart)
		{
			transport.playFromStart(false);
		}
		else
		{
			transport.play(false);
		}
	}
}