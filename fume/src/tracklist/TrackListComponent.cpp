#include "TrackListComponent.h"
#include "toolstates/TrackListSelectToolState.h"
#include "toolstates/TrackListWriteToolState.h"
#include "toolstates/TrackListDeleteToolState.h"
#include "toolstates/TrackListZoomToolState.h"

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

	float zoomX = uiContext.edit.state.getProperty(FumeIDs::ZoomX, 1.0f);
	float zoomY = uiContext.edit.state.getProperty(FumeIDs::ZoomY, 1.0f);
	auto beatWidth = fumeUI::timelineBeatWidth * zoomX;
	auto trackHeight = fumeUI::timelineTrackHeight * zoomY;

	double minimumPixelWidthForGridLines = 20.0;

	te::TempoSequence dummySequence(uiContext.edit);
	dummySequence.createEmptyState();
	auto firstTempo = dummySequence.insertTempo(te::BeatPosition::fromBeats(0.0), 120.0, 0.0f);
	
	double secondsPerBeat = 60.0 / firstTempo->getBpm();
	double pixelsPerSecond = beatWidth / secondsPerBeat;
	double secondsPerPixel = 1.0 / pixelsPerSecond;

	bool isTripletOverride = false;
	auto displayFormat = uiContext.edit.getTimecodeFormat();
	auto snapType = displayFormat.getBestSnapType(*firstTempo, te::TimeDuration::fromSeconds(secondsPerPixel), isTripletOverride);
		
	DBG("TrackListComponent::paint: snapType: " << snapType.getDescription(*firstTempo, isTripletOverride) << ", level: " << snapType.getLevel() << ", type: " << static_cast<int>(snapType.type));	
	

	g.fillAll(juce::Colours::lightgrey);

	fumeUI::drawGrid(g, clipBounds, *this, juce::Colours::lightgrey.darker(0.8f), juce::Colours::lightgrey.darker(0.2f), false);
	
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
	if (property == FumeIDs::ZoomX || property == FumeIDs::ZoomY)
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
