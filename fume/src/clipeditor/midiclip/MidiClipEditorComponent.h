#pragma once

#include <JuceHeader.h>
#include "Utilities.h"
#include "ClipEditorImplementationComponent.h"
#include "ManualViewport.h"
#include "MidiEventsEditorComponent.h"
#include "PianoRollComponent.h"
#include "TimelineComponent.h"


class MidiClipEditorComponent : public ClipEditorImplementationComponent, private juce::ComponentListener
{
public:
    MidiClipEditorComponent(te::MidiClip* clip, fumeUI::UIContext& context);
	~MidiClipEditorComponent() override;
	te::Clip* getClip() override;
	void resized() override;

private:
    ExtendedMidiClip midiClip;
	ManualViewport midiEventsViewport;
	MidiEventsEditorComponent midiEventsEditorComponent;
	PianoRollComponent pianoRollComponent;
	TimelineComponent timelineComponent;

	void componentMovedOrResized(juce::Component& component, bool wasMoved, bool wasResized) override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MidiClipEditorComponent)
};
