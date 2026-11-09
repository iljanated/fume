#pragma once

#include "MidiClipEditorComponent.h"

MidiClipEditorComponent::MidiClipEditorComponent(te::MidiClip* clip, fumeUI::UIContext& context)
	: midiClip(clip), uiContext(context, midiClip), midiEventsEditorComponent(uiContext, midiEventsViewport), pianoRollComponent(uiContext), midiClipTimelineComponent(uiContext)
{
	setWantsKeyboardFocus(true);
	addAndMakeVisible(pianoRollComponent);
	addAndMakeVisible(midiClipTimelineComponent);
	
	midiEventsEditorComponent.setSize(2024, 1600);
	midiEventsEditorComponent.addComponentListener(this);

	midiEventsViewport.setViewedComponent(&midiEventsEditorComponent, false);
	midiEventsViewport.setViewPosition(midiClip.getScrollX(), midiClip.getScrollY());
	addAndMakeVisible(midiEventsViewport);
}

MidiClipEditorComponent::~MidiClipEditorComponent()
{
}

void MidiClipEditorComponent::resized()
{
	auto localBounds = getLocalBounds();
	auto timelineBounds = localBounds.removeFromTop(fumeUI::midiEditorTimelineHeight);
	timelineBounds.removeFromLeft(fumeUI::midiEditorPianoRollWidth);
	midiClipTimelineComponent.setBounds(timelineBounds);
	pianoRollComponent.setBounds(localBounds.removeFromLeft(fumeUI::midiEditorPianoRollWidth));
    midiEventsViewport.setBounds(localBounds);
}

void MidiClipEditorComponent::focusGained(FocusChangeType cause) {
	if (midiEventsEditorComponent.getParentComponent() != nullptr && midiEventsEditorComponent.isShowing())
	{
		midiEventsEditorComponent.grabKeyboardFocus();
	}
}

te::Clip* MidiClipEditorComponent::getClip()
{
    return &midiClip.getClip();
}

void MidiClipEditorComponent::componentMovedOrResized(juce::Component& component, bool wasMoved, bool wasResized)
{
	if (&component == &midiEventsEditorComponent)
	{
		midiClip.setScrollX(-component.getX());
		midiClip.setScrollY(-component.getY());
		pianoRollComponent.setScrollOffset(-component.getY());
		midiClipTimelineComponent.setOffset(-component.getX());
	}
}
