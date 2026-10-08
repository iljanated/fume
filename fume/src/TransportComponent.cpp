#pragma once
#include "TransportComponent.h"

namespace te = tracktion;

TransportComponent::TransportComponent(te::Edit& e, InputManager& i)
    : edit(e),
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

void TransportComponent::onInputAction(InputManagerActionId actionId, int inputMask, bool isActive)
{
	if (actionId == InputManagerActionId::PLAY && isActive)
	{
		bool fromStart = (inputMask & static_cast<int>(InputManagerActionId::MOD1));
		if (fromStart)
		{
			startPlay(fromStart);
		}
		else
		{
			togglePlay();
		}
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
		togglePlay();
	}
}


void TransportComponent::startPlay(bool fromStart)
{
	auto& transport = edit.getTransport();

	if (fromStart)
	{
		transport.playFromStart(false);
	}
	else
	{
		transport.play(false);
	}
}

void TransportComponent::togglePlay()
{
	auto& transport = edit.getTransport();

	if (transport.isPlaying())
	{
		transport.stop(false, false);
	}
	else
	{
		transport.play(false);
	}
}