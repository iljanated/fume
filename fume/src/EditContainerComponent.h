#pragma once
#include <JuceHeader.h>
#include "InputManager.h"
#include "TrackListComponent.h"
#include "TransportComponent.h"

namespace te = tracktion;

class EditContainerComponent : public juce::Component, public InputManagerListener
{
public:
    //==============================================================================
    EditContainerComponent(te::Edit& edit, InputManager& inputManager);

    ~EditContainerComponent() override;

    //==============================================================================
    void resized() override;

	void onInputAction(InputManagerActionId actionId, int inputMask, bool isActive) override;

private:
    //==============================================================================
    // JUCE en Tracktion objecten (volgorde van declaratie bepaalt de destructie-volgorde)
    te::Edit& edit;
    InputManager& inputManager;

    Viewport trackListViewport;
    TrackListComponent trackListComponent{ edit, inputManager, trackListViewport };
    TransportComponent transportComponent{ edit, inputManager };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EditContainerComponent)
};
