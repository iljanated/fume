#pragma once
#include "TransportComponent.h"

namespace te = tracktion;

TransportComponent::TransportComponent(fumeUI::UIContext& c)
    : uiContext(c)
{
    addAndMakeVisible(playButton);
	playButton.addListener(this);
	
	auto focusTraverser = Component::createKeyboardFocusTraverser();
	auto comps = focusTraverser->getAllComponents(this);
	for (auto& comp : comps) {
		comp->setWantsKeyboardFocus(false);
		comp->setMouseClickGrabsKeyboardFocus(false);
	}
}

TransportComponent::~TransportComponent()
{
}

void TransportComponent::paint(juce::Graphics& g)
{
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