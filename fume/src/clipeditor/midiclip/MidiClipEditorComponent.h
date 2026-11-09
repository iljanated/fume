#pragma once

#include <JuceHeader.h>
#include "UIMidiClipContext.h"
#include "ClipEditorImplementationComponent.h"
#include "ManualViewport.h"
#include "MidiEventsEditorComponent.h"
#include "PianoRollComponent.h"
#include "MidiClipTimelineComponent.h"


class MidiClipEditorComponent : public ClipEditorImplementationComponent, private juce::ComponentListener
{
public:
    MidiClipEditorComponent(te::MidiClip* clip, fumeUI::UIContext& context);
	~MidiClipEditorComponent() override;
	te::Clip* getClip() override;
	void resized() override;
	void focusGained(FocusChangeType cause) override;
	void focusLost(FocusChangeType cause) override {}

private:
    ExtendedMidiClip midiClip;
	fumeUI::UIMidiClipContext uiContext;
	ManualViewport midiEventsViewport;
	MidiEventsEditorComponent midiEventsEditorComponent;
	PianoRollComponent pianoRollComponent;
	MidiClipTimelineComponent midiClipTimelineComponent;

	void componentMovedOrResized(juce::Component& component, bool wasMoved, bool wasResized) override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MidiClipEditorComponent)
};
