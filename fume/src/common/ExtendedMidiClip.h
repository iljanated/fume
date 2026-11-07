#pragma once

#include <JuceHeader.h>
#include "Utilities.h"

class ExtendedMidiClip
{
public:
	ExtendedMidiClip(te::MidiClip* c);

	te::MidiClip& getClip() const { return *clip; }

	juce::ValueTree getState() const { return clip->state; }

	juce::UndoManager* getUndoManager() noexcept
	{
		return clip->getUndoManager();
	}

	float getZoomX() const { return zoomX; }
	void setZoomX(float newZoomX) {
		beatsPerPixel = fumeUI::midiEditorBeatsPerPixel / newZoomX;
		zoomX.setValue(newZoomX, nullptr);
	}

	float getZoomY() const { return zoomY; }
	void setZoomY(float newZoomY) {
		noteHeight = fumeUI::midiEditorNoteHeight * newZoomY;
		zoomY.setValue(newZoomY, nullptr);
	}

	float getScrollX() const { return scrollX; }
	void setScrollX(float newScrollX) { scrollX.setValue(newScrollX, nullptr); }

	float getScrollY() const { return scrollY; }
	void setScrollY(float newScrollY) { scrollY.setValue(newScrollY, nullptr); }

	te::QuantisationType& getCursorQuantisation() noexcept { return cursorQuantisation; }
	const te::QuantisationType& getCursorQuantisation() const noexcept { return cursorQuantisation; }

	double getBeatsPerPixel() const { return beatsPerPixel; }
	double getNoteHeight() const { return noteHeight; }

private:
	te::MidiClip::Ptr clip;

	juce::CachedValue<float> zoomX;
	juce::CachedValue<float> zoomY;
	juce::CachedValue<float> scrollX;
	juce::CachedValue<float> scrollY;
	juce::CachedValue<double> noteLength;
	te::QuantisationType cursorQuantisation;

	double beatsPerPixel = fumeUI::midiEditorBeatsPerPixel;
	double noteHeight = fumeUI::midiEditorNoteHeight;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ExtendedMidiClip)
};