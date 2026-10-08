#include "TrackListComponent.h"
#include "toolstates/TrackListSelectToolState.h"
#include "toolstates/TrackListWriteToolState.h"
#include "toolstates/TrackListDeleteToolState.h"

namespace te = tracktion;

TrackListComponent::TrackListComponent(te::Edit& e, InputManager& i, juce::Viewport& v)
	: te::ValueTreeObjectList<Helpers::AsyncValueTreeItem<TrackComponent>>(e.state),
	edit(e),
	inputManager(i),
	viewport(v)
{
	selectionManager.addChangeListener(this);
	
	selectToolState = std::make_unique<TrackListSelectToolState>(*this, inputManager, *this);
	writeToolState = std::make_unique<TrackListWriteToolState>(*this, inputManager, *this);
	deleteToolState = std::make_unique<TrackListDeleteToolState>(*this, inputManager, *this);

	edit.state.setProperty(FumeIDs::timelineHorizontalZoom, 1.0f, &edit.getUndoManager());
	edit.state.setProperty(FumeIDs::timelineVerticalZoom, 1.0f, &edit.getUndoManager());
	edit.state.setProperty(FumeIDs::timelineQuantisation, 1.0f, &edit.getUndoManager());

	inputManager.add(this);
	currentPositionMarker.setFill(Colours::white.withAlpha(0.85f));
	addAndMakeVisible(currentPositionMarker);

	drawableCursor.setFill(Colours::white.withAlpha(0.85f));
	drawableCursor.addComponentListener(this);
	addAndMakeVisible(drawableCursor);

	rebuildObjects();

	activateToolState(selectToolState.get());

	startTimerHz(30);
}

