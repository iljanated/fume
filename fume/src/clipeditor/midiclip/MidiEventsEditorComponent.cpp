#pragma once

#include "MidiEventsEditorComponent.h"
#include "MidiClipMoveToolState.h"
#include "MidiClipSelectToolState.h"
#include "MidiClipDeleteToolState.h"
#include "MidiClipWriteToolState.h"
#include "MidiClipZoomToolState.h"

MidiEventsEditorComponent::MidiEventsEditorComponent(fumeUI::UIMidiClipContext& uiContext, juce::Viewport& v)
	: state(uiContext.extendedMidiClip.getClip().state), uiContext(uiContext), viewport(v)
{
	setWantsKeyboardFocus(true);

	selectedEvents = std::make_unique<te::SelectedMidiEvents>(uiContext.extendedMidiClip.getClip());

	state.addListener(this);
	selectedEvents->addChangeListener(this);

	addAndMakeVisible(cursorComponent);
	cursorComponent.setAlwaysOnTop(true);

    toolStateManager.addToolState(std::make_unique<MidiClipMoveToolState>(this));
    toolStateManager.addToolState(std::make_unique<MidiClipSelectToolState>(this));
    toolStateManager.addToolState(std::make_unique<MidiClipDeleteToolState>(this));
    toolStateManager.addToolState(std::make_unique<MidiClipWriteToolState>(this));
    toolStateManager.addToolState(std::make_unique<MidiClipZoomToolState>(this));

    toolStateManager.activateToolStateByIdentifier(FumeIDs::MidiClipSelectToolState);
}

MidiEventsEditorComponent::~MidiEventsEditorComponent()
{
    if (selectedEvents)
    {
        selectedEvents->removeChangeListener(this);
    }

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
    
    const auto clipLength = clip.getLengthInBeats().inBeats();
    if (clipLength <= 0.0)
    {
        return;
    }

    // draw grid
    
	auto beatsPerPixel = extendedMidiClip.getBeatsPerPixel();

	auto noteHeight = extendedMidiClip.getNoteHeight();

    g.fillAll(juce::Colours::lightgrey);

    juce::Path gridPath;

    auto minX = clipBounds.getX();
    auto maxX = clipBounds.getRight();
    auto minY = clipBounds.getY();
    auto maxY = clipBounds.getBottom();

	auto& quantisationType = extendedMidiClip.getCursorQuantisation();

    auto gridStartBeat = quantisationType.roundBeatUp(te::BeatPosition::fromBeats(minX) * beatsPerPixel);
    auto gridEndBeat = te::BeatPosition::fromBeats(maxX * beatsPerPixel);

    auto beat = gridStartBeat;
	
    while (beat < gridEndBeat)
    {
		auto x = beat.inBeats() / beatsPerPixel;
        gridPath.startNewSubPath(x, minY);
        gridPath.lineTo(x, maxY);

        beat = quantisationType.roundBeatUp(beat + te::BeatDuration::fromBeats(0.0001));
    }

    // black notes

    g.setColour(juce::Colours::lightgrey.darker(0.1));

    for (auto y = minY + noteHeight - std::fmod(minY, noteHeight); y < maxY; y += noteHeight)
    {
        auto noteNumber = (127 - static_cast<int>(y / noteHeight)) % 12;
        if (noteNumber == 1 || noteNumber == 3 || noteNumber == 6 || noteNumber == 8 || noteNumber == 10)
        {
            g.fillRect(minX, static_cast<int>(y), maxX - minX, noteHeight);
        }
    }

    // horizontal lines

    for (auto y = minY + noteHeight - std::fmod(minY, noteHeight); y < maxY; y += noteHeight)
    {
        gridPath.startNewSubPath(minX, static_cast<int>(y));
        gridPath.lineTo(maxX, static_cast<int>(y));
    }

    g.setColour(juce::Colours::lightgrey.darker(0.2));

    g.strokePath(gridPath, juce::PathStrokeType(1.0f));

    

    // draw notes

    auto& sequence = clip.getSequence();

    g.setColour(juce::Colours::aqua);

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
        g.fillRect(static_cast<int>(x), static_cast<int>(y), static_cast<int>(width), noteHeight);
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

void MidiEventsEditorComponent::findLassoItemsInArea(Array<te::MidiNote*>& itemsFound, const Rectangle<int>& area)
{
    auto& extendedMidiClip = uiContext.extendedMidiClip;
    auto beatsPerPixel = extendedMidiClip.getBeatsPerPixel();
    auto noteHeight = extendedMidiClip.getNoteHeight();

    auto xMin = area.getX() * beatsPerPixel;
    auto xMax = area.getRight() * beatsPerPixel;
    auto yMin = 127 - static_cast<int>(area.getBottom() / noteHeight);
    auto yMax = 127 - static_cast<int>(area.getY() / noteHeight);

    auto& clip = extendedMidiClip.getClip();
    auto& sequence = clip.getSequence();

    for (auto* note : sequence.getNotes())
    {
		if (note->getStartBeat().inBeats() < xMax && note->getEndBeat().inBeats() > xMin && note->getNoteNumber() >= yMin && note->getNoteNumber() <= yMax)
		{
			itemsFound.add(note);
		}
    }
}

void MidiEventsEditorComponent::lassoSelectionChanged(SelectedItemSet<te::MidiNote*>& newSelection)
{
    clearSelectedEvents();

    auto newNotes = newSelection.getItemArray();

	for (auto* note : newNotes)
	{
		selectedEvents->addSelectedEvent(note, true);
	}

	uiContext.selectionManager.selectOnly(selectedEvents.get());
}

te::SelectedMidiEvents& MidiEventsEditorComponent::clearSelectedEvents()
{
	if (selectedEvents)
	{
		selectedEvents->removeAllChangeListeners();
	}
    
    selectedEvents = std::make_unique<te::SelectedMidiEvents>(uiContext.extendedMidiClip.getClip());
    selectedEvents.get()->addChangeListener(this);
    return *selectedEvents;
}

void MidiEventsEditorComponent::valueTreePropertyChanged(juce::ValueTree& treeWhosePropertyHasChanged, const juce::Identifier& property)
{
    // Opt-out from updating on non-visual UI metadata tags to minimize performance overhead
    if (property == juce::Identifier("color") || property == juce::Identifier("name"))
    {
        return;
    }

	if (property == FumeIDs::CursorX || property == FumeIDs::CursorY)
	{
        juce::MessageManager::callAsync([this]() {
            fumeUI::makeComponentVisibleInViewport(viewport, cursorComponent, fumeUI::scrollPadding);
            });
        return;
	}

    repaint();
}

void MidiEventsEditorComponent::valueTreeChildAdded(juce::ValueTree& parentTree, juce::ValueTree& childWhichHasBeenAdded)
{
    repaint();
}

void MidiEventsEditorComponent::valueTreeChildRemoved(juce::ValueTree& parentTree, juce::ValueTree& childWhichHasBeenRemoved, int index)
{
    repaint();
}

void MidiEventsEditorComponent::valueTreeChildOrderChanged(juce::ValueTree& parentTree, int oldIndex, int newIndex)
{
    repaint();
}

void MidiEventsEditorComponent::valueTreeParentChanged(juce::ValueTree& treeWhoseParentHasChanged)
{
    asyncResizer.resizeAsync();
}