#pragma once
#include "TrackHeadersComponent.h"

TrackHeadersComponent::TrackHeadersComponent(fumeUI::UIContext& context)
	: te::ValueTreeObjectList<Helpers::AsyncValueTreeItem<TrackHeaderComponent>>(context.edit.state), uiContext(context)
{
	rebuildObjects();
}

TrackHeadersComponent::~TrackHeadersComponent()
{
	freeObjects();
}

void TrackHeadersComponent::paint(juce::Graphics& g)
{
	g.fillAll(juce::Colours::darkgrey);
}

void TrackHeadersComponent::resized()
{
	auto localBounds = getLocalBounds();
	auto rect = juce::Rectangle{0, trackOffset, localBounds.getWidth(), objects.size() * FUME_TIMELINE_TRACK_HEIGHT};
	for (auto objectWrapper : objects)
	{
		if (auto object = objectWrapper->getObject())
		{
			object->setBounds(rect.removeFromTop(FUME_TIMELINE_TRACK_HEIGHT));
		}
	}
}

void TrackHeadersComponent::setTrackOffset(int offset) { trackOffset = offset; resized(); }

bool TrackHeadersComponent::isSuitableType(const juce::ValueTree& v) const
{
	return v.hasType(te::IDs::TRACK);
}

Helpers::AsyncValueTreeItem<TrackHeaderComponent>* TrackHeadersComponent::createNewObject(const juce::ValueTree& v)
{
	return new Helpers::AsyncValueTreeItem<TrackHeaderComponent>(v,
		[this](auto state)
		{
			auto t = Helpers::findObjectForState(te::getAudioTracks(uiContext.edit), state);
			assert(t);
			auto tc = std::make_unique<TrackHeaderComponent>(*t, uiContext);
			addAndMakeVisible(*tc);
			asyncResizer.resizeAsync();
			return tc;
		});
}

void TrackHeadersComponent::deleteObject(Helpers::AsyncValueTreeItem<TrackHeaderComponent>* obj)
{
	delete obj;
}

void TrackHeadersComponent::newObjectAdded(Helpers::AsyncValueTreeItem<TrackHeaderComponent>*)
{
	asyncResizer.resizeAsync();
}

void TrackHeadersComponent::objectRemoved(Helpers::AsyncValueTreeItem<TrackHeaderComponent>*)
{
	asyncResizer.resizeAsync();
}

void TrackHeadersComponent::objectOrderChanged()
{
	asyncResizer.resizeAsync();
}

void TrackHeaderComponent::paint(juce::Graphics& g)
{
	auto localBounds = getLocalBounds();
	g.fillAll(juce::Colours::grey);
	g.setColour(juce::Colours::darkgrey);
	auto textBounds = localBounds.reduced(10);
	g.drawText(track.getName(), textBounds, juce::Justification::centredLeft);
	g.drawRect(localBounds, 1);
}
