#pragma once

#include "MidiClipEditorComponent.h"

MidiClipEditorComponent::MidiClipEditorComponent(te::MidiClip* clip, fumeUI::UIContext& context)
	: midiClip(clip), uiContext(context)
{
}

MidiClipEditorComponent::~MidiClipEditorComponent()
{
	DBG("destroying MidiClipEditorComponent");
}

void MidiClipEditorComponent::paint(juce::Graphics& g)
{
	g.fillAll(juce::Colours::darkgrey);
}

te::Clip* MidiClipEditorComponent::getClip()
{
    return midiClip;
}