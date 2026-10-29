#pragma once

#include <JuceHeader.h>
#include "Utilities.h"

namespace te = tracktion;

class ExtendedEdit
{
public:
    ExtendedEdit(te::Edit& e, juce::UndoManager* um);
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
		trackHeight = fumeUI::timelineTrackHeight * newZoomY;
		zoomY.setValue(newZoomY, nullptr);
	}

	float getScrollX() const { return scrollX; }
	void setScrollX(float newScrollX) { scrollX.setValue(newScrollX, nullptr); }

	float getScrollY() const { return scrollY; }
	void setScrollY(float newScrollY) { scrollY.setValue(newScrollY, nullptr); }

    double getSecondsPerPixel() const { return secondsPerPixel; }
    double getTrackHeight() const { return trackHeight; }

private:
    te::Edit& edit;
    juce::CachedValue<float> zoomX;
    juce::CachedValue<float> zoomY;
    juce::CachedValue<float> scrollX;
    juce::CachedValue<float> scrollY;

	// cached properties for performance
	double secondsPerPixel = fumeUI::timelineSecondsPerPixel;
	double trackHeight = fumeUI::timelineTrackHeight;
	
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ExtendedEdit)
};