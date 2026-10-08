#pragma once
#include <JuceHeader.h>
#include "InputManager.h"
#include "EditContainerComponent.h"

namespace te = tracktion;

class MainComponent : public juce::Component
{
public:
    //==============================================================================
    MainComponent();

    ~MainComponent() override;

    //==============================================================================
    void paint(juce::Graphics& g) override;

    void resized() override;

	void parentHierarchyChanged() override;

    void modifierKeysChanged(const ModifierKeys& modifiers) override;

    void loadFile();

private:
    //==============================================================================
    // JUCE en Tracktion objecten (volgorde van declaratie bepaalt de destructie-volgorde)
    te::Engine engine;
    std::unique_ptr<te::Edit> edit;
    InputManager inputManager;
    std::unique_ptr<EditContainerComponent> editContainerComponent;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};
