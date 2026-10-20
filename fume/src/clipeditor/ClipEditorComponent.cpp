#pragma once
#include "ClipEditorComponent.h"
#include "MidiClipEditorComponent.h"

namespace te = tracktion;

ClipEditorComponent::ClipEditorComponent(fumeUI::UIContext& context)
    : uiContext(context)
{
	setWantsKeyboardFocus(true);

	uiContext.selectionManager.addChangeListener(this);
}

ClipEditorComponent::~ClipEditorComponent()
{
	uiContext.selectionManager.removeChangeListener(this);
}

void ClipEditorComponent::paint(juce::Graphics& g)
{
	auto bounds = getLocalBounds();
	if (!implementationComponent)
	{
		g.fillAll(juce::Colours::darkgrey);
	}
}


void ClipEditorComponent::resized()
{
	auto bounds = getLocalBounds();
	if (implementationComponent)
	{
		implementationComponent->setBounds(bounds);
	}
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
				// If the implementation component is not showing the selected clip, create a new one
				if (implementationComponent == nullptr || implementationComponent->getClip() != clip)
				{
					implementationComponent = std::make_unique<MidiClipEditorComponent>(clip, uiContext);
					addAndMakeVisible(implementationComponent.get());
					resized();
					return;
				}
				// If the implementation component is already showing the selected clip, do nothing
					return;
			}
		}
		// If no clip is selected, reset the implementation component
		removeChildComponent(implementationComponent.get());
		implementationComponent.reset();
	}
}