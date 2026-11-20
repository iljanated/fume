#include "TrackListComponent.h"
#include "TrackListWriteToolState.h"
#include "TrackListDeleteToolState.h"
#include "TrackListSelectToolState.h"
#include "TrackListZoomToolState.h"

namespace te = tracktion;

TrackListComponent::TrackListComponent(fumeUI::UIContext& context, juce::Viewport& v)
	: viewport(v), uiContext(context), te::ValueTreeObjectList<Helpers::AsyncValueTreeItem<TrackComponent>>(context.extendedEdit.getState())
{
	tempoSequenceState = uiContext.extendedEdit.getTempoSequence().getState();
	tempoSequenceState.addListener(this);

	addAndMakeVisible(cursorComponent);
	cursorComponent.setAlwaysOnTop(true);
	
	setWantsKeyboardFocus(true);

	uiContext.selectionManager.addChangeListener(this);
	
	toolStateManager.addToolState(std::make_unique<TrackListSelectToolState>(this));
	toolStateManager.addToolState(std::make_unique<TrackListWriteToolState>(this));
	toolStateManager.addToolState(std::make_unique<TrackListDeleteToolState>(this));
	toolStateManager.addToolState(std::make_unique<TrackListZoomToolState>(this));

	rebuildObjects();

	toolStateManager.activateToolStateByIdentifier(FumeIDs::TrackListSelectToolState);

	updateSnapType();

	startTimerHz(30);
}

TrackListComponent::~TrackListComponent()
{
	tempoSequenceState.removeListener(this);
	uiContext.selectionManager.removeChangeListener(this);
	freeObjects();
}

te::AudioTrack* TrackListComponent::getTrackAt(int index) const
{
	if (index < 0 || index >= objects.size())
		return nullptr;
	if (auto objectWrapper = objects[index])
	{
		if (auto object = objectWrapper->getObject())
		{
			return &object->getTrack();
		}
	}
	return nullptr;
}

