#pragma once

#include <JuceHeader.h>
#include "Utilities.h"

class ExtendedMidiClip
{
public:
    ExtendedMidiClip(te::MidiClip& c, UndoManager* um) : clip(c)
    {
        zoomX.referTo(clip.state, FumeIDs::ZoomX, nullptr, 1.0f);
		zoomY.referTo(clip.state, FumeIDs::ZoomY, nullptr, 1.0f);
		scrollX.referTo(clip.state, FumeIDs::ScrollX, nullptr, 0);
		scrollY.referTo(clip.state, FumeIDs::ScrollY, nullptr, 64 * fumeUI::midiEditorNoteHeight);
    }

    te::MidiClip& getClip() const { return clip; }

    juce::CachedValue<float> zoomX;
	juce::CachedValue<float> zoomY;
	juce::CachedValue<float> scrollX;
	juce::CachedValue<float> scrollY;

private:
    te::MidiClip& clip;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ExtendedMidiClip)
};