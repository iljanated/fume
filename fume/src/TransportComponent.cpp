#pragma once
#include "TransportComponent.h"
#include "Utilities.h"

namespace te = tracktion;

TransportComponent::TransportComponent(fumeUI::UIContext& c)
	: uiContext(c)
{
	auto& extendedEdit = uiContext.extendedEdit;
	auto& edit = extendedEdit.getEdit();
	auto& transport = edit.getTransport();

	setWantsKeyboardFocus(true);

	addAndMakeVisible(recordButton);
	recordButton.addListener(this); 
	recordButton.setHasFocusOutline(true);
	addAndMakeVisible(playButton);
	playButton.addListener(this);
	playButton.setHasFocusOutline(true);

	addAndMakeVisible(tempoSlider);
	if (auto* tempo = extendedEdit.getTempoSequence().getTempo(0))
	{
		tempoSlider.getValueObject().referTo(juce::Value(new fumeHelpers::BpmRemapValueSource(edit, *tempo)));
		tempoSlider.setRange(20.0, 300.0, 0.1);
	}

	addAndMakeVisible(loopPoint1Slider);
	loopPoint1Slider.getValueObject().referTo(transport.state.getPropertyAsValue(te::IDs::loopPoint1, nullptr));
	loopPoint1Slider.setRange(0.0, edit.maximumLength, 0.01);

	addAndMakeVisible(loopPoint2Slider);
	loopPoint2Slider.getValueObject().referTo(transport.state.getPropertyAsValue(te::IDs::loopPoint2, nullptr));
	loopPoint2Slider.setRange(0.0, edit.maximumLength, 0.01);

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
	loopPoint1Slider.setBounds(bounds.removeFromLeft(100));
	loopPoint2Slider.setBounds(bounds.removeFromLeft(100));
}

void TransportComponent::focusGained(FocusChangeType cause)
{
	juce::ignoreUnused(cause);

	juce::MessageManager::callAsync([safeThis = juce::Component::SafePointer<TransportComponent>(this)]
	{
		if (safeThis == nullptr)
			return;

		if (auto* defaultComponent = safeThis->focusTraverser.getDefaultComponent(safeThis.getComponent()))
			defaultComponent->grabKeyboardFocus();
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
		focusComponent({ -1, 0 });
		return true;
	}
	if (inputManager.validate(keyCode, modifiers, InputAction::MoveRight))
	{
		focusComponent({ 1, 0 });
		return true;
	}
	if (inputManager.validate(keyCode, modifiers, InputAction::MoveUp))
	{
		focusComponent({ 0, -1 });
		return true;
	}
	if (inputManager.validate(keyCode, modifiers, InputAction::MoveDown))
	{
		focusComponent({ 0, 1 });
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

void TransportComponent::focusComponent(juce::Point<int> delta)
{
	auto* current = juce::Component::getCurrentlyFocusedComponent();

	if (current == nullptr || (!isParentOf(current) && current != this))
		current = focusTraverser.getDefaultComponent(this);

	if (current == nullptr)
		return;

	auto* next = focusTraverser.getNextComponent(current, delta);

	if (next == nullptr)
		next = focusTraverser.getDefaultComponent(this);

	if (next != nullptr)
		next->grabKeyboardFocus();
}
