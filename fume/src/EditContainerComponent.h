#pragma once
#include <JuceHeader.h>
#include "ClipEditorComponent.h"
#include "Utilities.h"
#include "TrackListComponent.h"
#include "TransportComponent.h"

namespace te = tracktion;

class EditContainerComponent : public juce::Component, public InputManagerListener
{
public:
    //==============================================================================
    EditContainerComponent(te::Engine& engine, te::Edit& edit, te::SelectionManager& selectionManager, InputManager& inputManager);

    ~EditContainerComponent() override;

    //==============================================================================
    void resized() override;

	void onInputAction(InputManagerActionId actionId, int inputMask, bool isActive) override;

private:
    //==============================================================================
    // JUCE en Tracktion objecten (volgorde van declaratie bepaalt de destructie-volgorde)
	fumeUI::UIContext uiContext;

    Viewport trackListViewport;
	ClipEditorComponent clipEditorComponent{ uiContext };
    TrackListComponent trackListComponent{ uiContext, trackListViewport };
    TransportComponent transportComponent{ uiContext };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EditContainerComponent)
};
