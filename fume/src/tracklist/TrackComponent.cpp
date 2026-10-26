#pragma once
#include "TrackComponent.h"
#include "MainComponent.h"
#include "MidiClipComponent.h"
#include "AudioClipComponent.h"

namespace te = tracktion;

TrackComponent::TrackComponent(te::AudioTrack& t, fumeUI::UIContext& context)
    : te::ValueTreeObjectList<Helpers::AsyncValueTreeItem<ClipComponent>>(t.state),
    state(t.state),
    track(t),
    uiContext(context)
{
	rebuildObjects();
}

TrackComponent::~TrackComponent()
{
	freeObjects();
}

te::AudioTrack& TrackComponent::getTrack()
{
	return track;
}

void TrackComponent::paint(juce::Graphics& g)
{
}

void TrackComponent::resized()
{
    auto localBounds = getLocalBounds();

    float zoomX = uiContext.edit.state.getProperty(FumeIDs::ZoomX, 1.0f);
	
	auto pixelsPerSecond = zoomX / fumeUI::timelineSecondsPerPixel;
    
    for (auto objectWrapper : objects)
    {
        if (auto object = objectWrapper->getObject())
        {
            te::Clip& clip = object->getClip();
			auto clipPosition = clip.getPosition();
            object->setBounds((int)(clipPosition.getStart().inSeconds() * pixelsPerSecond), 0, (int)(clipPosition.getLength().inSeconds() * pixelsPerSecond), localBounds.getHeight());
			DBG("Clip: " << clip.getName() << " Start: " << clipPosition.getStart().inSeconds() << " Length: " << clipPosition.getLength().inSeconds() << " Bounds: " << object->getBounds().toString());
        }
    }
}

bool TrackComponent::isSuitableType(const juce::ValueTree& v) const
{
    return v.hasType(te::IDs::MIDICLIP)
        || v.hasType(te::IDs::AUDIOCLIP);
}

Helpers::AsyncValueTreeItem<ClipComponent>* TrackComponent::createNewObject(const juce::ValueTree& v)
{
    if (v.hasType(te::IDs::MIDICLIP)) 
    {
        return new Helpers::AsyncValueTreeItem<ClipComponent>(v,
            [this](auto state)
            {
                auto t = static_cast<te::MidiClip*>(Helpers::findObjectForState(track.getClips(), state));
                assert(t);
                auto tc = std::make_unique<MidiClipComponent>(*t, uiContext);
                addAndMakeVisible(*tc);
                asyncResizer.resizeAsync();
                return tc;
            });
    }
    else
    {
        return new Helpers::AsyncValueTreeItem<ClipComponent>(v,
            [this](auto state)
            {
                auto t = static_cast<te::WaveAudioClip*>(Helpers::findObjectForState(track.getClips(), state));
                assert(t);
                auto tc = std::make_unique<AudioClipComponent>(*t);
                addAndMakeVisible(*tc);
                asyncResizer.resizeAsync();
                return tc;
            });
    }
}

void TrackComponent::deleteObject(Helpers::AsyncValueTreeItem<ClipComponent>* obj)
{
	delete obj;
}

void TrackComponent::newObjectAdded(Helpers::AsyncValueTreeItem<ClipComponent>*)
{
	asyncResizer.resizeAsync();
}

void TrackComponent::objectRemoved(Helpers::AsyncValueTreeItem<ClipComponent>*)

{
	asyncResizer.resizeAsync();
}

void TrackComponent::objectOrderChanged()
{
	asyncResizer.resizeAsync();
}

void TrackComponent::valueTreePropertyChanged(juce::ValueTree& treeWhosePropertyHasChanged, const juce::Identifier& property)
{
	// call super class implementation to handle property changes
    te::ValueTreeObjectList<Helpers::AsyncValueTreeItem<ClipComponent>>::valueTreePropertyChanged(treeWhosePropertyHasChanged, property);

	if(treeWhosePropertyHasChanged.hasType(te::IDs::MIDICLIP) && property == te::IDs::length)
    {
		asyncResizer.resizeAsync();
    }
}
