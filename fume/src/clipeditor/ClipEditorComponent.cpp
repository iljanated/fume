#pragma once
#include "ClipEditorComponent.h"

namespace te = tracktion;

ClipEditorComponent::ClipEditorComponent(fumeUI::UIContext& context)
    : uiContext(context)
{
	setWantsKeyboardFocus(true);

	uiContext.selectionManager.addChangeListener(this);

	viewport.setWantsKeyboardFocus(false);
	viewport.getHorizontalScrollBar().setMouseClickGrabsKeyboardFocus(false);
	viewport.getVerticalScrollBar().setMouseClickGrabsKeyboardFocus(false);
	addAndMakeVisible(viewport);
}

ClipEditorComponent::~ClipEditorComponent()
{
	uiContext.selectionManager.removeChangeListener(this);
}

void ClipEditorComponent::paint(juce::Graphics& g)
{
	auto bounds = getLocalBounds();
	viewport.setBounds(bounds);
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
				// If the implementation component is not showing the selected clip, create a new one
				if (implementationComponent == nullptr || implementationComponent->getClip() != clip)
				{
					implementationComponent = std::make_unique<MidiClipEditorComponent>(clip, uiContext);
					viewport.setViewedComponent(implementationComponent.get(), true);
					viewport.setScrollBarsShown(true, true, true, true);
					implementationComponent->setSize(2024, 1600);
					return;
				}
				// If the implementation component is already showing the selected clip, do nothing
					return;
			}
		}
		// If no clip is selected, reset the implementation component
		implementationComponent.reset();
	}
}