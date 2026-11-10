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
		noteHeight = static_cast<int>(fumeUI::midiEditorNoteHeight * newZoomY);
		zoomY.setValue(newZoomY, nullptr);
	}

	float getScrollX() const { return scrollX; }
	void setScrollX(float newScrollX) { scrollX.setValue(newScrollX, nullptr); }

	float getScrollY() const { return scrollY; }
	void setScrollY(float newScrollY) { scrollY.setValue(newScrollY, nullptr); }

	double getCursorX() const { return cursorX; }
	void setCursorX(double newCursorX) { cursorX.setValue(newCursorX, nullptr); }

	double getCursorY() const { return cursorY; }
	void setCursorY(double newCursorY) { cursorY.setValue(newCursorY, nullptr); }

	double getNoteLength() const { return noteLength; }
	void setNoteLength(double newNoteLength) { noteLength.setValue(newNoteLength, nullptr); }

	int getNoteVelocity() const { return noteVelocity; }
	void setNoteVelocity(int newNoteVelocity) { noteVelocity.setValue(newNoteVelocity, nullptr); }

	te::QuantisationType& getCursorQuantisation() noexcept { return cursorQuantisation; }
	const te::QuantisationType& getCursorQuantisation() const noexcept { return cursorQuantisation; }

	double getBeatsPerPixel() const { return beatsPerPixel; }
	int getNoteHeight() const { return noteHeight; }

private:
	te::MidiClip::Ptr clip;

	juce::CachedValue<float> zoomX;
	juce::CachedValue<float> zoomY;
	juce::CachedValue<float> scrollX;
	juce::CachedValue<float> scrollY;
	juce::CachedValue<double> cursorX;
	juce::CachedValue<double> cursorY;
	juce::CachedValue<double> noteLength;
	juce::CachedValue<int> noteVelocity;
	te::QuantisationType cursorQuantisation;

	double beatsPerPixel = fumeUI::midiEditorBeatsPerPixel;
	int noteHeight = static_cast<int>(fumeUI::midiEditorNoteHeight);

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ExtendedMidiClip)
};