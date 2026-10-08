#pragma once
#include "MidiSequenceComponent.h"

namespace te = tracktion;

MidiSequenceComponent::MidiSequenceComponent()
{
}

MidiSequenceComponent::~MidiSequenceComponent()
{
	if (clipState.isValid())
	{
		clipState.removeListener(this);
	}
}

void MidiSequenceComponent::paint(juce::Graphics& g)
{
	g.fillAll(juce::Colours::darkgrey);
	g.setColour(juce::Colours::white);
	g.drawText("MIDI Sequence", getLocalBounds(), juce::Justification::centred, true);
}


void MidiSequenceComponent::resized()
{
}

void MidiSequenceComponent::visibilityChanged()
{
	if (isVisible())
	{
		// Hier kun je code toevoegen die wordt uitgevoerd wanneer het component zichtbaar wordt
		DBG("MidiSequenceComponent is now visible");
	}
	else
	{
		// Hier kun je code toevoegen die wordt uitgevoerd wanneer het component niet meer zichtbaar is
		DBG("MidiSequenceComponent is now hidden");
	}
}

void MidiSequenceComponent::setMidiClip(te::MidiClip* newClip)
{
	if (clipState.isValid())
	{
		clipState.removeListener(this);
	}
	if (newClip != nullptr)
	{
		clipState = newClip->state;
		clipState.addListener(this);
	}
	else
	{
		clipState = juce::ValueTree();
	}

	repaint();
}

void MidiSequenceComponent::valueTreeParentChanged(juce::ValueTree& treeWhoseParentHasChanged)
{
	if (clipState.isValid() && treeWhoseParentHasChanged == clipState)
	{
		if (!clipState.getParent().isValid())
		{
			setClip(nullptr);
		}
	}
}