void TrackListComponent::paint(juce::Graphics& g)
{
	auto clipBounds = g.getClipBounds();

	auto& extendedEdit = uiContext.extendedEdit;

	auto secondsPerPixel = extendedEdit.getSecondsPerPixel();

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

	// draw transport position


	g.setColour(juce::Colours::darkgrey);
	auto transportPositionPixels = transportPosition.inSeconds() / secondsPerPixel;

	g.drawLine(transportPositionPixels, 0.0f, transportPositionPixels, (float)getHeight(), 1.0f);

	// draw border around viewport

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

void TrackListComponent::resized()
{
	auto localBounds = getLocalBounds();
	auto& extendedEdit = uiContext.extendedEdit;

	auto trackHeight = extendedEdit.getTrackHeight();

	for (auto objectWrapper : objects)
	{
		if (auto object = objectWrapper->getObject())
		{
			object->setBounds(localBounds.removeFromTop(trackHeight));
		}
	}

	if (cursorComponent.isVisible())
	{
		cursorComponent.updateBounds();
	}
}

void TrackListComponent::focusGained(FocusChangeType cause)
{
	cursorComponent.setVisible(true);
	repaint();
}

void TrackListComponent::focusLost(FocusChangeType cause)
{
	cursorComponent.setVisible(true);
	repaint();
}

bool TrackListComponent::isSuitableType(const juce::ValueTree& v) const
{
	return v.hasType(te::IDs::TRACK);
}

Helpers::AsyncValueTreeItem<TrackComponent>* TrackListComponent::createNewObject(const juce::ValueTree& v)
{
	return new Helpers::AsyncValueTreeItem<TrackComponent>(v,
		[this](auto state)
		{
			auto t = Helpers::findObjectForState(te::getAudioTracks(uiContext.extendedEdit.getEdit()), state);
			assert(t);
			auto tc = std::make_unique<TrackComponent>(*t, uiContext);
			addAndMakeVisible(*tc);
			asyncResizer.resizeAsync();
			return tc;
		});
}

void TrackListComponent::deleteObject(Helpers::AsyncValueTreeItem<TrackComponent>* obj)
{
	delete obj;
}

void TrackListComponent::newObjectAdded(Helpers::AsyncValueTreeItem<TrackComponent>*)
{
	updateBounds();
	asyncResizer.resizeAsync();
}

void TrackListComponent::objectRemoved(Helpers::AsyncValueTreeItem<TrackComponent>*)
{
	updateBounds();
	asyncResizer.resizeAsync();
}

void TrackListComponent::objectOrderChanged()
{
	asyncResizer.resizeAsync();
}

void TrackListComponent::valueTreePropertyChanged(juce::ValueTree& treeWhosePropertyHasChanged, const juce::Identifier& property)
{
	if (property == FumeIDs::ZoomX)
	{
		updateSnapType();
		updateBounds();
		asyncResizer.resizeAsync();
	}
	else if (property == FumeIDs::ZoomY)
	{
		updateBounds();
		asyncResizer.resizeAsync();
	}

	if (property == FumeIDs::CursorX || property == FumeIDs::CursorY)
	{
		juce::MessageManager::callAsync([this]() {
			fumeUI::makeComponentVisibleInViewport(viewport, cursorComponent, fumeUI::scrollPadding);
			});
		return;
	}

	if (property == te::IDs::bpm)
	{
		repaint();
	}
}

void TrackListComponent::timerCallback()
{
	updateCurrentPositionMarker();
}

void TrackListComponent::updateCurrentPositionMarker()
{
	auto secondsPerPixel = uiContext.extendedEdit.getSecondsPerPixel();
	auto oldOffset = static_cast<float>(transportPosition.inSeconds() / secondsPerPixel);
	transportPosition = uiContext.extendedEdit.getEdit().getTransport().getPosition();
	auto offset = static_cast<float>(transportPosition.inSeconds() / secondsPerPixel);
	
	auto min = std::min(oldOffset, offset) - 2;
	auto width = std::abs(oldOffset - offset) + 4;
	repaint(juce::Rectangle<int>(min, 0, width, getHeight()));
}

void TrackListComponent::changeListenerCallback(juce::ChangeBroadcaster* source)
{
	if (source == &uiContext.selectionManager)
	{
		repaint();
	}
}

void TrackListComponent::findLassoItemsInArea(Array<te::Clip*>& itemsFound, const Rectangle<int>& area)
{
	auto& extendedEdit = uiContext.extendedEdit;
	auto& tempoSequence = extendedEdit.getTempoSequence();
	auto secondsPerPixel = extendedEdit.getSecondsPerPixel();
	auto trackHeight = extendedEdit.getTrackHeight();

	auto xMin = area.getX() * secondsPerPixel;
	auto xMax = area.getRight() * secondsPerPixel;
	auto yMin = static_cast<int>(area.getY() / trackHeight);
	auto yMax = static_cast<int>(area.getBottom() / trackHeight);

	DBG("Trackheight: " << trackHeight << "area: " << area.getY());

	for (int i = yMin; i <= yMax; ++i)
	{
		DBG("Checking track index: " << i);
		if (auto track = getTrackAt(i))
		{

			for (auto clip : track->getClips())
			{
				if (auto* midiClip = dynamic_cast<tracktion_engine::MidiClip*> (clip))
				{
					auto start = tempoSequence.beatsToTime(clip->getStartBeat()).inSeconds();
					auto end = tempoSequence.beatsToTime(clip->getEndBeat()).inSeconds();
					if (start < xMax && end > xMin)
					{
						itemsFound.add(clip);
					}
				}
			}
		}
	}

	DBG("Lasso found " << itemsFound.size() << " items in area: " << area.toString());
}

void TrackListComponent::lassoSelectionChanged(SelectedItemSet<te::Clip*>& newSelection)
{
	uiContext.selectionManager.select(newSelection.getItemArray());
}

void TrackListComponent::updateSnapType()
{
	auto& edit = uiContext.extendedEdit.getEdit();
	auto secondsPerPixel = uiContext.extendedEdit.getSecondsPerPixel();
	auto leftSeconds = te::TimePosition::fromSeconds(viewport.getViewPositionX() * secondsPerPixel);
	te::TempoSequence& tempoSequence = edit.tempoSequence;
	bool isTripletOverride = false;
	auto displayFormat = edit.getTimecodeFormat();
	snapType = fumeHelpers::getBestSnapType(displayFormat, fumeUI::timelineMinimumPixelsPerSnap, tempoSequence.getTempoAt(leftSeconds), te::TimeDuration::fromSeconds(secondsPerPixel), isTripletOverride);
}

void TrackListComponent::updateBounds()
{
	auto& extendedEdit = uiContext.extendedEdit;

	auto secondsPerPixel = extendedEdit.getSecondsPerPixel();
	auto trackHeight = extendedEdit.getTrackHeight();

	auto width = secondsPerPixel * extendedEdit.getEdit().getMaximumLength().inSeconds();
	auto height = trackHeight * getTrackCount();
	setBounds(0, 0, width, height);
}
