#pragma once
#include <JuceHeader.h>

class FumeLookAndFeel : public juce::LookAndFeel_V4
{
public:
    FumeLookAndFeel()
    {
        // Define your global lasso color tokens here

        // 1. Semi-transparent background fill color (DAW style)
        setColour(juce::LassoComponent<juce::Component*>::lassoFillColourId,
            juce::Colours::blue.withAlpha(0.15f));

        // 2. The bounding outline/border color
        setColour(juce::LassoComponent<juce::Component*>::lassoOutlineColourId,
            juce::Colours::blue.withAlpha(0.8f));
    }
};