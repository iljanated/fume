#pragma once

#include "Utilities.h"

namespace fumeUI
{
    const double timelineBeatWidth = 20;
    const double timelineSecondsPerPixel = 0.1;
    const int timelineMinimumPixelsPerSnap = 12;
    const int timelineHeaderMinimumPixelsPerSnap = 48;
    const int timelineTrackHeight = 30;
    const int timelineTrackHeadWidth = 100;
    const int timelineTimelineHeight = 20;
    const double midiEditorSecondsPerPixel = 0.002;
    const int midiEditorMinimumPixelsPerSnap = 24;
    const int midiEditorHeaderMinimumPixelsPerSnap = 96;
    const int midiEditorNoteHeight = 10;
    const int midiEditorTimelineHeight = 20;
    const int midiEditorPianoRollWidth = 50;
    const int scrollPadding = 10;
    const int inputRefreshHz = 60;
    const int inputRepeatDelayFrames = 30;
    const int inputRepeatFrames = 2;
	const float zoomStepSize = 0.1f;

    
    void makeComponentVisibleInViewport(juce::Viewport& viewport, juce::Component& component, int padding)
    {
		auto targetRect = component.getBounds().expanded(padding);

        int viewX = viewport.getViewPositionX();
        int viewY = viewport.getViewPositionY();
        int viewW = viewport.getViewWidth();
        int viewH = viewport.getViewHeight();

        int newX = viewX;
        int newY = viewY;

        // Check horizontal bounds
        if (targetRect.getX() < viewX)
        {
            newX = targetRect.getX();
        }
        else if (targetRect.getRight() > viewX + viewW)
        {
            newX = targetRect.getRight() - viewW;
        }

        // Check vertical bounds
        if (targetRect.getY() < viewY)
        {
            newY = targetRect.getY();
        }
        else if (targetRect.getBottom() > viewY + viewH)
        {
            newY = targetRect.getBottom() - viewH;
        }

        // Apply the new position if it changed
        if (newX != viewX || newY != viewY)
        {
            viewport.setViewPosition(newX, newY);
        }
    }

    juce::String beatToString(te::core::tempo::BarsAndBeats barsAndBeats, bool useTriplets)
    {
        int bar = barsAndBeats.bars + 1;
        int beat = barsAndBeats.getWholeBeats() + 1;

        auto beatFraction = barsAndBeats.getFractionalBeats().inBeats();

        auto subdivisions = useTriplets ? 3 : 4;
        auto fractionIndex = static_cast<int>(std::floor((beatFraction * subdivisions) + 1.0e-10));
        fractionIndex = juce::jlimit(0, subdivisions - 1, fractionIndex);

        auto fractionInBeat = fractionIndex + 1;

        return juce::String::formatted("%d.%d.%d", bar, beat, fractionInBeat);
    }
}