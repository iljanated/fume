#pragma once
#include "TrackHeadersComponent.h"

TrackHeadersComponent::TrackHeadersComponent(fumeUI::UIContext& context)
	: te::ValueTreeObjectList<Helpers::AsyncValueTreeItem<TrackHeaderComponent>>(context.extendedEdit.getState()), uiContext(context)
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
	float zoomY = uiContext.extendedEdit.getState().getProperty(FumeIDs::ZoomY, 1.0f);
	auto trackHeight = fumeUI::timelineTrackHeight * zoomY;
	auto rect = juce::Rectangle{ 0, trackOffset, localBounds.getWidth(), static_cast<int>(objects.size() * trackHeight) };
	for (auto objectWrapper : objects)
	{
		if (auto object = objectWrapper->getObject())
		{
			object->setBounds(rect.removeFromTop(static_cast<int>(trackHeight)));
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
			auto t = Helpers::findObjectForState(te::getAudioTracks(uiContext.extendedEdit.getEdit()), state);
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
