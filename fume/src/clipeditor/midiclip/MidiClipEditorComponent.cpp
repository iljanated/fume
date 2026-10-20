#pragma once

#include "MidiClipEditorComponent.h"

MidiClipEditorComponent::MidiClipEditorComponent(te::MidiClip* clip, fumeUI::UIContext& context)
	: midiClip(*clip, &context.edit.getUndoManager()), midiEventsEditorComponent(clip, context, midiEventsViewport)
{
	addAndMakeVisible(pianoRollComponent);

	midiEventsEditorComponent.setSize(2024, 1600);
	midiEventsEditorComponent.addComponentListener(this);

	midiEventsViewport.setViewedComponent(&midiEventsEditorComponent, false);
	midiEventsViewport.setViewPosition(midiClip.scrollX.get(), midiClip.scrollY.get());
	addAndMakeVisible(midiEventsViewport);
}

MidiClipEditorComponent::~MidiClipEditorComponent()
{
}

void MidiClipEditorComponent::resized()
{
	auto localBounds = getLocalBounds();
	pianoRollComponent.setBounds(localBounds.removeFromLeft(50));
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
		midiClip.scrollX = -component.getX();
		midiClip.scrollY = -component.getY();
		pianoRollComponent.setScrollOffset(-component.getY());
	}
}
