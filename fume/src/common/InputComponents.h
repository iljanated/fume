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

		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ParameterSliderAttachment)
	};

	class LabeledSliderContainer : public juce::Component
	{
	public:
		LabeledSliderContainer(juce::Slider& slider, juce::String text)
			: slider(slider), label({}, text)
		{
			label.setJustificationType(juce::Justification::centred);
			addAndMakeVisible(slider);
			addAndMakeVisible(label);
		}
		void resized() override
		{
			auto bounds = getLocalBounds();
			slider.setBounds(bounds.removeFromTop(bounds.getHeight() - 20));
			label.setBounds(bounds);
		}

		juce::Slider& getSlider() { return slider; }
		juce::Label& getLabel() { return label; }

	private:
		juce::Slider& slider;
		juce::Label label;

		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LabeledSliderContainer)
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

		~TimePositionSlider() override = default;

		bool keyPressed(const juce::KeyPress& key) override;

		double snapValue(double attemptedValue, DragMode dragMode) override;

		juce::String getTextFromValue(double value) override;

	private:
		fumeUI::UIContext& uiContext;
		te::TimecodeSnapType snapType{ te::TimecodeType::barsBeats, 9 };

		double calculateStep(double currentValue, double offset);

		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TimePositionSlider)
	};

	class OptionsSlider : public KeyboardSlider
	{
	public:
		OptionsSlider(juce::String undoMessage, juce::UndoManager* um, juce::StringArray options)
			: KeyboardSlider(undoMessage, um), optionsArray(std::move(options))
		{
		}

		~OptionsSlider() override = default;

		juce::String getTextFromValue(double value) override
		{
			int intValue = static_cast<int>(value);

			if (intValue < 0 || intValue >= optionsArray.size())
			{
				return "";
			}
			return optionsArray[intValue];
		}

	private:
		juce::StringArray optionsArray;

		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OptionsSlider)
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