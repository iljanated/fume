#include "TrackListContainerComponent.h"
#pragma once

TrackListContainerComponent::TrackListContainerComponent(fumeUI::UIContext& context)
	: trackListComponent(context, trackListViewport), trackHeadersComponent(context), timelineComponent(context, trackListComponent)
{
	addAndMakeVisible(timelineComponent);
	addAndMakeVisible(trackHeadersComponent);

	trackListComponent.setSize(2024, 1600);
	trackListComponent.addComponentListener(this);

	trackListViewport.setViewedComponent(&trackListComponent, false);
	addAndMakeVisible(trackListViewport);
}

TrackListContainerComponent::~TrackListContainerComponent()
{
}

void TrackListContainerComponent::paint(juce::Graphics& g)
{
}

void TrackListContainerComponent::resized()
{
	auto localBounds = getLocalBounds();
	auto timelineBounds = localBounds.removeFromTop(fumeUI::timelineTimelineHeight);
	timelineBounds.removeFromLeft(fumeUI::timelineTrackHeadWidth);
	timelineComponent.setBounds(timelineBounds);
	trackHeadersComponent.setBounds(localBounds.removeFromLeft(fumeUI::timelineTrackHeadWidth));
	trackListViewport.setBounds(localBounds);
}

void TrackListContainerComponent::componentMovedOrResized(juce::Component& component, bool wasMoved, bool wasResized)
{
	if (&component == &trackListComponent)
	{
		trackHeadersComponent.setTrackOffset(trackListComponent.getY());
		timelineComponent.setOffset(trackListComponent.getX());
	}
}
