#pragma once
#include <JuceHeader.h>
#include "InputManager.h"
#include "EditContainerComponent.h"
#include "OptionsComponent.h"
#include "EditManager.h"

namespace te = tracktion;

class MainComponent : public juce::Component, public EditManager::Listener
{
public:
    //==============================================================================
    MainComponent();

    ~MainComponent() override;

    //==============================================================================
    void paint(juce::Graphics& g) override;

	void resized() override;

	bool keyPressed(const juce::KeyPress& key) override;

    void onEditUnloaded() override;

	void onEditLoaded(te::Edit* newEdit) override;

    void systemRequestedToQuit();

private:
    //==============================================================================
    // JUCE en Tracktion objecten (volgorde van declaratie bepaalt de destructie-volgorde)
    te::Engine engine;
	te::SelectionManager selectionManager{ engine };
    te::Edit* edit = nullptr;
    InputManager inputManager;
	EditManager editManager{ engine };
    std::unique_ptr<EditContainerComponent> editContainerComponent;
    OptionsComponent optionsComponent{ engine, inputManager, editManager };
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};
