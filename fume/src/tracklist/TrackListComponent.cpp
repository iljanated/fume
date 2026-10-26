#include "TrackListComponent.h"
#include "TrackListWriteToolState.h"
#include "TrackListDeleteToolState.h"
#include "TrackListSelectToolState.h"
#include "TrackListZoomToolState.h"

namespace te = tracktion;

TrackListComponent::TrackListComponent(fumeUI::UIContext& context, juce::Viewport& v)
	: GridEditor<TrackListComponent>(context, v, FumeIDs::TrackListSelectToolState), te::ValueTreeObjectList<Helpers::AsyncValueTreeItem<TrackComponent>>(context.edit.state)
{
	addAndMakeVisible(currentPositionMarker);
	currentPositionMarker.setFill(Colours::white.withAlpha(0.85f));
	currentPositionMarker.setAlwaysOnTop(true);

	uiContext.selectionManager.addChangeListener(this);
	
	toolStateManager.addToolState(std::make_unique<TrackListSelectToolState>(this));
	toolStateManager.addToolState(std::make_unique<TrackListWriteToolState>(this));
	toolStateManager.addToolState(std::make_unique<TrackListDeleteToolState>(this));
	toolStateManager.addToolState(std::make_unique<TrackListZoomToolState>(this));

	//uiContext.edit.state.setProperty(FumeIDs::ZoomX, 1.0f, &uiContext.edit.getUndoManager());
	//uiContext.edit.state.setProperty(FumeIDs::ZoomY, 1.0f, &uiContext.edit.getUndoManager());
	//uiContext.edit.state.setProperty(FumeIDs::Quantisation, 1.0f, &uiContext.edit.getUndoManager());

	rebuildObjects();

	toolStateManager.activateToolStateByIdentifier(FumeIDs::TrackListSelectToolState);

	updateSnapType();

	startTimerHz(30);
}

TrackListComponent::~TrackListComponent()
{
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

	auto leftSeconds = te::TimePosition::fromSeconds(clipBounds.getX() * secondsPerPixel);
	te::TempoSequence& tempoSequence = uiContext.edit.tempoSequence;
	
	float zoomY = extendedEdit.zoomY;
	auto trackHeight = fumeUI::timelineTrackHeight * zoomY;

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
	for (auto objectWrapper : objects)
	{
		if (auto object = objectWrapper->getObject())
		{
			object->setBounds(localBounds.removeFromTop(fumeUI::timelineTrackHeight * extendedEdit.zoomY));
		}
	}
}

void TrackListComponent::focusGained(FocusChangeType cause)
{
	GridEditor<TrackListComponent>::focusGained(cause);
	repaint();
}

void TrackListComponent::focusLost(FocusChangeType cause)
{
	GridEditor<TrackListComponent>::focusLost(cause);
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
			auto t = Helpers::findObjectForState(te::getAudioTracks(uiContext.edit), state);
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
	resizeViewport();
	asyncResizer.resizeAsync();
}

void TrackListComponent::objectRemoved(Helpers::AsyncValueTreeItem<TrackComponent>*)
{
	resizeViewport();
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
		float zoomX = treeWhosePropertyHasChanged.getProperty(FumeIDs::ZoomX, 1.0f);
		secondsPerPixel = fumeUI::timelineSecondsPerPixel / zoomX;
		updateSnapType();
		resizeViewport();
		asyncResizer.resizeAsync();
	}
	else if (property == FumeIDs::ZoomY)
	{
		resizeViewport();
		asyncResizer.resizeAsync();
	}
}

void TrackListComponent::timerCallback()
{
	updateCurrentPositionMarker();
}

