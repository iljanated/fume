#pragma once
#include "TrackComponent.h"
#include "MainComponent.h"
#include "MidiClipComponent.h"
#include "AudioClipComponent.h"

namespace te = tracktion;

TrackComponent::TrackComponent(te::AudioTrack& t, fumeUI::UIContext& context)
    : te::ValueTreeObjectList<Helpers::AsyncValueTreeItem<ClipComponent>>(t.state),
    track(t),
    uiContext(context)
{
    DBG("TrackComponent::TrackComponent: " + t.getName());

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
    auto localBounds = getLocalBounds();
    
    g.setColour(juce::Colours::white.withAlpha(0.2f));
	g.fillRect(localBounds);
    g.drawRect(localBounds);
}

void TrackComponent::resized()
{
    auto localBounds = getLocalBounds();

    float zoom = track.edit.state.getProperty(FumeIDs::timelineHorizontalZoom, 1.0f);
    auto beatWidth = FUME_BEAT_WIDTH * zoom;

    for (auto objectWrapper : objects)
    {
        if (auto object = objectWrapper->getObject())
        {
			te::Clip& clip = object->getClip();
			auto beatRange = clip.getEditBeatRange();

            object->setBounds((int)(beatRange.getStart().inBeats() * beatWidth), 0, (int)(beatRange.getLength().inBeats() * beatWidth), localBounds.getHeight());
        }
    }
}

bool TrackComponent::isSuitableType(const juce::ValueTree& v) const
{
    return v.hasType(te::IDs::MIDICLIP) || v.hasType(te::IDs::AUDIOCLIP);
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