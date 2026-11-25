#include "InputComponents.h"

namespace fumeUI
{
    ParameterSliderAttachment::ParameterSliderAttachment(te::AutomatableParameter::Ptr param, juce::Slider& s)
        : parameter(std::move(param)), slider(s)
    {
        jassert(parameter != nullptr);

        auto range = parameter->valueRange;
        
        slider.setRange(range.start, range.end, range.interval);
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

        // DBG("range " << slider.getRange().getStart() << " " << slider.getRange().getEnd() << " " << newValue);
		// DBG("parameter range " << parameter->getValueRange().getStart() << " " << parameter->getValueRange().getEnd() << " " << parameter->getCurrentValue());

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
    
    ADSRComponent::ADSRComponent(UIContext& context)
        : uiContext(context)
    {
        attackSlider.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
        attackSlider.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
        addAndMakeVisible(attackSlider);
        attackLabel.setJustificationType(juce::Justification::centred);
        addAndMakeVisible(attackLabel);

        decaySlider.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
        decaySlider.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
        addAndMakeVisible(decaySlider);
        decayLabel.setJustificationType(juce::Justification::centred);
        addAndMakeVisible(decayLabel);

        sustainSlider.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
        sustainSlider.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
        addAndMakeVisible(sustainSlider);
        sustainLabel.setJustificationType(juce::Justification::centred);
        addAndMakeVisible(sustainLabel);

        releaseSlider.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
        releaseSlider.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
        addAndMakeVisible(releaseSlider);
        releaseLabel.setJustificationType(juce::Justification::centred);
        addAndMakeVisible(releaseLabel);
    }
    
    void ADSRComponent::resized()
    {
        auto localBounds = getLocalBounds();
        auto width = localBounds.getWidth() / 4;

        auto labelHeight = 24;
        auto sliderHeight = localBounds.getHeight() - labelHeight;

        auto attackRect = localBounds.removeFromLeft(width);
        attackSlider.setBounds(attackRect.removeFromTop(sliderHeight));
        attackLabel.setBounds(attackRect);

        auto decayRect = localBounds.removeFromLeft(width);
        decaySlider.setBounds(decayRect.removeFromTop(sliderHeight));
        decayLabel.setBounds(decayRect);

        auto sustainRect = localBounds.removeFromLeft(width);
        sustainSlider.setBounds(sustainRect.removeFromTop(sliderHeight));
        sustainLabel.setBounds(sustainRect);

        auto releaseRect = localBounds.removeFromLeft(width);
        releaseSlider.setBounds(releaseRect.removeFromTop(sliderHeight));
        releaseLabel.setBounds(releaseRect);
    }
    
    bool TimePositionSlider::keyPressed(const juce::KeyPress& key)
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
    
    double TimePositionSlider::snapValue(double attemptedValue, DragMode dragMode)
    {
        auto currentValue = getValue();
        return calculateStep(currentValue, attemptedValue - currentValue);
    }
    
    juce::String TimePositionSlider::getTextFromValue(double value)
    {
        auto& tempoSequence = uiContext.extendedEdit.getTempoSequence();
        auto barsAndBeats = tempoSequence.timeToBarsBeats(te::TimePosition::fromSeconds(value));

        return fumeUI::beatToString(barsAndBeats, false);
    }
    
    double TimePositionSlider::calculateStep(double currentValue, double offset)
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
}