void TrackListComponent::updateCurrentPositionMarker()
{
	float zoom = uiContext.edit.state.getProperty(FumeIDs::ZoomX, 1.0f);
	auto position = uiContext.edit.getTransport().getPosition();
	te::TempoSequence& tempoSequence = uiContext.edit.tempoSequence;
	auto beats = tempoSequence.toBeats(position);
	auto offset = (float)beats.inBeats() * fumeUI::timelineBeatWidth * zoom;
	currentPositionMarker.setRectangle(Rectangle<float>(offset, 0, 1.5f, (float)(getHeight())));
}

void TrackListComponent::changeListenerCallback(juce::ChangeBroadcaster* source)
{
	if (source == &uiContext.selectionManager)
	{
		repaint();
	}
}

void TrackListComponent::selectItemsInBox(const juce::Rectangle<double>& selectionBox)
{
	auto& selectionManager = uiContext.selectionManager;

	auto xMin = selectionBox.getX();
	auto xMax = selectionBox.getRight();
	auto yMin = selectionBox.getY();
	auto yMax = selectionBox.getBottom();

	for (auto* selectable : uiContext.selectionManager.getSelectedObjects())
	{
		if (te::Clip* clip = dynamic_cast<te::Clip*> (selectable))
		{
			if (auto* track = clip->getTrack())
			{
				int trackIndex = track->getIndexInEditTrackList();
				if (trackIndex < yMin || trackIndex > yMax || clip->getEndBeat().inBeats() < xMin || clip->getStartBeat().inBeats() > xMax)
				{
					selectionManager.deselect(clip);
				}
			}
			else
			{
				selectionManager.deselect(clip);
			}
		}
	}

	for (int i = yMin; i <= yMax; ++i)
	{
		if (auto track = getTrackAt(i))
		{
			for (auto clip : track->getClips())
			{
				if (clip->getStartBeat().inBeats() <= xMax && clip->getEndBeat().inBeats() >= xMin)
				{
					selectionManager.addToSelection(*clip);
				}
			}
		}
	}
}

juce::Point<double> TrackListComponent::getGridSize() const
{
	auto& edit = uiContext.edit;
	te::TimePosition timePosition = te::TimePosition::fromSeconds(edit.getMaximumLength().inSeconds());
	auto width = edit.tempoSequence.timeToBeats(timePosition).inBeats();
	auto height = static_cast<double>(getTrackCount());

	return { width, height };
}

juce::Point<double> TrackListComponent::getUnitSize() const 
{ 
	return { 
		fumeUI::timelineBeatWidth * extendedEdit.zoomX, 
		static_cast<double>(fumeUI::timelineTrackHeight)* extendedEdit.zoomY
	}; 
}

void TrackListComponent::findLassoItemsInArea(Array<te::Clip*>& itemsFound, const Rectangle<int>& area)
{
	auto xMin = area.getX();
	auto xMax = area.getRight();
	auto yMin = area.getY();
	auto yMax = area.getBottom();

	for (int i = yMin; i <= yMax; ++i)
	{
		if (auto track = getTrackAt(i))
		{
			for (auto clip : track->getClips())
			{
				auto timeRange = clip->getEditTimeRange();
				if (timeRange.getStart().inSeconds() * getUnitSize().x <= xMax && timeRange.getEnd().inSeconds() * getUnitSize().x >= xMin)
				{
					itemsFound.add(clip);
				}
			}
		}
	}
}

void TrackListComponent::lassoSelectionChanged(SelectedItemSet<te::Clip*>& newSelection)
{
}

void TrackListComponent::updateSnapType()
{
	auto leftSeconds = te::TimePosition::fromSeconds(viewport.getViewPositionX() * secondsPerPixel);
	te::TempoSequence& tempoSequence = uiContext.edit.tempoSequence;
	bool isTripletOverride = false;
	auto displayFormat = uiContext.edit.getTimecodeFormat();
	snapType = fumeHelpers::getBestSnapType(displayFormat, fumeUI::timelineMinimumPixelsPerSnap, tempoSequence.getTempoAt(leftSeconds), te::TimeDuration::fromSeconds(secondsPerPixel), isTripletOverride);
}