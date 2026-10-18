#pragma once
#include <JuceHeader.h>
#include "ClipEditorComponent.h"
#include "Utilities.h"
#include "TrackListContainerComponent.h"
#include "TransportComponent.h"

namespace te = tracktion;

class EditContainerComponent : public juce::Component
{
public:
    //==============================================================================
    EditContainerComponent(te::Engine& engine, te::Edit& edit, te::SelectionManager& selectionManager, InputManager& inputManager);

    ~EditContainerComponent() override;

    //==============================================================================
    void resized() override;

	void visibilityChanged() override;

	bool keyPressed(const juce::KeyPress& key) override;

private:
    //==============================================================================
    // JUCE en Tracktion objecten (volgorde van declaratie bepaalt de destructie-volgorde)
	fumeUI::UIContext uiContext;

	TrackListContainerComponent trackListContainerComponent{ uiContext };
    ClipEditorComponent clipEditorComponent{ uiContext };
    TransportComponent transportComponent{ uiContext };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EditContainerComponent)
};
