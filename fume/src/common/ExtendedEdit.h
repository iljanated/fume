#pragma once

#include <JuceHeader.h>
#include "Utilities.h"

class ExtendedEdit
{
public:
    ExtendedEdit(te::Edit& e, UndoManager* um) : edit(e)
    {
        zoomX.referTo(edit.state, FumeIDs::ZoomX, nullptr, 1.0f);
        zoomY.referTo(edit.state, FumeIDs::ZoomY, nullptr, 1.0f);
        scrollX.referTo(edit.state, FumeIDs::ScrollX, nullptr, 0);
        scrollY.referTo(edit.state, FumeIDs::ScrollY, nullptr, 0);
	}

    te::Edit& getEdit() const { return edit; }

    juce::CachedValue<float> zoomX;
    juce::CachedValue<float> zoomY;
    juce::CachedValue<float> scrollX;
    juce::CachedValue<float> scrollY;

private:
    te::Edit& edit;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ExtendedEdit)
};