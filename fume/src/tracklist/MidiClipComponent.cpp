#pragma once
#include "MidiClipComponent.h"
#include "TrackListComponent.h"

namespace te = tracktion;

MidiClipComponent::MidiClipComponent(te::MidiClip& c, fumeUI::UIContext& context)
    : clip(c), uiContext(context)
{
}

MidiClipComponent::~MidiClipComponent()
{}

te::Clip& MidiClipComponent::getClip()
{
	return clip;
}

void MidiClipComponent::paint(juce::Graphics& g)
{
    bool isSelected = false;
    if (auto* parent = findParentComponentOfClass<TrackListComponent>())
    {
        isSelected = uiContext.selectionManager.isSelected(clip);
    }

    auto localBounds = getLocalBounds();

    if(isSelected)
	{
		g.setColour(clip.getColour().withMultipliedAlpha(0.5f));
	}
	else
	{
		g.setColour(clip.getColour().withMultipliedAlpha(0.3f));
	}
    g.fillRect(localBounds);
	g.setColour(juce::Colours::black);
    g.drawRect(localBounds);
}

void MidiClipComponent::resized()
{
}