TrackListComponent::~TrackListComponent()
{
	inputManager.remove(this);
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

void TrackListComponent::activateToolState(ToolState<TrackListComponent>* newState)
{
	if (currentToolState)
	{
		currentToolState->onExitState();
	}
	currentToolState = newState;
	if (currentToolState)
	{
		currentToolState->onEnterState();
	}
}

void TrackListComponent::onToolStateFinished()
{
	activateToolState(selectToolState.get());
}

void TrackListComponent::onInputAction(InputManagerActionId actionId, int inputMask, bool isActive)
{
	if (actionId == InputManagerActionId::SELECT && isActive && currentToolState != selectToolState.get())
	{
		activateToolState(selectToolState.get());
	}
	else if (actionId == InputManagerActionId::WRITE && isActive && currentToolState != writeToolState.get())
	{
		activateToolState(writeToolState.get());
	}
	else if (actionId == InputManagerActionId::DELETE && isActive && currentToolState != deleteToolState.get())
	{
		activateToolState(deleteToolState.get());
	}

	if (currentToolState)
	{
		currentToolState->onInputAction(actionId, inputMask, isActive);
	}
}

void TrackListComponent::paint(juce::Graphics& g)
{
	auto localBounds = getLocalBounds();

	g.setColour(juce::Colours::white.withAlpha(0.2f));
	g.drawRect(localBounds);

	g.setColour(Colours::white.withMultipliedAlpha(0.5f));
	g.fillRectList(gridBackgroundsLight);
	g.setColour(Colours::white.withMultipliedAlpha(0.2f));
	g.fillRectList(gridBackgroundsDark);

	g.setColour(Colours::white.withMultipliedAlpha(0.7f));
	g.strokePath(gridLines, juce::PathStrokeType(1.0f));
}

void TrackListComponent::resized()
{
	auto localBounds = getLocalBounds();
	for (auto objectWrapper : objects)
	{
		if (auto object = objectWrapper->getObject())
		{
			object->setBounds(localBounds.removeFromTop(FUME_TRACK_HEIGHT));
		}
	}
	updatePaths();
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
			auto t = Helpers::findObjectForState(te::getAudioTracks(edit), state);
			assert(t);
			auto tc = std::make_unique<TrackComponent>(*t);
			addAndMakeVisible(*tc);
			drawableCursor.toFront(false);
			currentPositionMarker.toFront(false);
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

void TrackListComponent::timerCallback()
{
	updateCurrentPositionMarker();
}

void TrackListComponent::updateCurrentPositionMarker()
{
	float zoom = edit.state.getProperty(FumeIDs::timelineHorizontalZoom, 1.0f);
	auto position = transport.getPosition();
	te::TempoSequence& tempoSequence = edit.tempoSequence;
	auto beats = tempoSequence.toBeats(position);
	auto offset = (float)beats.inBeats() * FUME_BEAT_WIDTH * zoom;
	currentPositionMarker.setRectangle(Rectangle<float>(offset, 0, 1.5f, (float)(getHeight())));
}

void TrackListComponent::updatePaths()
{
	gridBackgroundsLight.clear();
	gridBackgroundsDark.clear();
	float zoom = edit.state.getProperty(FumeIDs::timelineHorizontalZoom, 1.0f);

	auto height = (float)getHeight();
	auto beatWidth = FUME_BEAT_WIDTH * zoom;

	auto beatsPerRectangle = 16;

	for (int i = 0; i < 30; i = i + beatsPerRectangle)
	{
		auto offset = (float)i * beatWidth;

		if (i % (beatsPerRectangle * 2) == 0)
		{
			gridBackgroundsDark.add(Rectangle<float>(offset, 0, beatWidth * beatsPerRectangle, height));
		}
		else
		{
			gridBackgroundsLight.add(Rectangle<float>(offset, 0, beatWidth * beatsPerRectangle, height));
		}
	}

	auto beatsPerGridLine = 4;

	gridLines.clear();

	for (int i = 0; i < 30; i = i + beatsPerGridLine)
	{
		auto offset = (float)i * beatWidth;

		gridLines.startNewSubPath(offset, 0);
		gridLines.lineTo(offset, height);
	}
}

void TrackListComponent::changeListenerCallback(juce::ChangeBroadcaster* source)
{
	if (source == &selectionManager)
	{
		repaint();
	}
}

void TrackListComponent::componentMovedOrResized(Component& component, bool wasMoved, bool wasResized)
{
	if (&component == &drawableCursor)
	{
		auto x = cursor.getX() * FUME_BEAT_WIDTH;
		auto y = cursor.getY() * FUME_TRACK_HEIGHT;



		juce::Rectangle<int> viewArea = viewport.getViewArea();
		int currentScrollX = viewArea.getX();
		int currentScrollY = viewArea.getY();

		int viewWidth = viewArea.getWidth();
		int viewHeight = viewArea.getHeight();

		int maxX = currentScrollX + viewWidth;
		int maxY = currentScrollY + viewHeight;

		int targetScrollX = currentScrollX;
		int targetScrollY = currentScrollY;

		if(x < currentScrollX + FUME_SCROLL_PADDING)
		{
			targetScrollX = x - FUME_SCROLL_PADDING;
		}
		else if(x > maxX - FUME_SCROLL_PADDING)
		{
			targetScrollX = x - viewWidth + FUME_SCROLL_PADDING;
		}

		if(y < currentScrollY + FUME_SCROLL_PADDING)
		{
			targetScrollY = y - FUME_SCROLL_PADDING;
		}
		else if(y + FUME_TRACK_HEIGHT > maxY - FUME_SCROLL_PADDING)
		{
			targetScrollY = y + FUME_TRACK_HEIGHT - viewHeight + FUME_SCROLL_PADDING;
		}

		if (targetScrollX != currentScrollX || targetScrollY != currentScrollY)
		{
			viewport.setViewPosition(targetScrollX, targetScrollY);
		}
	}
}

void TrackListComponent::resizeViewport()
{
	te::TimePosition timePosition = te::TimePosition::fromSeconds(edit.getMaximumLength().inSeconds());
	auto width = (int)(edit.tempoSequence.timeToBeats(timePosition).inBeats() * FUME_BEAT_WIDTH);
	auto height = getTrackCount() * FUME_TRACK_HEIGHT + FUME_SCROLL_PADDING;
	setBounds(0, 0, width, height);

}
