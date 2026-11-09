#pragma once

#include <JuceHeader.h>
#include "Utilities.h"

namespace te = tracktion;

class ExtendedEdit
{
public:
    ExtendedEdit(te::Edit& e);
	~ExtendedEdit() = default;

    te::Edit& getEdit() const { return edit; }

	juce::ValueTree getState() const { return edit.state; }

	juce::UndoManager& getUndoManager() noexcept
	{
		return edit.getUndoManager();
	}

	te::TempoSequence& getTempoSequence() { return edit.tempoSequence; }   

	float getZoomX() const { return zoomX; }
	void setZoomX(float newZoomX) { 
		secondsPerPixel = fumeUI::timelineSecondsPerPixel / newZoomX;
		zoomX.setValue(newZoomX, nullptr);
	}

	float getZoomY() const { return zoomY; }
	void setZoomY(float newZoomY) { 
		trackHeight = static_cast<int>(fumeUI::timelineTrackHeight * newZoomY);
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

    double getSecondsPerPixel() const { return secondsPerPixel; }
    int getTrackHeight() const { return trackHeight; }

private:
    te::Edit& edit;
    juce::CachedValue<float> zoomX;
    juce::CachedValue<float> zoomY;
    juce::CachedValue<float> scrollX;
    juce::CachedValue<float> scrollY;
	juce::CachedValue<double> cursorX;
	juce::CachedValue<double> cursorY;

	// cached properties for performance
	double secondsPerPixel = fumeUI::timelineSecondsPerPixel;
	int trackHeight = static_cast<int>(fumeUI::timelineTrackHeight);
	
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ExtendedEdit)
};