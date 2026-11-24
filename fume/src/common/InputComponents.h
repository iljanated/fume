#pragma once

#include <JuceHeader.h>
#include "UIContext.h"
#include "Utilities.h"

namespace te = tracktion;

namespace fumeUI
{
	class ParameterSliderAttachment : private te::AutomatableParameter::Listener,
		private juce::Slider::Listener,
		private juce::AsyncUpdater
	{
	public:
		ParameterSliderAttachment(te::AutomatableParameter::Ptr param, juce::Slider& s);

		~ParameterSliderAttachment() override;

	private:
		te::AutomatableParameter::Ptr parameter;
		juce::Slider& slider;
		bool isUpdatingFromSlider = false;
		bool isUpdatingFromParameter = false;
		bool isDragging = false;

		// 1. UI -> Engine: User moves the slider
		void sliderValueChanged(juce::Slider*) override;

		// 2. Touch / Latch Gesture Handling
		void sliderDragStarted(juce::Slider*) override;

		void sliderDragEnded(juce::Slider*) override;

		void startAutomationGesture();

		void endAutomationGesture();

		// 3. Engine -> UI: Automation playback or external control changes the parameter
		void curveHasChanged(te::AutomatableParameter&) override;

		void currentValueChanged(te::AutomatableParameter&) override;

		void handleAsyncUpdate() override;
	};

	class BorderedFlexContainer : public juce::Component
	{
	public:
		BorderedFlexContainer(juce::Colour borderCol = juce::Colours::grey, float thickness = 1.5f)
			: borderColour(borderCol), borderThickness(thickness)
		{
			// Allows mouse clicks to pass through to underlying components if needed
			setInterceptsMouseClicks(false, true);
		}

		void paint(juce::Graphics& g) override
		{
			g.setColour(borderColour);
			g.drawRect(getLocalBounds().toFloat(), borderThickness);
		}

		void resized() override
		{
			// Shrink the layout area slightly so children don't cover the border
			auto innerBounds = getLocalBounds().reduced(static_cast<int> (borderThickness) + 4);
			flexBox.performLayout(innerBounds);
		}

		juce::FlexBox flexBox;

	private:
		juce::Colour borderColour;
		float borderThickness;
	};

	class KeyboardSlider : public juce::Slider,
		private juce::Timer
	{
	public:
		KeyboardSlider(juce::String undoMessage, juce::UndoManager* um);

		~KeyboardSlider() override;

		bool keyPressed(const juce::KeyPress& key) override;

		void focusLost(FocusChangeType cause) override;

		int getUndoWindowMs() const { return undoWindowMs; }
		void setUndoWindowMs(int ms) { undoWindowMs = ms; }

	private:
		void timerCallback() override;

		void finishKeyboardEdit();

		juce::String undoMessage = "Change";
		juce::UndoManager* undoManager;
		bool isKeyboardEditing = false;
		int undoWindowMs = 500; // Time in milliseconds to wait before finalizing the keyboard edit

		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(KeyboardSlider)
	};

	class TimePositionSlider : public KeyboardSlider
	{
	public:
		TimePositionSlider(fumeUI::UIContext& context, juce::String undoMessage, juce::UndoManager* um)
			: KeyboardSlider(undoMessage, um), uiContext(context)
		{
		}

		bool keyPressed(const juce::KeyPress& key) override
		{
			if (key == juce::KeyPress::upKey || key == juce::KeyPress::rightKey)
			{
				double nextVal = calculateStep(getValue(), 0.001);
				setValue(nextVal, juce::sendNotificationSync);
				return true; // Consume the keypress
			}

			if (key == juce::KeyPress::downKey || key == juce::KeyPress::leftKey)
			{
				double prevVal = calculateStep(getValue(), -0.001);
				setValue(prevVal, juce::sendNotificationSync);
				return true; // Consume the keypress
			}

			return juce::Slider::keyPressed(key);
		}

		double snapValue(double attemptedValue, DragMode dragMode) override
		{
			auto currentValue = getValue();
			return calculateStep(currentValue, attemptedValue - currentValue);
		}

		juce::String getTextFromValue(double value) override
		{
			auto& tempoSequence = uiContext.extendedEdit.getTempoSequence();
			auto barsAndBeats = tempoSequence.timeToBarsBeats(te::TimePosition::fromSeconds(value));

			return fumeUI::beatToString(barsAndBeats, false);
		}

	private:
		fumeUI::UIContext& uiContext;
		te::TimecodeSnapType snapType{ te::TimecodeType::barsBeats, 9 };

		double calculateStep(double currentValue, double offset)
		{
			auto& tempoSequence = uiContext.extendedEdit.getTempoSequence();

			if (offset > 0)
			{
				auto nextValue = snapType.roundTimeUp(te::TimePosition::fromSeconds(currentValue + offset), tempoSequence);
				return nextValue.inSeconds();
			}
			else
			{
				auto prevValue = snapType.roundTimeDown(te::TimePosition::fromSeconds(currentValue + offset), tempoSequence);
				return prevValue.inSeconds();
			}
		}

		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TimePositionSlider)
	};

	class ADSRComponent : public juce::Component
	{
	public:
		ADSRComponent(UIContext& context);

		~ADSRComponent() override = default;
		void resized() override;

		KeyboardSlider& getAttackSlider() { return attackSlider; }
		KeyboardSlider& getDecaySlider() { return decaySlider; }
		KeyboardSlider& getSustainSlider() { return sustainSlider; }
		KeyboardSlider& getReleaseSlider() { return releaseSlider; }
	private:
		UIContext& uiContext;
		KeyboardSlider attackSlider{ "Attack", &uiContext.extendedEdit.getUndoManager() };
		KeyboardSlider decaySlider{ "Decay", &uiContext.extendedEdit.getUndoManager() };
		KeyboardSlider sustainSlider{ "Sustain", &uiContext.extendedEdit.getUndoManager() };
		KeyboardSlider releaseSlider{ "Release", &uiContext.extendedEdit.getUndoManager() };

		juce::Label attackLabel{ {}, "A" };
		juce::Label decayLabel{ {}, "D" };
		juce::Label sustainLabel{ {}, "S" };
		juce::Label releaseLabel{ {}, "R" };

		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ADSRComponent)
	};
}