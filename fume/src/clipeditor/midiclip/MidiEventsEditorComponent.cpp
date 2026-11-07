#pragma once

#include "MidiEventsEditorComponent.h"
#include "MidiClipMoveToolState.h"
#include "MidiClipSelectToolState.h"
#include "MidiClipDeleteToolState.h"
#include "MidiClipWriteToolState.h"

MidiEventsEditorComponent::MidiEventsEditorComponent(fumeUI::UIMidiClipContext& uiContext, juce::Viewport& v)
	: state(uiContext.extendedMidiClip.getClip().state), uiContext(uiContext), viewport(v)
{
    setWantsKeyboardFocus(true);

    state.addListener(this);
	selectedEvents->addChangeListener(this);

	addAndMakeVisible(cursorComponent);
	cursorComponent.setAlwaysOnTop(true);

    //toolStateManager.addToolState(std::make_unique<MidiClipMoveToolState>(this));
    toolStateManager.addToolState(std::make_unique<MidiClipSelectToolState>(this));
    //toolStateManager.addToolState(std::make_unique<MidiClipDeleteToolState>(this));
    //toolStateManager.addToolState(std::make_unique<MidiClipWriteToolState>(this));
    
    toolStateManager.activateToolStateByIdentifier(FumeIDs::MidiClipSelectToolState);
}

MidiEventsEditorComponent::~MidiEventsEditorComponent()
{
    selectedEvents->removeChangeListener(this);
    state.removeListener(this);
}

void MidiEventsEditorComponent::resized()
{
	auto& clip = uiContext.extendedMidiClip.getClip();
    setSize((int)(clip.getLengthInBeats().inBeats() / uiContext.extendedMidiClip.getBeatsPerPixel()), 128 * uiContext.extendedMidiClip.getNoteHeight());

    if (cursorComponent.isVisible())
    {
        cursorComponent.updateBounds();
    }
}

void MidiEventsEditorComponent::changeListenerCallback(juce::ChangeBroadcaster* source)
{
	if (source == selectedEvents.get())
    {
	    repaint();
    }
}

void MidiEventsEditorComponent::paint(juce::Graphics& g)
{
	auto& extendedMidiClip = uiContext.extendedMidiClip;
	auto& clip = extendedMidiClip.getClip();

    auto clipBounds = g.getClipBounds();
    if (clipBounds.isEmpty())
    {
        return;
    }
    float zoomX = clip.state.getProperty(FumeIDs::ZoomX, 1.0f);
    float zoomY = clip.state.getProperty(FumeIDs::ZoomY, 1.0f);

    // Draw the background frame
    g.fillAll(juce::Colours::lightgrey);
    g.setColour(juce::Colours::grey);
    g.drawRect(getLocalBounds(), 1);

    const auto clipLength = clip.getLengthInBeats().inBeats();
    if (clipLength <= 0.0)
    {
        return;
    }

    // draw grid
    /*
	auto beatsPerPixel = extendedMidiClip.getBeatsPerPixel();

    auto leftSeconds = te::TimePosition::fromSeconds(clipBounds.getX() * secondsPerPixel);
    te::TempoSequence& tempoSequence = extendedEdit.getTempoSequence();

    auto trackHeight = extendedEdit.getTrackHeight();

    g.fillAll(juce::Colours::lightgrey);

    juce::Path gridPath;

    auto minX = clipBounds.getX();
    auto maxX = clipBounds.getRight();
    auto minY = clipBounds.getY();
    auto maxY = clipBounds.getBottom();

    auto startTime = snapType.roundTimeUp(leftSeconds, tempoSequence);
    auto endTime = te::TimePosition::fromSeconds(maxX * secondsPerPixel);

    auto time = startTime;
    while (time < endTime)
    {
        auto x = time.inSeconds() / secondsPerPixel;
        gridPath.startNewSubPath(x, minY);
        gridPath.lineTo(x, maxY);

        time = snapType.roundTimeUp(time + te::TimeDuration::fromSeconds(0.0001), tempoSequence);
    }

    // horizontal lines

    for (auto y = minY + trackHeight - std::fmod(minY, trackHeight); y < maxY; y += trackHeight)
    {
        gridPath.startNewSubPath(minX, y);
        gridPath.lineTo(maxX, y);
    }

    g.setColour(juce::Colours::lightgrey.darker(0.2));

    g.strokePath(gridPath, juce::PathStrokeType(1.0f));

    */

    // draw notes

    auto& sequence = clip.getSequence();

    g.setColour(juce::Colours::aqua);

	auto beatsPerPixel = uiContext.extendedMidiClip.getBeatsPerPixel();
	auto noteHeight = uiContext.extendedMidiClip.getNoteHeight();

    for (auto* note : sequence.getNotes())
    {
        if (note == nullptr) continue;

        // Map time ranges to component X dimensions
        const float noteStart = (float)note->getStartBeat().inBeats();
        const float noteLength = (float)note->getLengthBeats().inBeats();

        const float x = noteStart / beatsPerPixel;
        const float width = noteLength / beatsPerPixel;
        const float y = (127.0f - (float)note->getNoteNumber()) * noteHeight;
        
        if (x >= clipBounds.getX() + clipBounds.getWidth() || x + width <= clipBounds.getX() || y >= clipBounds.getY() + clipBounds.getHeight() || y + noteHeight <= clipBounds.getY())
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
        g.fillRect(x, y, width, noteHeight);
    }

    // draw focusbox

    if (hasKeyboardFocus(true))
    {
        g.setColour(juce::Colours::red);
    }
    else
    {
        g.setColour(juce::Colours::white);
    }
    g.drawRect(viewport.getViewArea());

}

void MidiEventsEditorComponent::childBoundsChanged(juce::Component* child)
{
    if (child == &cursorComponent)
    {
        fumeUI::makeComponentVisibleInViewport(viewport, cursorComponent, fumeUI::scrollPadding);
    }
}

void MidiEventsEditorComponent::valueTreePropertyChanged(juce::ValueTree& treeWhosePropertyHasChanged, const juce::Identifier& property)
{
    // Opt-out from updating on non-visual UI metadata tags to minimize performance overhead
    if (property == juce::Identifier("color") || property == juce::Identifier("name"))
    {
        return;
    }

    asyncResizer.resizeAsync();
}

void MidiEventsEditorComponent::valueTreeChildAdded(juce::ValueTree& parentTree, juce::ValueTree& childWhichHasBeenAdded)
{
    asyncResizer.resizeAsync();
}

void MidiEventsEditorComponent::valueTreeChildRemoved(juce::ValueTree& parentTree, juce::ValueTree& childWhichHasBeenRemoved, int index)
{
    asyncResizer.resizeAsync();
}

void MidiEventsEditorComponent::valueTreeChildOrderChanged(juce::ValueTree& parentTree, int oldIndex, int newIndex)
{
    asyncResizer.resizeAsync();
}

void MidiEventsEditorComponent::valueTreeParentChanged(juce::ValueTree& treeWhoseParentHasChanged)
{
    asyncResizer.resizeAsync();
}