#pragma once
#include <JuceHeader.h>
#include "InputManager.h"
#include "TrackListComponent.h"
#include "TransportComponent.h"

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

private:
    //==============================================================================
    // JUCE en Tracktion objecten (volgorde van declaratie bepaalt de destructie-volgorde)
    te::Engine engine;
    te::Edit edit{ engine, te::Edit::EditRole::forEditing };
    te::TransportControl& transport{ edit.getTransport() };

    InputManager inputManager;

    // Component voor het beheren van audio-inputs en outputs
	Viewport trackListViewport;
    TrackListComponent trackListComponent{ edit, inputManager, trackListViewport };
	TransportComponent transportComponent{ transport, inputManager };
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};
