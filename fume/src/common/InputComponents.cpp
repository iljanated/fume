#include "InputComponents.h"

namespace fumeUI
{
    KeyboardSlider::KeyboardSlider(juce::String undoMessage, juce::UndoManager* um)
        : undoMessage(undoMessage), undoManager(um)
    {
        setSliderStyle(juce::Slider::SliderStyle::LinearBarVertical);
        setTextBoxIsEditable(false);
        setWantsKeyboardFocus(true);
        setHasFocusOutline(true);
    }
    
    KeyboardSlider::~KeyboardSlider()
    {
        stopTimer();
    }

    bool KeyboardSlider::keyPressed(const juce::KeyPress& key)
    {
        // Only intercept if this slider actually has focus
        if (hasKeyboardFocus(true))
        {
            if (!isKeyboardEditing)
            {
                isKeyboardEditing = true;
                if (undoManager)
                {
                    undoManager->beginNewTransaction(undoMessage);
                }
            }

            // Reset the 500ms debounce timer on every keystroke
            startTimer(undoWindowMs);
        }

        return juce::Slider::keyPressed(key);
    }

    void KeyboardSlider::focusLost(FocusChangeType cause)
    {
        juce::Slider::focusLost(cause);
        finishKeyboardEdit();
    }

    void KeyboardSlider::timerCallback()
    {
        finishKeyboardEdit();
    }

    void KeyboardSlider::finishKeyboardEdit()
    {
        if (isKeyboardEditing)
        {
            stopTimer();
            isKeyboardEditing = false;
            if (undoManager)
            {
                undoManager->beginNewTransaction();
            }
        }
    }
}