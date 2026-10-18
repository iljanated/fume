#pragma once

#include <JuceHeader.h>

class ManualViewport : public juce::Viewport
{
public:
    ManualViewport() = default;

    bool keyPressed(const juce::KeyPress& key) override
    {
        if (key == juce::KeyPress::leftKey ||
            key == juce::KeyPress::rightKey ||
            key == juce::KeyPress::upKey ||
            key == juce::KeyPress::downKey)
        {
            return false;
        }
        return juce::Viewport::keyPressed(key);
    }
};