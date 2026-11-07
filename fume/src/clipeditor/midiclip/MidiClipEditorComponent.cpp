#pragma once

#include "MidiClipEditorComponent.h"

MidiClipEditorComponent::MidiClipEditorComponent(te::MidiClip* clip, fumeUI::UIContext& context)
	: midiClip(clip), uiContext(context, midiClip), midiEventsEditorComponent(uiContext, midiEventsViewport), pianoRollComponent(uiContext)
{
	setWantsKeyboardFocus(true);
	addAndMakeVisible(pianoRollComponent);
	
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
	pianoRollComponent.setBounds(localBounds.removeFromLeft(fumeUI::midiEditorPianoRollWidth));
    midiEventsViewport.setBounds(localBounds);
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
	}
}
