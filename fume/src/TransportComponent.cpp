#pragma once
#include "TransportComponent.h"
#include "Utilities.h"

namespace te = tracktion;

TransportComponent::TransportComponent(fumeUI::UIContext& c)
	: uiContext(c), focusTraverser{ uiContext, this }
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

	tempoSlider.setSliderStyle(juce::Slider::SliderStyle::LinearBarVertical);
	addAndMakeVisible(tempoSlider);
	if (auto* tempo = extendedEdit.getTempoSequence().getTempo(0))
	{
		tempoSlider.setRange(20.0, 300.0, 0.1); 
		tempoSlider.getValueObject().referTo(juce::Value(new fumeHelpers::BpmRemapValueSource(edit, *tempo)));
	}
	
	loopPoint1Slider.setSliderStyle(juce::Slider::SliderStyle::LinearBarVertical);
	addAndMakeVisible(loopPoint1Slider);
	loopPoint1Slider.getValueObject().referTo(transport.state.getPropertyAsValue(te::IDs::loopPoint1, nullptr));
	loopPoint1Slider.setRange(0.0, edit.maximumLength, 0.01);

	loopPoint2Slider.setSliderStyle(juce::Slider::SliderStyle::LinearBarVertical);
	addAndMakeVisible(loopPoint2Slider);
	loopPoint2Slider.getValueObject().referTo(transport.state.getPropertyAsValue(te::IDs::loopPoint2, nullptr));
	loopPoint2Slider.setRange(0.0, edit.maximumLength, 0.01);

	loopToggleButton.addListener(this);
	loopToggleButton.setHasFocusOutline(true);
	loopToggleButton.setToggleState(transport.looping, juce::dontSendNotification);
	addAndMakeVisible(loopToggleButton);
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

	juce::FlexBox flexBox;
	flexBox.flexDirection = juce::FlexBox::Direction::row;

	flexBox.items.add(juce::FlexItem(recordButton).withFlex(1.0f).withMargin(1.0f));
	flexBox.items.add(juce::FlexItem(playButton).withFlex(1.0f).withMargin(1.0f));
	flexBox.items.add(juce::FlexItem(tempoSlider).withFlex(1.0f).withMargin(1.0f));
	flexBox.items.add(juce::FlexItem(loopToggleButton).withFlex(1.0f).withMargin(1.0f));
	flexBox.items.add(juce::FlexItem(loopPoint1Slider).withFlex(1.0f).withMargin(1.0f));
	flexBox.items.add(juce::FlexItem(loopPoint2Slider).withFlex(1.0f).withMargin(1.0f));

	flexBox.performLayout(bounds);
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
		auto* currentlyFocusedComponent = dynamic_cast<juce::Button*>(focusTraverser.getCurrentComponent());
		if (currentlyFocusedComponent != nullptr && currentlyFocusedComponent->isEnabled())
		{
			currentlyFocusedComponent->triggerClick();
		}
		return true;
	}
	if (focusTraverser.keyPressed(key))
	{
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

	if (button == &loopToggleButton)
	{
		auto& transport = uiContext.extendedEdit.getEdit().getTransport();
		transport.looping = loopToggleButton.getToggleState();
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
