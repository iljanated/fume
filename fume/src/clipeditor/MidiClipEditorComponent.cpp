#pragma once

#include "MidiClipEditorComponent.h"

MidiClipEditorComponent::MidiClipEditorComponent(te::MidiClip* clip, fumeUI::UIContext& context)
	: midiClip(clip), uiContext(context)
{
	state = clip->state;
	state.addListener(this);
}

MidiClipEditorComponent::~MidiClipEditorComponent()
{
	state.removeListener(this);
	cancelPendingUpdate();
}

void MidiClipEditorComponent::resized()
{
	setSize((int)(midiClip->getLengthInBeats().inBeats() * FUME_MIDI_EDITOR_BEAT_WIDTH), 128 * FUME_MIDI_EDITOR_NOTE_HEIGHT);
}

juce::Point<double> MidiClipEditorComponent::getGridSize() const
{
    if (midiClip)
    {
        DBG("Grid size: " << midiClip->getLengthInBeats().inBeats() << ", 128.0");
        return { midiClip->getLengthInBeats().inBeats(), 128.0 };
    }
    return { 0.0, 128.0 };
}

void MidiClipEditorComponent::paint(juce::Graphics& g)
{
    auto clipBounds = g.getClipBounds();
	if (midiClip == nullptr || clipBounds.isEmpty())
	{
		return;
	}
    float zoomX = midiClip->state.getProperty(FumeIDs::ZoomX, 1.0f);
    float zoomY = midiClip->state.getProperty(FumeIDs::ZoomY, 1.0f);
    
	auto unitSize = getUnitSize();

    // Draw the background frame
    g.fillAll(juce::Colours::lightgrey);
    g.setColour(juce::Colours::grey);
    g.drawRect(getLocalBounds(), 1);

	fumeUI::drawGrid(g, clipBounds, *this, juce::Colours::lightgrey.darker(0.8f), juce::Colours::lightgrey.darker(0.2f));
    DBG("GridSize: " << getGridSize().toString());
    // Fetch clip parameters safely on the Message Thread
    const auto clipLength = midiClip->getLengthInBeats().inBeats();
    if (clipLength <= 0.0)
    {
        return;
    }

    g.setColour(juce::Colours::aqua);

    // Render individual MIDI notes safely using Tracktion's high-level MIDI sequence wrapper
    auto& sequence = midiClip->getSequence();

	for (auto* note : sequence.getNotes())
    {
        if (note == nullptr) continue;

        // Map time ranges to component X dimensions
        const float noteStart = (float)note->getStartBeat().inBeats();
        const float noteLength = (float)note->getLengthBeats().inBeats();

        const float x = noteStart * unitSize.x;
        const float width = noteLength * unitSize.x;
        const float y = (127.0f - (float)note->getNoteNumber()) * unitSize.y;
        const float height = unitSize.y;

		if (x >= clipBounds.getX() + clipBounds.getWidth() || x + width <= clipBounds.getX() || y >= clipBounds.getY() + clipBounds.getHeight() || y + height <= clipBounds.getY())
		{
			continue; // Skip rendering notes outside the visible area
		}
        g.fillRect(x, y, width, height);
    }
}

te::Clip* MidiClipEditorComponent::getClip()
{
    return midiClip;
}

void MidiClipEditorComponent::valueTreePropertyChanged(juce::ValueTree& treeWhosePropertyHasChanged, const juce::Identifier& property)
{
    // Opt-out from updating on non-visual UI metadata tags to minimize performance overhead
    if (property == juce::Identifier("color") || property == juce::Identifier("name"))
    {
        return;
    }

    triggerAsyncUpdate();
}

void MidiClipEditorComponent::valueTreeChildAdded(juce::ValueTree& parentTree, juce::ValueTree& childWhichHasBeenAdded)
{
    triggerAsyncUpdate();
}

void MidiClipEditorComponent::valueTreeChildRemoved(juce::ValueTree& parentTree, juce::ValueTree& childWhichHasBeenRemoved, int index)
{
    triggerAsyncUpdate();
}

void MidiClipEditorComponent::valueTreeChildOrderChanged(juce::ValueTree& parentTree, int oldIndex, int newIndex)
{
    triggerAsyncUpdate();
}

void MidiClipEditorComponent::valueTreeParentChanged(juce::ValueTree& treeWhoseParentHasChanged)
{
}

void MidiClipEditorComponent::handleAsyncUpdate()
{
    resized();
}