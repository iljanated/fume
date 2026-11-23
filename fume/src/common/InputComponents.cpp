#include "InputComponents.h"

namespace fumeUI
{
    ParameterSliderAttachment::ParameterSliderAttachment(te::AutomatableParameter::Ptr param, juce::Slider& s)
        : parameter(std::move(param)), slider(s)
    {
        jassert(parameter != nullptr);

        auto range = parameter->getValueRange();
        slider.setRange(range.getStart(), range.getEnd(), 0.0);
        slider.setValue(parameter->getCurrentValue(), juce::dontSendNotification);

        parameter->addListener(this);
        slider.addListener(this);
    }

    ParameterSliderAttachment::~ParameterSliderAttachment()
    {
        // Ensure any active touch/latch gesture is cleanly closed if destroyed mid-drag
        if (isDragging)
        {
            endAutomationGesture();
        }

        slider.removeListener(this);
        if (parameter != nullptr)
        {
            parameter->removeListener(this);
        }
        cancelPendingUpdate();
    }

    // 1. UI -> Engine: User moves the slider
    void ParameterSliderAttachment::sliderValueChanged(juce::Slider*)
    {
        if (isUpdatingFromParameter || parameter == nullptr)
        {
            return;
        }

        const juce::ScopedValueSetter<bool> svs(isUpdatingFromSlider, true);
        const float newValue = static_cast<float>(slider.getValue());

        // setParameter triggers automation point writing when automation record is enabled
        parameter->setParameter(newValue, juce::sendNotification);
    }

    // 2. Touch / Latch Gesture Handling
   void ParameterSliderAttachment::sliderDragStarted(juce::Slider*)
    {
        if (parameter == nullptr)
        {
            return;
        }

        startAutomationGesture();
    }

   void ParameterSliderAttachment::sliderDragEnded(juce::Slider*)
   {
       if (parameter == nullptr || !isDragging)
       {
           return;
       }

       endAutomationGesture();
   }

   void ParameterSliderAttachment::startAutomationGesture()
   {
       if (parameter != nullptr && !isDragging)
       {
           isDragging = true;
           parameter->parameterChangeGestureBegin();
       }
   }

   void ParameterSliderAttachment::endAutomationGesture()
   {
       isDragging = false;

       if (parameter != nullptr)
       {
           parameter->parameterChangeGestureEnd();
       }
   }

   // 3. Engine -> UI: Automation playback or external control changes the parameter
   void ParameterSliderAttachment::curveHasChanged(te::AutomatableParameter&)
   {
       triggerAsyncUpdate();
   }

   void ParameterSliderAttachment::currentValueChanged(te::AutomatableParameter&)
   {
       // Avoid fighting the user's mouse while they are actively dragging
       if (!isDragging)
       {
           triggerAsyncUpdate();
       }
   }

   void ParameterSliderAttachment::handleAsyncUpdate()
   {
       if (isUpdatingFromSlider || isDragging || parameter == nullptr)
       {
           return;
       }

       const juce::ScopedValueSetter<bool> svs(isUpdatingFromParameter, true);
       slider.setValue(parameter->getCurrentValue(), juce::dontSendNotification);
   }



    KeyboardSlider::KeyboardSlider(juce::String undoMessage, juce::UndoManager* um)
        : undoMessage(undoMessage), undoManager(um)
    {
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