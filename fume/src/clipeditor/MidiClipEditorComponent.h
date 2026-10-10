#pragma once

#include <JuceHeader.h>
#include "Utilities.h"
#include "ClipEditorImplementationComponent.h"

class MidiClipEditorComponent : public ClipEditorImplementationComponent
{
	public:
    MidiClipEditorComponent(te::MidiClip* clip, fumeUI::UIContext& context);
	~MidiClipEditorComponent();
	void paint(juce::Graphics& g) override;
    te::Clip* getClip() override;

private:
    te::MidiClip* midiClip;
    fumeUI::UIContext& uiContext;
};
