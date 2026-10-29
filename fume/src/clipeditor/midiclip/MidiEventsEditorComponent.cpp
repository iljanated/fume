#pragma once

#include "MidiEventsEditorComponent.h"
#include "MidiClipMoveToolState.h"
#include "MidiClipSelectToolState.h"
#include "MidiClipDeselectToolState.h"
#include "MidiClipDeleteToolState.h"
#include "MidiClipWriteToolState.h"

MidiEventsEditorComponent::MidiEventsEditorComponent(te::MidiClip* clip, fumeUI::UIContext& uiContext, juce::Viewport& v)
	: GridEditor<MidiEventsEditorComponent>(uiContext, v, FumeIDs::MidiClipMoveToolState), midiClip(*clip, &uiContext.extendedEdit.getUndoManager()), state(clip->state)
{
    state.addListener(this);
	selectedEvents->addChangeListener(this);

    toolStateManager.addToolState(std::make_unique<MidiClipMoveToolState>(this));
    toolStateManager.addToolState(std::make_unique<MidiClipSelectToolState>(this));
    toolStateManager.addToolState(std::make_unique<MidiClipDeselectToolState>(this));
    toolStateManager.addToolState(std::make_unique<MidiClipDeleteToolState>(this));
    toolStateManager.addToolState(std::make_unique<MidiClipWriteToolState>(this));
    
    //uiContext.edit.state.setProperty(FumeIDs::ZoomX, 1.0f, &uiContext.edit.getUndoManager());
    //uiContext.edit.state.setProperty(FumeIDs::ZoomY, 1.0f, &uiContext.edit.getUndoManager());
    //uiContext.edit.state.setProperty(FumeIDs::Quantisation, 1.0f, &uiContext.edit.getUndoManager());

	midiClip.getClip().state.addListener(this);

    toolStateManager.activateToolStateByIdentifier(FumeIDs::MidiClipMoveToolState);
}

MidiEventsEditorComponent::~MidiEventsEditorComponent()
{
    state.removeListener(this);
    cancelPendingUpdate();
}

void MidiEventsEditorComponent::resized()
{
    setSize((int)(midiClip.getClip().getLengthInBeats().inBeats() * fumeUI::midiEditorBeatWidth), 128 * fumeUI::midiEditorNoteHeight);
}

void MidiEventsEditorComponent::selectItemsInBox(const juce::Rectangle<double>& selectionBox)
{

    auto xMin = selectionBox.getX();
    auto xMax = selectionBox.getRight();
    auto yMin = 127 - selectionBox.getBottom();
    auto yMax = 127 - selectionBox.getY();

    for (auto* note : selectedEvents->getSelectedNotes())
    {
        if (note->getBeatPosition().inBeats() > xMax || note->getEndBeat().inBeats() < xMin  || note->getNoteNumber() < yMin || note->getNoteNumber() > yMax)
        {
			selectedEvents->removeSelectedEvent(note);
        }
    }

    for (auto* note : midiClip.getClip().getSequence().getNotes())
    {
        if (note->getBeatPosition().inBeats() <= xMax && note->getEndBeat().inBeats() >= xMin && note->getNoteNumber() >= yMin && note->getNoteNumber() <= yMax)
        {
            selectedEvents->addSelectedEvent(note, true);
        }
    }
}

void MidiEventsEditorComponent::changeListenerCallback(juce::ChangeBroadcaster* source)
{
	if (source == selectedEvents.get())
    {
	    repaint();
    }
}

juce::Point<double> MidiEventsEditorComponent::getGridSize() const
{
    return { midiClip.getClip().getLengthInBeats().inBeats(), 128.0 };
}

te::SelectedMidiEvents& MidiEventsEditorComponent::clearSelectedEvents() {
    selectedEvents.get()->removeAllChangeListeners();
    selectedEvents = std::make_unique<te::SelectedMidiEvents>(midiClip.getClip());
    selectedEvents.get()->addChangeListener(this);
    repaint();
    return *selectedEvents;
}

void MidiEventsEditorComponent::paint(juce::Graphics& g)
{
    auto clipBounds = g.getClipBounds();
    if (clipBounds.isEmpty())
    {
        return;
    }
    float zoomX = midiClip.getClip().state.getProperty(FumeIDs::ZoomX, 1.0f);
    float zoomY = midiClip.getClip().state.getProperty(FumeIDs::ZoomY, 1.0f);

    auto unitSize = getUnitSize();

    // Draw the background frame
    g.fillAll(juce::Colours::lightgrey);
    g.setColour(juce::Colours::grey);
    g.drawRect(getLocalBounds(), 1);

    fumeUI::drawGrid(g, clipBounds, *this, juce::Colours::lightgrey.darker(0.8f), juce::Colours::lightgrey.darker(0.2f), true);
    // Fetch clip parameters safely on the Message Thread
    const auto clipLength = midiClip.getClip().getLengthInBeats().inBeats();
    if (clipLength <= 0.0)
    {
        return;
    }

    g.setColour(juce::Colours::aqua);

    // Render individual MIDI notes safely using Tracktion's high-level MIDI sequence wrapper
    auto& sequence = midiClip.getClip().getSequence();

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
		if (selectedEvents->getSelectedNotes().contains(note))
		{
			g.setColour(juce::Colours::orange);
		}
		else
		{
			g.setColour(juce::Colours::aqua);
		}
        g.fillRect(x, y, width, height);
    }
}

void MidiEventsEditorComponent::valueTreePropertyChanged(juce::ValueTree& treeWhosePropertyHasChanged, const juce::Identifier& property)
{
    // Opt-out from updating on non-visual UI metadata tags to minimize performance overhead
    if (property == juce::Identifier("color") || property == juce::Identifier("name"))
    {
        return;
    }

    triggerAsyncUpdate();
}

void MidiEventsEditorComponent::valueTreeChildAdded(juce::ValueTree& parentTree, juce::ValueTree& childWhichHasBeenAdded)
{
    triggerAsyncUpdate();
}

void MidiEventsEditorComponent::valueTreeChildRemoved(juce::ValueTree& parentTree, juce::ValueTree& childWhichHasBeenRemoved, int index)
{
    triggerAsyncUpdate();
}

void MidiEventsEditorComponent::valueTreeChildOrderChanged(juce::ValueTree& parentTree, int oldIndex, int newIndex)
{
    triggerAsyncUpdate();
}

void MidiEventsEditorComponent::valueTreeParentChanged(juce::ValueTree& treeWhoseParentHasChanged)
{
    triggerAsyncUpdate();
}

void MidiEventsEditorComponent::handleAsyncUpdate()
{
    resized();
    repaint();
}