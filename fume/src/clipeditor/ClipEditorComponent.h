#pragma once
#include <JuceHeader.h>
#include "Utilities.h"   
#include "ClipEditorImplementationComponent.h"

class ClipEditorComponent : public juce::Component, private juce::ChangeListener
{
public:
    ClipEditorComponent(fumeUI::UIContext& context);
    ~ClipEditorComponent() override;

    void paint(juce::Graphics& g) override;
    void resized() override;

    void focusGained(FocusChangeType cause) override { 
        if(implementationComponent)
        {
            implementationComponent->grabKeyboardFocus(); 
        }
    }
    void focusLost(FocusChangeType cause) override {}

private:
    fumeUI::UIContext& uiContext;
	std::unique_ptr<ClipEditorImplementationComponent> implementationComponent;

	void changeListenerCallback(juce::ChangeBroadcaster* source) override;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ClipEditorComponent)
};
