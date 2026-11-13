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
	auto maxClipLength = uiContext.extendedEdit.getEdit().getMaximumLength().inSeconds();
	setSize(static_cast<int>(maxClipLength / uiContext.extendedMidiClip.getSecondsPerPixel()), 128 * uiContext.extendedMidiClip.getNoteHeight());

	updateGridSnapType();

    // default colours
    setColour(gridColourId, juce::Colours::lightgrey);
    setColour(blackNoteGridColourId, juce::Colours::lightgrey.darker(0.1));
    setColour(outsideGridColourId, juce::Colours::darkgrey.withAlpha(0.2f));
    setColour(gridlineColourId, juce::Colours::lightgrey.darker(0.2));
    setColour(whiteNoteColourId, juce::Colours::white);
    setColour(blackNoteColourId, juce::Colours::black);

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
    auto maxClipLength = uiContext.extendedEdit.getEdit().getMaximumLength().inSeconds();
    setSize(static_cast<int>(maxClipLength / uiContext.extendedMidiClip.getSecondsPerPixel()), 128 * uiContext.extendedMidiClip.getNoteHeight());
    cursorComponent.updateBounds();
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

    auto secondsPerPixel = extendedMidiClip.getSecondsPerPixel();
    auto noteHeight = extendedMidiClip.getNoteHeight();

    auto clipTimeRange = clip.getEditTimeRange();
    auto clipStart = clipTimeRange.getStart().inSeconds();
    auto clipEnd = clipTimeRange.getEnd().inSeconds();
    
    // boundaries
	
    auto minX = clipBounds.getX();
    auto maxX = clipBounds.getRight();
    auto minY = clipBounds.getY();
    auto maxY = clipBounds.getBottom();

	// draw background

	g.fillAll(findColour(gridColourId));

    // draw grid
    
	juce::Path gridPath;
	juce::Path beatGridPath;
    juce::Path barPath;

    auto snapType = te::TimecodeSnapType(te::TimecodeType::barsBeats, extendedMidiClip.getCursorSnapLevel());
	auto& tempoSequence = uiContext.extendedEdit.getTempoSequence();
    bool useTriplets = false;

    auto gridStartTime =  gridSnapType.roundTimeUp(te::TimePosition::fromSeconds(minX * secondsPerPixel), tempoSequence);
    auto gridEndTime = te::TimePosition::fromSeconds(maxX * secondsPerPixel);

    auto time = gridStartTime;
	auto timeOffset = te::TimeDuration::fromSeconds(0.0001);
	auto fractionalMargin = 0.001;
    while (time < gridEndTime)
    {
		auto x = time.inSeconds() / secondsPerPixel;
        gridPath.startNewSubPath(x, minY);
        gridPath.lineTo(x, maxY);

        auto barsAndBeats = tempoSequence.timeToBarsBeats(time);
		auto fractionalBeats = barsAndBeats.getFractionalBeats().inBeats();

		if (fractionalBeats < fractionalMargin || fractionalBeats > 1.0 - fractionalMargin)
		{
			beatGridPath.startNewSubPath(x, minY);
			beatGridPath.lineTo(x, maxY);

            if(barsAndBeats.getWholeBeats() == 0)
            {
                barPath.startNewSubPath(x, minY);
                barPath.lineTo(x, maxY);
            }
		}

        time = gridSnapType.roundTimeUp(time + timeOffset, tempoSequence);
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

    g.setColour(juce::Colours::lightgrey.darker(0.3));
    g.strokePath(beatGridPath, juce::PathStrokeType(1.0f));

    g.setColour(juce::Colours::lightgrey.darker(0.6));
    g.strokePath(barPath, juce::PathStrokeType(1.0f));


    // draw notes

    auto& sequence = clip.getSequence();

    g.setColour(juce::Colours::aqua);

	for (auto* note : sequence.getNotes())
    {
        if (note == nullptr) continue;

		auto noteStart = static_cast<float>(clipStart + tempoSequence.beatsToTime(note->getStartBeat()).inSeconds());
        auto noteEnd = static_cast<float>(clipStart + tempoSequence.beatsToTime(note->getEndBeat()).inSeconds());
        auto noteLength = noteEnd - noteStart;

        const float x = noteStart / secondsPerPixel;
        const float width = noteLength / secondsPerPixel;
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

	auto clipPixelStart = clipStart / secondsPerPixel;
	auto clipPixelEnd = clipEnd / secondsPerPixel;

    if (clipPixelStart > minX)
	{
		g.setColour(findColour(outsideGridColourId));
		g.fillRect(minX, minY, clipPixelStart - minX, maxY - minY);
	}

    if (clipPixelEnd < maxX)
    {
        g.setColour(findColour(outsideGridColourId));
        g.fillRect(clipPixelEnd, minY, maxX - clipPixelEnd, maxY - minY);
    }
}

void MidiEventsEditorComponent::findLassoItemsInArea(Array<te::MidiNote*>& itemsFound, const Rectangle<int>& area)
{
    auto& extendedMidiClip = uiContext.extendedMidiClip;
    auto secondsPerPixel = extendedMidiClip.getSecondsPerPixel();
    auto noteHeight = extendedMidiClip.getNoteHeight();

	auto& tempoSequence = uiContext.extendedEdit.getTempoSequence();

	auto clipStartBeat = extendedMidiClip.getClip().getStartBeat();

    auto xMin = (tempoSequence.timeToBeats(te::TimePosition::fromSeconds(area.getX() * secondsPerPixel)) - clipStartBeat).inBeats();
    auto xMax = (tempoSequence.timeToBeats(te::TimePosition::fromSeconds(area.getRight() * secondsPerPixel)) - clipStartBeat).inBeats();
    auto yMin = 127 - static_cast<int>(area.getBottom() / noteHeight);
    auto yMax = 127 - static_cast<int>(area.getY() / noteHeight);

    auto& clip = extendedMidiClip.getClip();
    auto& sequence = clip.getSequence();

    for (auto* note : sequence.getNotes())
    {
		auto startBeat = note->getStartBeat().inBeats();
		auto endBeat = note->getEndBeat().inBeats();
		auto isOnXMin = std::abs(startBeat - xMin) < 0.001;
        if ((isOnXMin || (startBeat < xMax && endBeat > xMin)) && note->getNoteNumber() >= yMin && note->getNoteNumber() <= yMax)
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

    if (property == FumeIDs::ZoomX || property == FumeIDs::ZoomY)
    {
		updateGridSnapType();
        asyncResizer.resizeAsync();
        repaint();
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

void MidiEventsEditorComponent::updateGridSnapType()
{
    auto& extendedMidiClip = uiContext.extendedMidiClip;
    auto secondsPerPixel = extendedMidiClip.getSecondsPerPixel();
    auto leftSeconds = te::TimePosition::fromSeconds(viewport.getViewPositionX() * secondsPerPixel);
    te::TempoSequence& tempoSequence = uiContext.extendedEdit.getEdit().tempoSequence;
    bool isTripletOverride = false;
    auto displayFormat = uiContext.extendedEdit.getEdit().getTimecodeFormat();
    gridSnapType = fumeHelpers::getBestSnapType(displayFormat, fumeUI::midiEditorMinimumPixelsPerSnap, tempoSequence.getTempoAt(leftSeconds), te::TimeDuration::fromSeconds(secondsPerPixel), isTripletOverride);

}