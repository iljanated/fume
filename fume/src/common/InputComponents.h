#pragma once

#include <JuceHeader.h>
#include "UIContext.h"
#include "Utilities.h"

namespace te = tracktion;

namespace fumeUI
{
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
        te::TimecodeSnapType snapType{ te::TimecodeType::barsBeats, 9};

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
}