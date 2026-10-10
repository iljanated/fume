#pragma once
#include "ClipEditorComponent.h"

namespace te = tracktion;

ClipEditorComponent::ClipEditorComponent(fumeUI::UIContext& context)
    : uiContext(context)
{
	uiContext.selectionManager.addChangeListener(this);
}

ClipEditorComponent::~ClipEditorComponent()
{
	uiContext.selectionManager.removeChangeListener(this);
}

void ClipEditorComponent::paint(juce::Graphics& g)
{
	g.fillAll(juce::Colours::darkgrey);
	g.setColour(juce::Colours::white);
	g.drawText("MIDI Sequence", getLocalBounds(), juce::Justification::centred, true);
}


void ClipEditorComponent::resized()
{
}

void ClipEditorComponent::changeListenerCallback(juce::ChangeBroadcaster* source)
{
	auto& selectionManager = uiContext.selectionManager;

	if (source == &selectionManager)
	{
		for (auto* selectable : selectionManager.getSelectedObjects())
		{
			if (te::MidiClip* clip = dynamic_cast<te::MidiClip*> (selectable))
			{
				if (implementationComponent == nullptr || implementationComponent->getClip() != clip)
				{
					implementationComponent = std::make_unique<MidiClipEditorComponent>(clip, uiContext);
					addAndMakeVisible(implementationComponent.get());
					return;
				}
			}
		}
	}
}